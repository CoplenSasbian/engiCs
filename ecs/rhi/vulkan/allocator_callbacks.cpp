#include "allocator_callbacks.h"
#include "core/memory/memory.h"

void* vulkan_allocation_func(
    void* pUserData,              
    size_t size,                 
    size_t alignment,             
    VkSystemAllocationScope allocationScope 
) {
    return nx::ecs_malloc_aligned(size, alignment);
}

void vulkan_free_func(
    void* pUserData,
    void* pMemory
) {
    if (pMemory) {
        nx::ecs_free(pMemory);
    }
}

void* vulkan_reallocation_func(
    void* pUserData,
    void* pOriginal, 
    size_t size,  
    size_t alignment,
    VkSystemAllocationScope allocationScope
) {
    return nx::ecs_realloc_aligned(pOriginal, size, alignment);
}





vk::AllocationCallbacks* nx::GetVulkanAllocatorCallbacks() noexcept
{
    struct MimallocUserData {
    };
    static MimallocUserData s_user_data{};

    static vk::AllocationCallbacks callbacks{
        &s_user_data,
        &vulkan_allocation_func,
        &vulkan_reallocation_func,
        &vulkan_free_func,
        nullptr,
        nullptr
    };

    return &callbacks;
}
