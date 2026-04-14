//
// Created by futurvo on 2026/3/21.
//

#include "swap_chain.h"
#include "surface.h"
#include "allocator_callbacks.h"
#include "resource.h"
#include "defs.h"

nx::Error nx::VkSwapChain::Initialize(RhiSurface* surface,uint32_t width,uint32_t height) noexcept
{
	m_surface = surface;
	return Create(width,height);
}

nx::Error nx::VkSwapChain::Resize(uint32_t w, uint32_t h) noexcept
{
	m_device.waitIdle();
	vk::SwapchainKHR oldSwapChain = m_swapChain;
	RETURN_ON_ERROR(Create(w,h));
	m_device.destroySwapchainKHR(oldSwapChain, GetVulkanAllocatorCallbacks());
	return Succeeded;
}

nx::VkSwapChain::~VkSwapChain()
{
}

nx::VkSwapChain::VkSwapChain(vk::Device& device,vk::PhysicalDevice& physicalDevice) noexcept
	: m_surface(nullptr), m_physicalDevice(physicalDevice), m_device(device), m_swapChainImageFormat()
{
}

const nx::RhiImage* nx::VkSwapChain::GetImage(uint32_t) const noexcept
{
	return m_swapChainImages[CurrentImageIndex()].get();
}

uint32_t nx::VkSwapChain::GetImageCount() const noexcept
{
	return m_swapChainImages.size();
}

uint32_t nx::VkSwapChain::CurrentImageIndex() const noexcept
{
	return  m_currentImageIndex% m_swapChainImages.size();
}

nx::Error nx::VkSwapChain::Present() noexcept
{
	// TODO:

	m_currentImageIndex++;
	return Succeeded;
}

vk::SurfaceFormatKHR nx::VkSwapChain::ChooseSwapSurfaceFormat(
    const std::vector<vk::SurfaceFormatKHR>& availableFormats) noexcept
{
    if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined) {
        return { vk::Format::eB8G8R8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear };
    }

    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == vk::Format::eB8G8R8A8Srgb &&
            availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return availableFormat;
            }
    }
    return availableFormats[0];
}

vk::PresentModeKHR nx::VkSwapChain::ChooseSwapPresentMode(
    const std::vector<vk::PresentModeKHR>& availablePresentModes) noexcept
{
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
            return availablePresentMode;
        }
    }

    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == vk::PresentModeKHR::eImmediate) {
            return availablePresentMode;
        }
    }
    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D nx::VkSwapChain::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities ,uint32_t width, uint32_t height) noexcept
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    vk::Extent2D actualExtent = {
        std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, width)),
        std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, height))
    };

    return actualExtent;
}

uint32_t nx::VkSwapChain::ChooseSwapImageCount(const vk::SurfaceCapabilitiesKHR& capabilities) noexcept
{
    uint32_t imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
        imageCount = capabilities.maxImageCount;
    }
    return imageCount;
}

nx::Error nx::VkSwapChain::Create(uint32_t w, uint32_t h) noexcept
{

	auto surface = dynamic_cast<VkSurface*>(m_surface);
	auto& vkSurface = surface->Get();
    auto surface_capabilities = m_physicalDevice.getSurfaceCapabilitiesKHR(vkSurface);
	if (surface_capabilities.minImageCount == 0) {
		return nx::Unexpected(nx::make_error_code(EcsErrc::Retry));
	}
	auto surface_formats = m_physicalDevice.getSurfaceFormatsKHR(vkSurface);
	auto present_modes = m_physicalDevice.getSurfacePresentModesKHR(vkSurface);

	auto surface_format = ChooseSwapSurfaceFormat(surface_formats);
	auto present_mode = ChooseSwapPresentMode(present_modes);
	auto extent = ChooseSwapExtent(surface_capabilities,w,h);
	auto image_count = ChooseSwapImageCount(surface_capabilities);

	auto vk_surface = dynamic_cast<VkSurface*>(m_surface);
	vk::SwapchainCreateInfoKHR createInfo{};
	createInfo.setSurface(vk_surface->Get())
	          .setMinImageCount(image_count)
	          .setImageFormat(surface_format.format)
	          .setImageColorSpace(surface_format.colorSpace)
	          .setImageExtent(extent)
	          .setImageArrayLayers(1)
	          .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);

	VK_RETURN_IF_ERROR( m_device.createSwapchainKHR(&createInfo, GetVulkanAllocatorCallbacks(), &m_swapChain));

	auto images = m_device.getSwapchainImagesKHR(m_swapChain);


	m_swapChainImages.clear();
	m_swapChainImages.reserve(images.size());
	m_swapChainImageViews.clear();
	m_swapChainImageViews.reserve(images.size());

	ImageCreateInfo imageCreateInfo{
		.width = extent.width,
		.height = extent.height,
		.depth = 1,
		.mipLevels = 1,
		.arrayLayers = 1,
		.format = FromVulkan(surface_format.format),
		.usage = EImageUsageBits::eColorAttachment
	};
	for (auto& i : images)
	{
		auto & image = m_swapChainImages.emplace_back(MakeCommonPtr<VkImage>(m_device, imageCreateInfo, std::move(i)));

		ImageViewCreateInfo viewCreateInfo{
			.viewType = EImageViewType::e2D,
			.subresourceRange = {
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			},
			.swizzleMapping = {
				ESwizzleMapping::eR,
				ESwizzleMapping::eG,
				ESwizzleMapping::eB,
				ESwizzleMapping::eA
			}
		};
		auto ret = image->CreateView(viewCreateInfo);
		if (!ret) return nx::Unexpected(ret.error());
		m_swapChainImageViews.emplace_back(std::move(ret.value()));
	}

	return Succeeded;
}
