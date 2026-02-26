module;

#include <mimalloc.h>
#include <memory_resource>

module nx.core.memory;



void* nx::CommonResource::do_allocate(size_t bytes, size_t alignment)
{
    return mi_malloc_aligned(bytes, alignment);
}

void nx::CommonResource::do_deallocate(void* p, size_t, size_t) noexcept
{
    mi_free(p);
}



std::pmr::memory_resource* nx::GetCommandResource()
{
    static nx::CommonResource gCommonResource;

    return &gCommonResource;
}

void nx::InitResource()
{
    std::pmr::set_default_resource(GetCommandResource());
}

void nx::ShutdownResource()
{
    std::pmr::set_default_resource(nullptr);
}
