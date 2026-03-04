module;
#include <assert.h>
#include<memory_resource>
#include <mimalloc.h>
#include "exception/assert.h"
export module nx.core.memory;
import nx.core.error_code;
export namespace nx
{
    void InitResource();
    void ShutdownResource();


    class CommonResource : public std::pmr::memory_resource
    {
    protected:
        void* do_allocate(size_t bytes, size_t alignment) override;

        void do_deallocate(void* p, size_t, size_t) noexcept override;

        bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override
        {
            return this == &other;
        }
    };

    std::pmr::memory_resource* GetCommandResource();


    class TickResource : public std::pmr::monotonic_buffer_resource
    {
    public:
        TickResource(std::pmr::memory_resource* upstream = GetCommandResource(),
                     size_t initial_buffer_size = 4 * 1024 * 1024)
            : std::pmr::monotonic_buffer_resource(initial_buffer_size, upstream)
        {
        }

        void tick()
        {
            this->release();
        }
    };


    template <typename T>
    std::pmr::memory_resource* GetSynchronizedCacheResource()
    {
        static std::pmr::synchronized_pool_resource cache{GetCommandResource()};
        return &cache;
    }

    template <typename T>
    std::pmr::memory_resource* GetUnsynchronizedCacheResource()
    {
        static std::pmr::unsynchronized_pool_resource cache{GetCommandResource()};
        return &cache;
    }


    template <typename T, typename... Args>
    std::shared_ptr<T> MakeShared(Args&&... args)
    {
        return std::allocate_shared<T, std::pmr::polymorphic_allocator<T>>(
            GetCommandResource(), std::forward<Args>(args)...);
    }

    template <typename T, typename... Args>
    std::shared_ptr<T> MakeSharedWidthSynchronizedCache(Args&&... args)
    {
        return std::allocate_shared<T, std::pmr::polymorphic_allocator<T>>(
            GetSynchronizedCacheResource<T>(), std::forward<Args>(args)...);
    }

    template <typename T, typename... Args>
    std::shared_ptr<T> MakeSharedWidthUnsynchronizedCache(Args&&... args)
    {
        return std::allocate_shared<T, std::pmr::polymorphic_allocator<T>>(
            GetUnsynchronizedCacheResource<T>(), std::forward<Args>(args)...);
    }

    using ::mi_malloc;
    using ::mi_free;

    template <typename T, typename... Args>
    [[nodiscard]]
    T* Make(Args&&... args)
    {
        auto ptr = mi_malloc(sizeof(T));
        auto r = new(ptr) T(std::forward<Args>(args)...);
        return r;
    }

    template <typename T>
    void Destroy(T* ptr)
    {
        if (ptr != nullptr)
        {
            ptr->~T();
            mi_free(ptr);
        }
    }


    template <size_t BufferSize, size_t Alignment = alignof(std::max_align_t), size_t ObjectCount = 1>
    class AlignedStackPmr
    {
        struct Object
        {

            using Deleter = void(void*);
            using Mover = void(void*, void*);

            void* m_objectPtr;
            Deleter* m_deleter;
            Mover * m_mover;

            size_t m_size;
            size_t m_alignment;
        };

    public:
        AlignedStackPmr() : m_buffer{}, m_bufferUsed(0), m_allocatedCount(0)
        {
        }

        AlignedStackPmr(const AlignedStackPmr&) = delete;

        AlignedStackPmr(AlignedStackPmr&& oth) noexcept
            :AlignedStackPmr()
        {
            for (auto i = 0uz; i < oth.m_allocatedCount; ++i)
            {
                auto &moveSrc = oth.m_objects[i];
                size_t total_bytes_needed;
                auto res = allocate_space(moveSrc.m_size, moveSrc.m_alignment, total_bytes_needed);
                if (!res) std::unreachable();
                auto ptr = res.value();
                auto& moveDesc = m_objects[i];
                moveDesc.m_objectPtr = ptr;
                moveDesc.m_size = moveSrc.m_size;
                moveDesc.m_alignment = moveSrc.m_alignment;
                moveDesc.m_deleter = moveSrc.m_deleter;
                moveDesc.m_mover = moveSrc.m_mover;
                moveSrc.m_mover(moveSrc.m_objectPtr, ptr);

                m_allocatedCount++;
                m_bufferUsed += total_bytes_needed;

            }

            oth.m_bufferUsed = 0;
            oth.m_allocatedCount = 0;

        };

       // Result<T>  = std::expected<T,std::error_code>

        Result<void*> allocate_space (size_t size, size_t alignment, size_t& total_bytes_needed) noexcept
        {
            const size_t space_before = m_bufferUsed;
            const size_t space_available = BufferSize - space_before;

            void* unaligned_ptr = m_buffer + space_before;
            size_t available_size = space_available;

            void* aligned_ptr = std::align(alignment, size, unaligned_ptr, available_size);
            if (!aligned_ptr) {
                return UnExpected(make_error_code(CoreErrc::BufferOverflow));
            }

            size_t padding_used = static_cast<char*>(aligned_ptr) - (m_buffer + space_before);
            total_bytes_needed = padding_used + size;

            if (available_size < size) [[unlikely]] {
                return UnExpected(make_error_code(CoreErrc::BufferOverflow));
            }

            return aligned_ptr;
        }


        template <class T, class... Args>
        requires std::is_constructible_v<T, Args...>
        && std::is_nothrow_move_constructible_v<T>
        T* allocate(Args&&... args) noexcept
        {
           Assert(m_allocatedCount < ObjectCount, "The number of allocated objects exceeds the limit!");


            const size_t object_size = sizeof(T);
            const size_t object_alignment = alignof(T);
            size_t total_bytes_needed;

            auto res = allocate_space(object_size,object_alignment,total_bytes_needed);
            Assert(!!res, "Failed to allocate space for object!");
            auto aligned_ptr = res.value();

            T* constructed_obj = new(aligned_ptr) T(std::forward<Args>(args)...);

            auto& stored_obj = m_objects[m_allocatedCount++];
            stored_obj.m_objectPtr = constructed_obj;
            stored_obj.m_deleter = [](void* ptr)
            {
                static_cast<T*>(ptr)->~T();
            };
            stored_obj.m_size = object_size;
            stored_obj.m_alignment = object_alignment;
            stored_obj.m_mover = [](void* src, void* dst)
            {
                new(dst) T(std::move(*static_cast<T*>(src)));
            };
            m_bufferUsed += total_bytes_needed;
            return constructed_obj;
        }

        void Reset() noexcept
        {
            for (size_t i = m_allocatedCount; i > 0; --i)
            {
                m_objects[i - 1].m_deleter(m_objects[i - 1].m_objectPtr);
            }
            m_allocatedCount = 0;
            m_bufferUsed = 0;
        }

        ~AlignedStackPmr()
        {
            Reset();
        }

    private:
        alignas(Alignment) char m_buffer[BufferSize];
        Object m_objects[ObjectCount];
        size_t m_bufferUsed;
        size_t m_allocatedCount;
    };

    template<typename T ,size_t TypeSize,size_t TypeAlign = alignof(std::max_align_t)>
    class Pimpl
    {
        template<class ...Args>
        Pimpl(Args&&... args)
        {
            if (sizeof(T) > TypeSize) std::abort();

            if (auto res = m_buffer.template allocate<T>(std::forward<Args>(args)...))
            {
                m_ptr = res.value();
            }else
            {
                std::abort();
            }
        }

        T* operator->()
        {
            return m_ptr;
        }

    private:
        AlignedStackPmr<TypeSize,TypeAlign> m_buffer;
        T* m_ptr;
    };

}
