#pragma once
#include "rhi/defs.h"
#include <vulkan/vulkan.hpp>
namespace nx
{
	vk::QueueFlagBits ToVulkan(EQueueTypeBits bits) noexcept;
	EQueueTypeBits FromVulkan(vk::QueueFlagBits bits) noexcept;

	vk::QueueFlags ToVulkan(EQueueType bits) noexcept;
	EQueueType FromVulkan(vk::QueueFlags bits) noexcept;

	vk::ShaderStageFlagBits ToVulkan(EShaderTypeBits type) noexcept;
	EShaderTypeBits FromVulkan(vk::ShaderStageFlagBits bits) noexcept;
	vk::ShaderStageFlags ToVulkan(EShaderType bits) noexcept;
	EShaderType FromVulkan(vk::ShaderStageFlags bits) noexcept;

	EImageUsageBits FromVulkan(vk::ImageUsageFlagBits)noexcept;
	vk::ImageUsageFlagBits ToVulkan(EImageUsageBits)noexcept;

	EImageUsage FromVulkan(vk::ImageUsageFlags flag)noexcept;
	vk::ImageUsageFlags ToVulkan(EImageUsage)noexcept;

	EBufferUsageBits FromVulkan(vk::BufferUsageFlagBits)noexcept;
	vk::BufferUsageFlagBits ToVulkan(EBufferUsageBits)noexcept;

	EBufferUsage FromVulkan(vk::BufferUsageFlags)noexcept;
	vk::BufferUsageFlags ToVulkan(EBufferUsage)noexcept;



	vk::Format ToVulkan(EPixelFormat format) noexcept;
	EPixelFormat FromVulkan(vk::Format format) noexcept;

	EImageViewType FromVulkan(vk::ImageViewType type) noexcept;
	vk::ImageViewType ToVulkan(EImageViewType type) noexcept;

	EImageAspectFlagBits FromVulkan(vk::ImageAspectFlagBits flags) noexcept;
	vk::ImageAspectFlagBits ToVulkan(EImageAspectFlagBits flags) noexcept;

	EImageAspectFlags FromVulkan(vk::ImageAspectFlags flags) noexcept;
	vk::ImageAspectFlags ToVulkan(EImageAspectFlags flags) noexcept;

	ESwizzleMapping FromVulkan(vk::ComponentSwizzle mapping) noexcept;
	vk::ComponentSwizzle ToVulkan(ESwizzleMapping mapping) noexcept;


	EVertexFormat VectextFormatFromVulkan(vk::Format format) noexcept;
	vk::Format VertexFormatToVulkan(EVertexFormat format) noexcept;

	ESampleCountFlagBits FromVulkan(vk::SampleCountFlagBits)noexcept;
	vk::SampleCountFlagBits ToVulkan(ESampleCountFlagBits)noexcept;

	ESampleCountFlags FromVulkan(vk::SampleCountFlags)noexcept;
	vk::SampleCountFlags ToVulkan(ESampleCountFlags)noexcept;


	ELoadOp FromVulkan(vk::AttachmentLoadOp op);
	vk::AttachmentLoadOp ToVulkan(ELoadOp op);

	EStoreOp FromVulkan(vk::AttachmentStoreOp op);
	vk::AttachmentStoreOp ToVulkan(EStoreOp op);

	//EImageLayout FromVulkan(vk::ImageLayout layout);
	vk::ImageLayout ToVulkan(EImageLayout layout);



}
