module;
#include <vulkan/vulkan.hpp>
export module nx.rhi.vk.allocator;
export namespace nx {

	vk::AllocationCallbacks CreateVulkanAllocator();
}