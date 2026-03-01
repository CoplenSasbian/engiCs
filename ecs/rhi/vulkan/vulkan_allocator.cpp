module;
#include <mimalloc.h>
#include "log/log.h"
#include <vulkan/vulkan_raii.hpp>
module nx.rhi.vk.allocator;
import nx.core.log;

LOGGER(vulkan_allocator);

static const  char* tags[] = {
		"VK_SYSTEM_ALLOCATION_SCOPE_COMMAND",
		"VK_SYSTEM_ALLOCATION_SCOPE_OBJECT",
		"VK_SYSTEM_ALLOCATION_SCOPE_CACHE",
		"VK_SYSTEM_ALLOCATION_SCOPE_DEVICE",
		"VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE"
};
static constexpr const char* VkSystemAllocationScopeTag(vk::SystemAllocationScope s) {
	return tags[std::to_underlying(s)];
}



vk::AllocationCallbacks nx::CreateVulkanAllocator()
{
	auto all = vk::AllocationCallbacks{};


	all.setPfnAllocation(
		[](void* pUserData, size_t size, size_t alignment, vk::SystemAllocationScope allocationScope) -> void*
		{
			return mi_malloc_aligned(size, alignment);
		}
	)
		.setPfnFree(
			[](void* pUserData, void* pMemory)
			{
				mi_free(pMemory);
			}
		).setPfnInternalAllocation(
			[](void* pUserData, size_t size, vk::InternalAllocationType allocationType, vk::SystemAllocationScope allocationScope)
			{
#ifdef  _DEBUG
					_logger.Debug(std::format("Vulkan internal allocation: size={}, scope={}", size, VkSystemAllocationScopeTag(allocationScope)));
#endif
			}
		).setPfnInternalFree(
			[](void* pUserData, size_t size, vk::InternalAllocationType allocationType, vk::SystemAllocationScope allocationScope)
			{
#ifdef _DEBUG
					_logger.Debug(std::format("Vulkan internal free: size={}, scope={}", size, VkSystemAllocationScopeTag(allocationScope)));
#endif

			}
		).setPfnReallocation(
			[](void* pUserData, void* pOriginal, size_t size, size_t alignment, vk::SystemAllocationScope allocationScope) -> void*
			{
				void* pNew = mi_malloc_aligned(size, alignment);
				if (pOriginal && pNew) {
					std::memcpy(pNew, pOriginal, size);
					mi_free(pOriginal);
				}
				return pNew;
			}
		);

	return all;
}
