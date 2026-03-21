#pragma once
#include "defs.h"
#include "core/memory/memory.h"
#include "rhi/error_code.h"
#include <memory>
namespace nx
{
	  

	struct ImageCreateInfo
	{
		uint32_t width;
		uint32_t height;
		uint32_t depth;
		uint32_t mipLevels;
		uint32_t arrayLayers;
		EPixelFormat format;
		EImageUsage usage;
	};

	struct BufferCreateInfo
	{
		uint32_t size;
		EBufferUsage usage;
	};



	struct BufferViewCreateInfo
	{
		uint32_t offset{};
		uint32_t range{};
		EVertexFormat format = EVertexFormat::eUndefined;
	};

	class RhiBufferView
	{
	public:
		virtual ~RhiBufferView() = default;
	};

	class RhiBuffer {
	public:
		virtual ~RhiBuffer() = default;
		virtual Result<std::unique_ptr<RhiBufferView>> CreateView(const BufferViewCreateInfo& createInfo)noexcept = 0;
	};




	class RhiImageView
	{
	public:
		virtual ~RhiImageView() = default;
	};

	struct ImageSubresourceRange
	{
		uint32_t baseMipLevel;
		uint32_t levelCount;
		uint32_t baseArrayLayer;
		uint32_t layerCount;
	};

	struct ImageViewCreateInfo {

		EImageViewType viewType;
		ImageSubresourceRange subresourceRange;
		ESwizzleMapping swizzleMapping[4]; // R,G,B,A

	};

	class RhiImage
	{
	public:
		virtual ~RhiImage() = default;
		
		virtual Result<CommonPtr<RhiImageView>> CreateView(const ImageViewCreateInfo& createInfo) noexcept = 0;
		
		virtual const ImageCreateInfo& GetImageInfo()noexcept = 0;

	};


	


	

}