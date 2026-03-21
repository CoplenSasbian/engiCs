//
// Created by futurvo on 2026/3/21.
//

#include "renderer.h"
#include "rhi/vulkan/vulkan.h"
nx::Error renderer::Renderer::Initialize(const AppInfo& appInfo, nx::IWindow* window, RendererType  type) noexcept
{
    m_window = window;
    if (type == RendererType::Vulkan)
    {
        m_instance = nx::CreateVulkanInstance();
    }

    RETURN_ON_ERROR( m_instance->Initialize(appInfo.name));
    ASSIGN_OR_RETURN(m_device,m_instance->CreateDevice());
    ASSIGN_OR_RETURN(m_surface,m_instance->CreateSurface(window->NativeHandle()));
    ASSIGN_OR_RETURN(m_swapChain,m_device->CreateSwapChain(m_surface.get()));
    return nx::Succeeded;
}

exec::task<nx::Error> renderer::Renderer::Update(std::chrono::microseconds) noexcept
{

    co_return nx::Succeeded;
}

exec::task<nx::Error> renderer::Renderer::Render() noexcept
{

    co_return nx::Succeeded;
}
