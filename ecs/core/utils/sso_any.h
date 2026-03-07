#pragma once
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <typeinfo>
#include <utility>




namespace nx
{

    class SsoBadCast : public std::bad_cast
    {
    public:
        const char* what() const noexcept override
        {
            return "SsoAny cast failed";
        }
    };
    // Concept to check if type T fits within N bytes
    template <typename T, size_t N>
    concept FitsInStorage = sizeof(T) <= N && std::is_nothrow_move_constructible_v<T>;


    template <size_t N>
    class SsoAny
    {
    private:
        // Storage type: an aligned array of size N
        alignas(std::max_align_t) unsigned char m_storage[N];
        void* m_ptr = nullptr; // Pointer to the object in storage
        void (*m_destructor)(void*) = nullptr;
        void* (*m_copy_constructor)(const void*, void*) = nullptr;
        void* (*m_move_constructor)(void*, void*) = nullptr;
        const std::type_info* m_type = nullptr;



        // Helper to get the address of our storage where the object lives
        void* get_address() { return static_cast<void*>(m_ptr); }
        const void* get_address() const { return static_cast<const void*>(m_ptr); }

        // Type-erased deleter and copier function templates
        template <typename T>
        static void destroy(void* ptr)
        {
            if constexpr (std::is_nothrow_destructible_v<T>)
            {
                reinterpret_cast<T*>(ptr)->~T();
            }
        }

        template <typename T>
        static void* copy_construct(const void* src, void* dst_buf)
        {
            // src is the address of the object to copy
            // dst_buf is the address of the uninitialized storage to construct into
            return new(dst_buf) T(*reinterpret_cast<const T*>(src));
        }

        template <typename T>
        static void* move_construct(void* src_dst, void* dst_buf)
        {
            // src_dst is the address of the object to move from/to (it gets invalidated)
            // dst_buf is the address of the uninitialized storage to construct into
            T* src_obj = reinterpret_cast<T*>(src_dst);
            return new(dst_buf) T(std::move(*src_obj));
        }

    public:
        // Default constructor: creates an empty SsoAny
        SsoAny() = default;

        // Constructor from value
        template <FitsInStorage<N> T>
        SsoAny(T&& value)
        {
            static_assert(sizeof(T) <= N, "Type is too large for SSO buffer");
            m_ptr = new(m_storage) std::decay_t<T>(std::forward<T>(value));
            m_destructor = &destroy<std::decay_t<T>>;
            m_copy_constructor = &copy_construct<std::decay_t<T>>;
            m_move_constructor = &move_construct<std::decay_t<T>>;
            m_type = &typeid(std::decay_t<T>);
        }

        // Copy constructor
        SsoAny(const SsoAny& other)
        {
            if (other.m_ptr != nullptr)
            {
                // Calculate offset of the stored object from the beginning of m_storage
                ptrdiff_t offset = static_cast<const unsigned char*>(other.m_ptr) - other.m_storage;
                void* new_location_in_this = m_storage + offset;

                m_ptr = other.m_copy_constructor(other.get_address(), new_location_in_this);
                m_destructor = other.m_destructor;
                m_copy_constructor = other.m_copy_constructor;
                m_move_constructor = other.m_move_constructor;
                m_type = other.m_type;
            }
            else
            {
                // Other is empty
                m_ptr = nullptr;
                m_destructor = nullptr;
                m_copy_constructor = nullptr;
                m_move_constructor = nullptr;
                m_type = nullptr;
            }
        }

        // Move constructor
        SsoAny(SsoAny&& other) noexcept
        {
            if (other.m_ptr != nullptr)
            {
                // Calculate offset of the stored object from the beginning of m_storage
                ptrdiff_t offset = static_cast<unsigned char*>(other.m_ptr) - other.m_storage;
                void* new_location_in_this = m_storage + offset;

                m_ptr = other.m_move_constructor(other.m_ptr, new_location_in_this);

                // After moving, the 'other' object's destructor must not be called.
                // We reset its pointer to prevent it.
                other.m_ptr = nullptr;

                m_destructor = other.m_destructor;
                m_copy_constructor = other.m_copy_constructor;
                m_move_constructor = other.m_move_constructor;
                m_type = other.m_type;
            }
            else
            {
                m_ptr = nullptr;
                m_destructor = nullptr;
                m_copy_constructor = nullptr;
                m_move_constructor = nullptr;
                m_type = nullptr;
            }
        }

        // Copy assignment operator
        SsoAny& operator=(const SsoAny& other)
        {
            if (this != &other)
            {
                this->~SsoAny(); // Destroy current content
                new(this) SsoAny(other); // Placement new using copy constructor
            }
            return *this;
        }

        // Move assignment operator
        SsoAny& operator=(SsoAny&& other) noexcept
        {
            if (this != &other)
            {
                this->~SsoAny(); // Destroy current content
                new(this) SsoAny(std::move(other)); // Placement new using move constructor
            }
            return *this;
        }

        // Destructor
        ~SsoAny()
        {
            if (m_destructor && m_ptr)
            {
                m_destructor(m_ptr);
            }
        }

        void* get() noexcept
        {
            return get_address();
        }

        // Check if SsoAny holds a value
        bool has_value() const noexcept
        {
            return m_ptr != nullptr;
        }

        // Get type info of the held value
        const std::type_info& type() const noexcept
        {
            return m_type ? *m_type : typeid(void);
        }

        // Cast to the specified type
        template <typename T>
        T& cast()
        {
            if (!has_value() || m_type != &typeid(T))
            {
                throw SsoBadCast{};
            }
            return *static_cast<T*>(get_address());
        }

        template <typename T>
        const T& cast() const
        {
            if (!has_value() || m_type != &typeid(T))
            {
                throw SsoBadCast{};
            }
            return *static_cast<const T*>(get_address());
        }
    };
}
