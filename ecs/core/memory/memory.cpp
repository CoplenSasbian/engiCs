#include "memory.h"
#include <mimalloc.h>
#include <memory_resource>





void* nx::CommonResource::do_allocate(size_t bytes, size_t alignment)
{

    return mi_malloc_aligned(bytes, alignment);
}

void nx::CommonResource::do_deallocate(void* p, size_t, size_t) noexcept
{
    ecs_free(p);
}



std::pmr::memory_resource* nx::GetCommandResource()
{
    static nx::CommonResource gCommonResource;

    return &gCommonResource;
}

void* nx::ecs_malloc(size_t size)
{
    return ::mi_malloc(size);
}

void nx::ecs_free(void* ptr)
{
    ::mi_free(ptr);
}

void* nx::ecs_malloc_aligned(size_t size, size_t alignment)
{
    return ::mi_malloc_aligned(size, alignment);
}

void* nx::ecs_realloc_aligned(void* pOriginal, size_t size, size_t alignment)
{
    return ::mi_realloc_aligned(pOriginal, size, alignment);
}




void nx::InitResource()
{

    std::pmr::set_default_resource(GetCommandResource());
}

void nx::ShutdownResource()
{
    std::pmr::set_default_resource(nullptr);
}

