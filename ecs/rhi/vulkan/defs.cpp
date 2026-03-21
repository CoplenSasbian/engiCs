#include "defs.h"

namespace nx
{

vk::QueueFlagBits ToVulkan(EQueueTypeBits bits) noexcept
{
    switch (bits)
    {
    case EQueueTypeBits::eDirect:
        return vk::QueueFlagBits::eGraphics;
    case EQueueTypeBits::eCompute:
        return vk::QueueFlagBits::eCompute;
    case EQueueTypeBits::eTransfer:
        return vk::QueueFlagBits::eTransfer;
    case EQueueTypeBits::ePresent:
        return vk::QueueFlagBits::eGraphics;
    default:
        return vk::QueueFlagBits::eGraphics;
    }
}

EQueueTypeBits FromVulkan(vk::QueueFlagBits bits) noexcept
{
    switch (bits)
    {
    case vk::QueueFlagBits::eGraphics:
		return EQueueTypeBits::eDirect;
    case vk::QueueFlagBits::eCompute:
		return EQueueTypeBits::eCompute;
    case vk::QueueFlagBits::eTransfer:
		return EQueueTypeBits::eTransfer;
    default:
		std::abort(); // Invalid queue flag bit
        break;
    }
    
}

vk::QueueFlags ToVulkan(EQueueType bits) noexcept
{
    vk::QueueFlags flags;
    if (bits & (EQueueTypeBits::eDirect))
        flags |= vk::QueueFlagBits::eGraphics;
    if (bits & EQueueTypeBits::eCompute)
        flags |= vk::QueueFlagBits::eCompute;
    if (bits & EQueueTypeBits::eTransfer)
        flags |= vk::QueueFlagBits::eTransfer;
    if (bits & EQueueTypeBits::ePresent)
        flags |= vk::QueueFlagBits::eGraphics;
    return flags;
}

EQueueType FromVulkan(vk::QueueFlags bits) noexcept
{
    EQueueType result;
    if (bits & vk::QueueFlagBits::eGraphics)
        result |= EQueueType::Bits::eDirect;
    if (bits & vk::QueueFlagBits::eCompute)
        result |= EQueueType::Bits::eCompute;
    if (bits & vk::QueueFlagBits::eTransfer)
        result |= EQueueType::Bits::eTransfer;
    return EQueueType(result);
}

vk::ShaderStageFlagBits ToVulkan(EShaderTypeBits type) noexcept
{
    switch (type)
    {
    case EShaderTypeBits::eVertex:
        return vk::ShaderStageFlagBits::eVertex;
    case EShaderTypeBits::eFragment:
        return vk::ShaderStageFlagBits::eFragment;
    case EShaderTypeBits::eCompute:
        return vk::ShaderStageFlagBits::eCompute;
    case EShaderTypeBits::eGeometry:
        return vk::ShaderStageFlagBits::eGeometry;
    case EShaderTypeBits::eTessellationControl:
        return vk::ShaderStageFlagBits::eTessellationControl;
    case EShaderTypeBits::eTessellationEvaluation:
        return vk::ShaderStageFlagBits::eTessellationEvaluation;
    default:
        return vk::ShaderStageFlagBits::eVertex;
    }
}

EShaderTypeBits FromVulkan(vk::ShaderStageFlagBits bits) noexcept
{
    switch (bits)
    {
    case vk::ShaderStageFlagBits::eVertex:
		return EShaderTypeBits::eVertex;
    case vk::ShaderStageFlagBits::eTessellationControl:
		return EShaderTypeBits::eTessellationControl;
    case vk::ShaderStageFlagBits::eTessellationEvaluation:
		return EShaderTypeBits::eTessellationEvaluation;
    case vk::ShaderStageFlagBits::eGeometry:
		return EShaderTypeBits::eGeometry;
    case vk::ShaderStageFlagBits::eFragment:
		return EShaderTypeBits::eFragment;
    case vk::ShaderStageFlagBits::eCompute:
		return EShaderTypeBits::eCompute;
    default:
		std::abort(); // Invalid shader stage flag bit
    }
}

vk::ShaderStageFlags ToVulkan(EShaderType bits) noexcept
{
    vk::ShaderStageFlags flags;
    if (bits & EShaderTypeBits::eVertex)
        flags |= vk::ShaderStageFlagBits::eVertex;
    if (bits &  EShaderTypeBits::eFragment)
        flags |= vk::ShaderStageFlagBits::eFragment;
    if (bits & EShaderTypeBits::eCompute)
        flags |= vk::ShaderStageFlagBits::eCompute;
    if (bits &  EShaderTypeBits::eGeometry)
        flags |= vk::ShaderStageFlagBits::eGeometry;
    if (bits & EShaderTypeBits::eTessellationControl)
        flags |= vk::ShaderStageFlagBits::eTessellationControl;
    if (bits&  EShaderTypeBits::eTessellationEvaluation)
        flags |= vk::ShaderStageFlagBits::eTessellationEvaluation;
    return flags;
}

EShaderType FromVulkan(vk::ShaderStageFlags bits) noexcept
{
    EShaderType result;
    if (bits & vk::ShaderStageFlagBits::eVertex)
        result |= EShaderType::Bits::eVertex;
    if (bits & vk::ShaderStageFlagBits::eFragment)
        result |= EShaderType::Bits::eFragment;
    if (bits & vk::ShaderStageFlagBits::eCompute)
        result |= EShaderType::Bits::eCompute;
    if (bits & vk::ShaderStageFlagBits::eGeometry)
        result |= EShaderType::Bits::eGeometry;
    if (bits & vk::ShaderStageFlagBits::eTessellationControl)
        result |= EShaderType::Bits::eTessellationControl;
    if (bits & vk::ShaderStageFlagBits::eTessellationEvaluation)
        result |= EShaderType::Bits::eTessellationEvaluation;
    return EShaderType(result);
}

EImageUsageBits FromVulkan(vk::ImageUsageFlagBits flag) noexcept
{
    switch (flag)
    {
    case vk::ImageUsageFlagBits::eTransferSrc:
        return EImageUsageBits::eTransferSrc;
    case vk::ImageUsageFlagBits::eTransferDst:
        return EImageUsageBits::eTransferDst;
    case vk::ImageUsageFlagBits::eSampled:
        return EImageUsageBits::eSampledImage;
    case vk::ImageUsageFlagBits::eStorage:
        return EImageUsageBits::eStorageImage;
    case vk::ImageUsageFlagBits::eColorAttachment:
        return EImageUsageBits::eColorAttachment;
    case vk::ImageUsageFlagBits::eDepthStencilAttachment:
        return EImageUsageBits::eDepthStencilAttachment;
    default:
        std::abort();
    }
}

vk::ImageUsageFlagBits ToVulkan(EImageUsageBits flag) noexcept
{
    switch (flag)
    {
    case nx::EImageUsageBits::eUndefined:
        return vk::ImageUsageFlagBits::eStorage;
    case nx::EImageUsageBits::eSampledImage:
        return vk::ImageUsageFlagBits::eSampled;
    case nx::EImageUsageBits::eStorageImage:
        return vk::ImageUsageFlagBits::eStorage;
    case nx::EImageUsageBits::eColorAttachment:
        return vk::ImageUsageFlagBits::eColorAttachment;
    case nx::EImageUsageBits::eDepthStencilAttachment:
        return vk::ImageUsageFlagBits::eDepthStencilAttachment;
    case nx::EImageUsageBits::eTransferSrc:
        return vk::ImageUsageFlagBits::eTransferSrc;
    case nx::EImageUsageBits::eTransferDst:
        return vk::ImageUsageFlagBits::eTransferDst;
    default:
        std::unreachable();
    }
}

EImageUsage FromVulkan(vk::ImageUsageFlags flag) noexcept
{
    EImageUsage result;
    if (flag & vk::ImageUsageFlagBits::eStorage)
        result |= EImageUsage::Bits::eStorageImage;
    if (flag & vk::ImageUsageFlagBits::eSampled)
        result |= EImageUsage::Bits::eSampledImage;
    if (flag & vk::ImageUsageFlagBits::eTransferSrc)
        result |= EImageUsage::Bits::eTransferSrc;
    if (flag & vk::ImageUsageFlagBits::eTransferDst)
        result |= EImageUsage::Bits::eTransferDst;
    if (flag & vk::ImageUsageFlagBits::eColorAttachment)
        result |= EImageUsage::Bits::eColorAttachment;
    if (flag & vk::ImageUsageFlagBits::eDepthStencilAttachment)
        result |= EImageUsage::Bits::eDepthStencilAttachment;

    return result;

}

vk::ImageUsageFlags ToVulkan(EImageUsage flag) noexcept
{
    vk::ImageUsageFlags result;
    if (flag & EImageUsage::Bits::eStorageImage)
        result |= vk::ImageUsageFlagBits::eStorage;
    if (flag & EImageUsage::Bits::eSampledImage)
        result |= vk::ImageUsageFlagBits::eSampled;
    if (flag & EImageUsage::Bits::eTransferSrc)
        result |= vk::ImageUsageFlagBits::eTransferSrc;
    if (flag & EImageUsage::Bits::eTransferDst)
        result |= vk::ImageUsageFlagBits::eTransferDst;
    if (flag & EImageUsage::Bits::eColorAttachment)
        result |= vk::ImageUsageFlagBits::eColorAttachment;
    if (flag & EImageUsage::Bits::eDepthStencilAttachment)
        result |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
    return result;
}

EBufferUsageBits FromVulkan(vk::BufferUsageFlagBits flag) noexcept
{
    switch (flag)
    {
    case vk::BufferUsageFlagBits::eTransferSrc:
        return EBufferUsageBits::eTransferSrc;
    case vk::BufferUsageFlagBits::eTransferDst:
        return EBufferUsageBits::eTransferDst;
    case vk::BufferUsageFlagBits::eUniformBuffer:
        return EBufferUsageBits::eUniformBuffer;
    case vk::BufferUsageFlagBits::eStorageBuffer:
        return EBufferUsageBits::eStorageBuffer;
    case vk::BufferUsageFlagBits::eIndexBuffer:
        return EBufferUsageBits::eIndexBuffer;
    case vk::BufferUsageFlagBits::eVertexBuffer:
        return EBufferUsageBits::eVertexBuffer;
    case vk::BufferUsageFlagBits::eIndirectBuffer:
        return EBufferUsageBits::eIndexBuffer;
    default:
        std::abort();
    }
}

vk::BufferUsageFlagBits ToVulkan(EBufferUsageBits flag) noexcept
{
    switch (flag)
    {
    case nx::EBufferUsageBits::eVertexBuffer:
        return vk::BufferUsageFlagBits::eVertexBuffer;
    case nx::EBufferUsageBits::eIndexBuffer:
        return vk::BufferUsageFlagBits::eIndexBuffer;
    case nx::EBufferUsageBits::eUniformBuffer:
        return vk::BufferUsageFlagBits::eUniformBuffer;
    case nx::EBufferUsageBits::eStorageBuffer:
        return vk::BufferUsageFlagBits::eStorageBuffer;
    case nx::EBufferUsageBits::eIndirectBuffer:
        return vk::BufferUsageFlagBits::eIndirectBuffer;
    case nx::EBufferUsageBits::eTransferSrc:
        return vk::BufferUsageFlagBits::eTransferSrc;
    case nx::EBufferUsageBits::eTransferDst:
        return vk::BufferUsageFlagBits::eTransferDst;
    default:
        std::abort();
    }
}

EBufferUsage FromVulkan(vk::BufferUsageFlags flags) noexcept
{
    EBufferUsage result;
    if (flags & vk::BufferUsageFlagBits::eVertexBuffer)
        result |= EBufferUsage::Bits::eVertexBuffer;
    if (flags & vk::BufferUsageFlagBits::eIndexBuffer) {
        result |= EBufferUsage::Bits::eIndexBuffer;
    }
    if (flags & vk::BufferUsageFlagBits::eUniformBuffer) {
        result |= EBufferUsage::Bits::eUniformBuffer;
    }
    if (flags & vk::BufferUsageFlagBits::eStorageBuffer) {
        result |= EBufferUsage::Bits::eStorageBuffer;
    }
    if (flags & vk::BufferUsageFlagBits::eIndirectBuffer) {
        result |= EBufferUsage::Bits::eIndirectBuffer;
    }
    if (flags & vk::BufferUsageFlagBits::eTransferSrc) {
        result |= EBufferUsage::Bits::eTransferSrc;
    }
    if (flags & vk::BufferUsageFlagBits::eTransferDst) {
        result |= EBufferUsage::Bits::eTransferDst;
    }
    return result;
}

vk::BufferUsageFlags ToVulkan(EBufferUsage flag) noexcept
{
    vk::BufferUsageFlags result{};

    if (flag & nx::EBufferUsage::Bits::eVertexBuffer) {
        result |= vk::BufferUsageFlagBits::eVertexBuffer;
    }
    if (flag & nx::EBufferUsage::Bits::eIndexBuffer) {
        result |= vk::BufferUsageFlagBits::eIndexBuffer;
    }
    if (flag & nx::EBufferUsage::Bits::eUniformBuffer) {
        result |= vk::BufferUsageFlagBits::eUniformBuffer;
    }
    if (flag & nx::EBufferUsage::Bits::eStorageBuffer) {
        result |= vk::BufferUsageFlagBits::eStorageBuffer;
    }
    if (flag & nx::EBufferUsage::Bits::eIndirectBuffer) {
        result |= vk::BufferUsageFlagBits::eIndirectBuffer;
    }
    if (flag & nx::EBufferUsage::Bits::eTransferSrc) {
        result |= vk::BufferUsageFlagBits::eTransferSrc;
    }
    if (flag & nx::EBufferUsage::Bits::eTransferDst) {
        result |= vk::BufferUsageFlagBits::eTransferDst;
    }

    return result;
}







vk::Format ToVulkan(EPixelFormat format) noexcept
{
    switch (format)
    {
    case nx::EPixelFormat::eR8G8B8A8Unorm:
		return vk::Format::eR8G8B8A8Unorm;
    case nx::EPixelFormat::eB8G8R8A8Unorm:
		return vk::Format::eB8G8R8A8Unorm;
    case nx::EPixelFormat::eR16G16B16A16Sfloat:
		return vk::Format::eR16G16B16A16Sfloat;
    case nx::EPixelFormat::eD32Sfloat:
		return vk::Format::eD32Sfloat;
    case nx::EPixelFormat::eD24UnormS8Uint:
		return vk::Format::eD24UnormS8Uint;
    default:
		std::unreachable();
    }
}

EPixelFormat FromVulkan(vk::Format format) noexcept
{
    switch (format)
    {
    case vk::Format::eR8G8B8A8Unorm:
		return nx::EPixelFormat::eR8G8B8A8Unorm;
    case vk::Format::eB8G8R8A8Unorm:
		return nx::EPixelFormat::eB8G8R8A8Unorm;
	case vk::Format::eR16G16B16A16Sfloat:
		return nx::EPixelFormat::eR16G16B16A16Sfloat;
	case vk::Format::eD32Sfloat:
		return nx::EPixelFormat::eD32Sfloat;
	case vk::Format::eD24UnormS8Uint:
		return nx::EPixelFormat::eD24UnormS8Uint;
    default:
		std::abort();
    }
}

EImageViewType FromVulkan(vk::ImageViewType type) noexcept
{
    switch (type)
    {
    case vk::ImageViewType::e1D:
		return EImageViewType::e1D;
    case vk::ImageViewType::e2D:
		return EImageViewType::e2D;
    case vk::ImageViewType::e3D:
        return EImageViewType::e3D;
    case vk::ImageViewType::eCube:
        return EImageViewType::eCube;
    case vk::ImageViewType::e1DArray:
        return EImageViewType::e1DArray;
    case vk::ImageViewType::e2DArray:
        return EImageViewType::e2DArray;
    case vk::ImageViewType::eCubeArray:
        return EImageViewType::eCubeArray;
    default:
		std::unreachable();
    }
}

vk::ImageViewType ToVulkan(EImageViewType type) noexcept
{
    switch (type)
    {
    case nx::EImageViewType::e1D:
		return vk::ImageViewType::e1D;
    case nx::EImageViewType::e2D:
        return vk::ImageViewType::e2D;
    case nx::EImageViewType::e3D:
        return vk::ImageViewType::e3D;
    case nx::EImageViewType::eCube:
        return vk::ImageViewType::eCube;
    case nx::EImageViewType::e1DArray:
        return vk::ImageViewType::e1DArray;
    case nx::EImageViewType::e2DArray:
        return vk::ImageViewType::e2DArray;
    case nx::EImageViewType::eCubeArray:
        return vk::ImageViewType::eCubeArray;
    default:
		std::unreachable();
    }
}

EImageAspectFlagBits FromVulkan(vk::ImageAspectFlagBits flags) noexcept
{
    switch (flags)
    {
    case vk::ImageAspectFlagBits::eColor:
		return EImageAspectFlagBits::eColor;
    case vk::ImageAspectFlagBits::eDepth:
		return EImageAspectFlagBits::eDepth;
    case vk::ImageAspectFlagBits::eStencil:
		return EImageAspectFlagBits::eStencil;
    default:
		std::abort();
    }
}

vk::ImageAspectFlagBits ToVulkan(EImageAspectFlagBits flags) noexcept
{
    switch (flags)
    {
    case nx::EImageAspectFlagBits::eColor:
		return  vk::ImageAspectFlagBits::eColor;
    case nx::EImageAspectFlagBits::eDepth:
		return  vk::ImageAspectFlagBits::eDepth;
    case nx::EImageAspectFlagBits::eStencil:
		return vk::ImageAspectFlagBits::eStencil;
    default:
		std::unreachable();
    }
}

EImageAspectFlags FromVulkan(vk::ImageAspectFlags flags) noexcept
{
	std::underlying_type_t<EImageAspectFlagBits> result = 0;
    if (flags & vk::ImageAspectFlagBits::eColor) 
		result |= std::to_underlying(EImageAspectFlagBits::eColor);
    if(flags & vk::ImageAspectFlagBits::eDepth)
		result |= std::to_underlying(EImageAspectFlagBits::eDepth);
	if (flags & vk::ImageAspectFlagBits::eStencil)
        result |= std::to_underlying(EImageAspectFlagBits::eStencil);
	return EImageAspectFlags(result);
}

vk::ImageAspectFlags ToVulkan(EImageAspectFlags flags) noexcept
{
	vk::ImageAspectFlags result;
    if (flags & EImageAspectFlagBits::eColor)
        result |= vk::ImageAspectFlagBits::eColor;
    if (flags & EImageAspectFlagBits::eDepth)
        result |= vk::ImageAspectFlagBits::eDepth;
    if (flags & EImageAspectFlagBits::eStencil)
        result |= vk::ImageAspectFlagBits::eStencil;
	return result;
}

ESwizzleMapping FromVulkan(vk::ComponentSwizzle mapping) noexcept
{
    switch (mapping)
    {
    case vk::ComponentSwizzle::eIdentity:
		return ESwizzleMapping::eIdentity;
    case vk::ComponentSwizzle::eZero:
		return ESwizzleMapping::eZero;
    case vk::ComponentSwizzle::eOne:
		return ESwizzleMapping::eOne;
    case vk::ComponentSwizzle::eR:
		return ESwizzleMapping::eR;
    case vk::ComponentSwizzle::eG:
		return ESwizzleMapping::eG;
    case vk::ComponentSwizzle::eB:
		return ESwizzleMapping::eB;
    case vk::ComponentSwizzle::eA:
		return ESwizzleMapping::eA;
    default:
		std::unreachable();
    }
}

vk::ComponentSwizzle ToVulkan(ESwizzleMapping mapping) noexcept
{
    switch (mapping)
    {
    case ESwizzleMapping::eIdentity:
        return vk::ComponentSwizzle::eIdentity;
    case ESwizzleMapping::eZero:
        return vk::ComponentSwizzle::eZero;
    case ESwizzleMapping::eOne:
        return vk::ComponentSwizzle::eOne;
    case ESwizzleMapping::eR:
        return vk::ComponentSwizzle::eR;
    case ESwizzleMapping::eG:
        return vk::ComponentSwizzle::eG;
    case ESwizzleMapping::eB:
        return vk::ComponentSwizzle::eB;
    case ESwizzleMapping::eA:
        return vk::ComponentSwizzle::eA;
    default:
        std::unreachable();
	}
}

EVertexFormat VectextFormatFromVulkan(vk::Format format) noexcept
{
    switch (format)
    {
	case vk::Format::eR32Sfloat:
		return EVertexFormat::eFloat32;
	case vk::Format::eR32G32Sfloat:
		return EVertexFormat::eFloat32x2;
	case vk::Format::eR32G32B32Sfloat:
		return EVertexFormat::eFloat32x3;
	case vk::Format::eR32G32B32A32Sfloat:
		return EVertexFormat::eFloat32x4;
	case vk::Format::eR32Uint:
		return EVertexFormat::eUint32;
	case vk::Format::eR32G32Uint:
		return EVertexFormat::eUint32x2;
	case vk::Format::eR32G32B32Uint:
		return EVertexFormat::eUint32x3;
	case vk::Format::eR32G32B32A32Uint:
		return EVertexFormat::eUint32x4;
    default:
		return EVertexFormat::eUndefined;
    }
}

vk::Format VertexFormatToVulkan(EVertexFormat format) noexcept
{
    switch (format)
    {
    case nx::EVertexFormat::eUndefined:
        return vk::Format::eUndefined;
    case nx::EVertexFormat::eFloat32:
        return vk::Format::eR32Sfloat;
    case nx::EVertexFormat::eFloat32x2:
        return vk::Format::eR32G32Sfloat;
    case nx::EVertexFormat::eFloat32x3:
        return vk::Format::eR32G32B32Sfloat;
    case nx::EVertexFormat::eFloat32x4:
        return vk::Format::eR32G32B32A32Sfloat;
    case nx::EVertexFormat::eUint32:
        return vk::Format::eR32Uint;
    case nx::EVertexFormat::eUint32x2:
        return vk::Format::eR32G32Uint;
    case nx::EVertexFormat::eUint32x3:
        return vk::Format::eR32G32B32Uint;
    case nx::EVertexFormat::eUint32x4:
        return vk::Format::eR32G32B32A32Uint;
    default:
		std::unreachable();
    }

}

ESampleCountFlagBits FromVulkan(vk::SampleCountFlagBits bits) noexcept
{
    return 
        static_cast<ESampleCountFlagBits>(
            static_cast<std::underlying_type_t<ESampleCountFlagBits>>(bits));
}

vk::SampleCountFlagBits ToVulkan(ESampleCountFlagBits bits) noexcept
{
    return
        static_cast<vk::SampleCountFlagBits>(
            static_cast<std::underlying_type_t<vk::SampleCountFlagBits>>(bits));
}

ESampleCountFlags FromVulkan(vk::SampleCountFlags bits) noexcept
{
   return  ESampleCountFlags{ static_cast<ESampleCountFlags::UnderlyingType>(static_cast<std::underlying_type_t<vk::SampleCountFlagBits>>(bits))};
}

vk::SampleCountFlags ToVulkan(ESampleCountFlags bits) noexcept
{
    return vk::SampleCountFlags{
        bits.value
    };
}

ELoadOp FromVulkan(vk::AttachmentLoadOp op)
{
    switch (op)
    {
    case vk::AttachmentLoadOp::eLoad:
        return ELoadOp::eLoad;
    case vk::AttachmentLoadOp::eClear:
        return ELoadOp::eClear;
    case vk::AttachmentLoadOp::eDontCare:
        return ELoadOp::eDiscard;
    default:
        return ELoadOp::eUndefined;
    }
}

vk::AttachmentLoadOp ToVulkan(ELoadOp op)
{
    switch (op)
    {
    case nx::ELoadOp::eLoad:
        return vk::AttachmentLoadOp::eLoad;
    case nx::ELoadOp::eClear:
        return vk::AttachmentLoadOp::eClear;
    case nx::ELoadOp::eDiscard:
        return vk::AttachmentLoadOp::eDontCare;
    case nx::ELoadOp::eUndefined:
        return vk::AttachmentLoadOp::eNone;
    default:
        std::unreachable();
    }
}

EStoreOp FromVulkan(vk::AttachmentStoreOp op)
{
    switch (op)
    {
    case vk::AttachmentStoreOp::eStore:
        return EStoreOp::eStore;
    case vk::AttachmentStoreOp::eDontCare:
        return EStoreOp::eDiscard;
    default:
        return EStoreOp::eUndefined;
    }
}

vk::AttachmentStoreOp ToVulkan(EStoreOp op)
{
    switch (op)
    {
    case nx::EStoreOp::eStore:
        return vk::AttachmentStoreOp::eStore;
    case nx::EStoreOp::eDiscard:
        return vk::AttachmentStoreOp::eDontCare;
    case nx::EStoreOp::eUndefined:
        return vk::AttachmentStoreOp::eNone;
    default:
        std::unreachable();
    }
}



vk::ImageLayout ToVulkan(EImageLayout layout)
{
    switch (layout) {
    case EImageLayout::eUndefined:
        return vk::ImageLayout::eUndefined;
    case EImageLayout::eCommon:
    case EImageLayout::eUnorderedAccess:
        return vk::ImageLayout::eGeneral;
    case EImageLayout::eUninitialized:
        return vk::ImageLayout::eUndefined;
    case EImageLayout::eRenderTarget:
        return vk::ImageLayout::eColorAttachmentOptimal;
    case EImageLayout::eDepthStencilWrite:
        return vk::ImageLayout::eDepthStencilAttachmentOptimal;
    case EImageLayout::eDepthStencilRead:
        return vk::ImageLayout::eDepthStencilReadOnlyOptimal;
    case EImageLayout::eShaderResource:
        return vk::ImageLayout::eShaderReadOnlyOptimal;
    case EImageLayout::eCopySource:
        return vk::ImageLayout::eTransferSrcOptimal;
    case EImageLayout::eCopyDestination:
        return vk::ImageLayout::eTransferDstOptimal;
    case EImageLayout::ePresent:
        return vk::ImageLayout::ePresentSrcKHR;
    case EImageLayout::eResolveSource:
    case EImageLayout::eResolveDestination:
        return vk::ImageLayout::eTransferSrcOptimal;
    default:
        return vk::ImageLayout::eUndefined;
    }
}





} // namespace nx