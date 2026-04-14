#pragma once
#include "rhi/instance.h"
#include <vulkan/vulkan.hpp>

namespace nx
{
    class VkInstance : public RhiInstance
    {
    public:
        VkInstance();
        ~VkInstance() override;
        Error Initialize(const std::string& appName) noexcept override;
        Result<CommonPtr<RhiSurface>> CreateSurface(void* nativeWindowHandle) noexcept override;
        Result<CommonPtr<RhiContext>> CreateContext() noexcept override;

        [[nodiscard]] const vk::Instance& Get()const noexcept;
        [[nodiscard]] uint32_t CurrentVulkanVersion()const noexcept;
    private:
        vk::Instance m_instance;
    };


}

