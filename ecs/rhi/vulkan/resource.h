#pragma once
#include"rhi/resource.h"
#include <vulkan/vulkan.hpp>
namespace nx
{

	


	class VkBuffer : public RhiBuffer
	{
	public:
		static Result<CommonPtr<VkBuffer>> DeviceCreate(vk::Device& d, const BufferCreateInfo& tinfo)noexcept;
		VkBuffer(vk::Device&, const BufferCreateInfo& createInfo, vk::Buffer&& buffer);
		~VkBuffer();
		Result<std::unique_ptr<RhiBufferView>> CreateView(const BufferViewCreateInfo& createInfo) noexcept override;
	private:
		vk::Device& m_device;
		BufferCreateInfo m_createInfo;
		vk::Buffer m_buffer;

	};

	class VkBufferView : public RhiBufferView
	{
	public:
		VkBufferView(vk::Device&, VkBuffer* buffer, const BufferViewCreateInfo& createInfo, vk::BufferView&& bufferView);
		~VkBufferView() override;
	private:
		vk::Device& m_device;
		VkBuffer* m_buffer;
		BufferViewCreateInfo m_createInfo;
		vk::BufferView m_bufferView;
	};


	class VkImage : public RhiImage
	{
	public:
		static Result<CommonPtr<VkImage>> DeviceCreate(vk::Device& d, const ImageCreateInfo& tinfo)noexcept;


		VkImage(vk::Device& ,const ImageCreateInfo& createInfo, vk::Image&& image);
		~VkImage()override;
		Result<CommonPtr<RhiImageView>> CreateView(const ImageViewCreateInfo& createInfo) noexcept override;
		const ImageCreateInfo& GetImageInfo()noexcept override;
	private:
		vk::Device& m_device;
		ImageCreateInfo m_createInfo;
		vk::Image m_image;

	};


	class VkImageView : public RhiImageView
	{
	public:
		VkImageView(vk::Device&, VkImage* image, const ImageViewCreateInfo& createInfo, vk::ImageView&& imageView);
		~VkImageView() override;
	private:
		vk::Device& m_device;
		VkImage* m_image;
		ImageViewCreateInfo m_createInfo;
		vk::ImageView m_imageView;
	};
}