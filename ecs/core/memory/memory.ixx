module;
#include<memory_resource>
#include <mimalloc.h>
export module nx.core.memory;

export namespace nx {

    void InitResource();
    void ShutdownResource();


    class CommonResource : public std::pmr::memory_resource {
    protected:
        void* do_allocate(size_t bytes, size_t alignment) override;

        void do_deallocate(void* p, size_t, size_t) noexcept override;

        bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
            return this == &other;
        }
    };

    std::pmr::memory_resource* GetCommandResource();


    class TickResource : public std::pmr::monotonic_buffer_resource {
    public:
        TickResource(std::pmr::memory_resource* upstream = GetCommandResource(),
            size_t initial_buffer_size = 4 * 1024 * 1024)
            : std::pmr::monotonic_buffer_resource(initial_buffer_size, upstream) {}

        void tick() {
            this->release();
        }
    };




    template<typename T>
    std::pmr::memory_resource* GetSynchronizedCacheResource()
    {
        static std::pmr::synchronized_pool_resource cache{ GetCommandResource() };
        return &cache;
    }

    template<typename T>
    std::pmr::memory_resource* GetUnsynchronizedCacheResource()
    {
        static std::pmr::unsynchronized_pool_resource cache{ GetCommandResource() };
        return &cache;
    }


    template<typename T, typename... Args>
    std::shared_ptr<T> MakeShared(Args&&... args)
    {
        return std::allocate_shared<T, std::pmr::polymorphic_allocator<T>>(GetCommandResource(), std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    std::shared_ptr<T> MakeSharedWidthSynchronizedCache(Args&&... args)
    {
        return std::allocate_shared<T, std::pmr::polymorphic_allocator<T>>(GetSynchronizedCacheResource<T>(), std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    std::shared_ptr<T> MakeSharedWidthUnsynchronizedCache(Args&&... args)
    {
        return std::allocate_shared<T, std::pmr::polymorphic_allocator<T>>(GetUnsynchronizedCacheResource<T>(), std::forward<Args>(args)...);
    }

    using ::mi_malloc;
    using ::mi_free;

    template<typename T, typename... Args>
    [[nodiscard]]
    T* Make(Args&& ...args) {
        auto ptr = mi_malloc(sizeof (T));
        auto r = new(ptr) T(std::forward<Args>(args)...);
        return r;
    }

    template<typename T>
    void Destroy(T* ptr) {
        if (ptr != nullptr)
        {
            ptr->~T();
            mi_free(ptr);
        }
    }

}