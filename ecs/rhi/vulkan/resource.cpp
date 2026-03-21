#include "resource.h"
#include <vulkan/vulkan.hpp>

#include "defs.h"
#include "allocator_callbacks.h"
#include "resource.h"



nx::VkBuffer::VkBuffer(vk::Device& d, const BufferCreateInfo& createInfo, vk::Buffer&& buffer)
	:m_device(d),m_createInfo(createInfo), m_buffer(std::move(buffer))
{
}

nx::VkBuffer::~VkBuffer()
{
	if(m_buffer)
		m_device.destroyBuffer(m_buffer, GetVulkanAllocatorCallbacks());
}

nx::Result<std::unique_ptr<nx::RhiBufferView>> nx::VkBuffer::CreateView(const BufferViewCreateInfo& createInfo) noexcept
{
	vk::BufferViewCreateInfo vkCreateInfo;
	vkCreateInfo.setBuffer(m_buffer)
		.setFormat(VertexFormatToVulkan(createInfo.format))
		.setOffset(createInfo.offset)
		.setRange(createInfo.range)
		.setBuffer(m_buffer);

	vk::BufferView bufferView;
	VK_UNEXPECTED_ON_ERROR(m_device.createBufferView(&vkCreateInfo, GetVulkanAllocatorCallbacks(), &bufferView));
	return std::make_unique<VkBufferView>(m_device, this, createInfo, std::move(bufferView));

}

nx::VkImage::VkImage(vk::Device& d, const ImageCreateInfo& createInfo, vk::Image&& image)
	:m_device(d),m_createInfo(createInfo), m_image(std::move(image))
{
}

nx::VkImage::~VkImage()
{
	if(m_image)
		m_device.destroyImage(m_image, GetVulkanAllocatorCallbacks());
}

nx::Result<nx::CommonPtr<nx::RhiImageView>> nx::VkImage::CreateView(
	const ImageViewCreateInfo& createInfo) noexcept
{

	vk::ImageViewCreateInfo vkCreateInfo;
	vkCreateInfo.setImage(m_image)
		.setViewType(ToVulkan(createInfo.viewType))
		.setFormat(ToVulkan(m_createInfo.format))
		.setImage(m_image)
		.setComponents(vk::ComponentMapping{}
			.setR(ToVulkan(createInfo.swizzleMapping[0]))
			.setG(ToVulkan(createInfo.swizzleMapping[1]))
			.setB(ToVulkan(createInfo.swizzleMapping[2]))
			.setA(ToVulkan(createInfo.swizzleMapping[3]))
		)
		.setSubresourceRange(vk::ImageSubresourceRange{}
	.setBaseMipLevel(createInfo.subresourceRange.baseMipLevel)
		.setLevelCount(createInfo.subresourceRange.levelCount)
		.setBaseArrayLayer(createInfo.subresourceRange.baseArrayLayer)
		.setLayerCount(createInfo.subresourceRange.layerCount)
		);

	vk::ImageView imageView;
	VK_UNEXPECTED_ON_ERROR(m_device.createImageView(&vkCreateInfo, GetVulkanAllocatorCallbacks(), &imageView));

	return make_common_ptr<VkImageView>(m_device,this, createInfo, std::move(imageView));

}

const nx::ImageCreateInfo& nx::VkImage::GetImageInfo() noexcept
{
	return m_createInfo;
}

nx::VkImageView::VkImageView(vk::Device& d, VkImage* image,const ImageViewCreateInfo& createInfo, vk::ImageView&& imageView)
	:m_device(d), m_image(image),m_createInfo(createInfo), m_imageView(std::move(imageView))
{
}

nx::VkImageView::~VkImageView()
{
	if(m_imageView)
		m_device.destroyImageView(m_imageView, GetVulkanAllocatorCallbacks());
}

nx::VkBufferView::VkBufferView(vk::Device& d, VkBuffer* buffer, const BufferViewCreateInfo& createInfo, vk::BufferView&& bufferView)
	:m_device(d), m_buffer(buffer), m_createInfo(createInfo), m_bufferView(std::move(bufferView))
{
}

nx::VkBufferView::~VkBufferView()
{
	if(m_bufferView)
		m_device.destroyBufferView(m_bufferView, GetVulkanAllocatorCallbacks());
}
