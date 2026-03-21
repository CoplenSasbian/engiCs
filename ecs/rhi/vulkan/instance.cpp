#include "instance.h"
#include "device.h"
#include "config.h"

#include <Windows.h>
#include <vulkan/vulkan_win32.h>


#include "allocator_callbacks.h"
#include <ranges>
#include "rhi/error_code.h"
#include "surface.h"
#include "core/memory/memory.h"




nx::VkInstance::~VkInstance()
{
    if (m_instance)
        m_instance.destroy();
}

nx::Error nx::VkInstance::Initialize(const std::string& appName) noexcept
{
    auto layersConfig = EngineConfig::GetRequiredInstanceLayers(true);
    auto layers = layersConfig | std::views::transform([](const VulkanExtensionLayerInfo& info) { return info.name.c_str(); }) | std::ranges::to<std::vector<const char*>>();

    auto extensionsConfig = EngineConfig::GetRequiredInstanceExtensions(true);
    auto extensions = extensionsConfig | std::views::transform([](const VulkanExtensionLayerInfo& info) { return info.name.c_str(); }) | std::ranges::to<std::vector<const char*>>();


    vk::ApplicationInfo appInfo(
        appName.c_str(),
        VK_MAKE_VERSION(1, 0, 0),
        appName.c_str(),
        VK_MAKE_VERSION(1, 0, 0),
        VulkanVersion
    );

    vk::InstanceCreateInfo createInfo(
        vk::InstanceCreateFlags(),
        &appInfo,
        static_cast<uint32_t>(layers.size()),
        layers.data(),
        static_cast<uint32_t>(extensions.size()),
        extensions.data()
    );

    VK_RETURN_ON_ERROR(vk::createInstance(&createInfo,GetVulkanAllocatorCallbacks(),& m_instance));
    return  Succeeded;
}

nx::Result<nx::CommonPtr<nx::RhiSurface>> nx::VkInstance::CreateSurface(void* nativeWindowHandle) noexcept
{
#ifdef WIN32

    VkWin32SurfaceCreateInfoKHR createInfo{
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .hinstance = GetModuleHandle(0),
        .hwnd = static_cast<HWND>(nativeWindowHandle)
    };
    VkSurfaceKHR surface;
    C_VK_UNEXPECTED_ON_ERROR(vkCreateWin32SurfaceKHR(
        m_instance,
        &createInfo,
        (VkAllocationCallbacks*)GetVulkanAllocatorCallbacks(),
        &surface
    ));

    return  make_common_ptr<VkSurface>(m_instance, std::move(surface));

#endif // WIN32

}

nx::Result<nx::CommonPtr<nx::RhiDevice>> nx::VkInstance::CreateDevice() noexcept
{
    return make_common_ptr<VkDevice>(m_instance);
}

nx::VkInstance::VkInstance()
{

}

const vk::Instance& nx::VkInstance::Get() const noexcept
{
    return m_instance;
}

uint32_t nx::VkInstance::CurrentVulkanVersion() const noexcept
{
    return VulkanVersion;
}
