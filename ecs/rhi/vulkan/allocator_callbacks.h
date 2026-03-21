#pragma once
#include <vulkan/vulkan.hpp>
namespace nx {
	vk::AllocationCallbacks* GetVulkanAllocatorCallbacks() noexcept;
}