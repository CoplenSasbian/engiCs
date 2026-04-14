#pragma once
#ifndef ECS_RENDERER_H
#define ECS_RENDERER_H
#include <core/error_code.h>
#include <platform/window/window.h>
#include <concurrency/threadpool/threadpool.h>
#include <exec/task.hpp>

#include "rhi/instance.h"


namespace renderer
{
    enum class Mode
    {
        Editor,
        Game
    };

    enum class RendererType
    {
        Vulkan,
        DX12
    };

    struct AppInfo
    {
        std::string name;
        Mode mode;
    };

    class Renderer
    {
    public:
        Renderer() = default;
         nx::Error Initialize(const AppInfo& info,  nx::IWindow*,RendererType = RendererType::Vulkan) noexcept;
         exec::task<nx::Error> Update(std::chrono::microseconds) noexcept;
         exec::task<nx::Error> Render() noexcept;
    private:
        nx::IWindow* m_window;
        nx::CommonPtr<nx::RhiInstance> m_instance;
        nx::CommonPtr<nx::RhiSurface> m_surface;
        nx::CommonPtr<nx::RhiContext> m_device;
        nx::CommonPtr<nx::RhiSwapChain> m_swapChain;
    };
}


#endif //ECS_RENDERER_H
