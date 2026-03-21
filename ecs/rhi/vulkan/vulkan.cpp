#include "vulkan.h"
#include "instance.h"

nx::CommonPtr<nx::RhiInstance> nx::CreateVulkanInstance() noexcept
{
    return make_common_ptr<VkInstance>();
}
