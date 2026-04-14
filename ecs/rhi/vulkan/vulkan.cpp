#include "vulkan.h"
#include "instance.h"

nx::CommonPtr<nx::RhiInstance> nx::CreateVulkanInstance() noexcept
{
    return MakeCommonPtr<VkInstance>();
}
