#pragma once

#include "rhi/swap_chain.h"
#include "rhi/resource.h"

#include<vulkan/vulkan.hpp>
namespace nx
{
    class VkSwapChain :public RhiSwapChain
    {
    public:
        Error Initialize(RhiSurface*,uint32_t,uint32_t)noexcept override;
        Error Resize(uint32_t, uint32_t)noexcept override;
        ~VkSwapChain() override;
        VkSwapChain(vk::Device& device,vk::PhysicalDevice& physicalDevice)noexcept;
        const RhiImage* GetImage(uint32_t) const noexcept override;
        uint32_t GetImageCount() const noexcept override;
        uint32_t CurrentImageIndex() const noexcept override;
        Error Present() noexcept override;

    private:
        static vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)noexcept;
        static vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)noexcept;
        static vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities,uint32_t w,uint32_t h)noexcept;
        static uint32_t ChooseSwapImageCount(const vk::SurfaceCapabilitiesKHR& capabilities)noexcept;
        Error Create(uint32_t,uint32_t)noexcept;
    private:
        RhiSurface* m_surface;
        vk::PhysicalDevice& m_physicalDevice;
        vk::Device& m_device;
        vk::SwapchainKHR m_swapChain;
        vk::Format m_swapChainImageFormat;
        vk::Extent2D m_swapChainExtent;
        std::vector<CommonPtr<RhiImage>> m_swapChainImages;
        std::vector<CommonPtr<RhiImageView>> m_swapChainImageViews;
        uint32_t m_currentImageIndex = 0;
    };
}

