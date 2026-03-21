#pragma once
#include <cstdint>
#include <type_traits>
namespace nx
{
 
    union Color
    {
        struct {
            float r, g, b, a;
        }f;
        
        struct  {
            uint32_t i[4];
        } u;

    };


	template<typename T>
    struct Flag
    {
		using UnderlyingType = std::underlying_type_t<T>;
        using Bits = T;

        Flag()noexcept = default;
		Flag(T t) : value(static_cast<UnderlyingType>(t)) {}
		explicit Flag(UnderlyingType t) : value(t) {}
        Flag(const Flag&)noexcept = default;
		Flag& operator=(const Flag&)noexcept = default;

        Flag operator|(const Flag& other) const noexcept {
            return Flag(value | other.value);
		}

        Flag operator&(const Flag& other) const noexcept {
            return Flag(value & other.value);
		}

        Flag& operator|=(const Flag& other) noexcept {
            value |= other.value;
            return *this;
        }

        Flag& operator&=(const Flag& other) noexcept {
            value &= other.value;
            return *this;
		}

        operator UnderlyingType() const noexcept {
            return value;
		}

        UnderlyingType value;
    };


    enum class EQueueTypeBits :uint8_t {
        eDirect = 1,      // (Vulkan: Graphics, DX12: Direct)
        eCompute = 1 << 1, // (Vulkan: Compute, DX12: Compute)
        eTransfer = 1 << 2, // (Vulkan: Transfer, DX12: Copy)
        ePresent = 1 << 3  // (Vulkan: Present, DX12: N/A - Handled by DXGI)
    };
    using EQueueType = Flag<EQueueTypeBits>;


    enum class EShaderTypeBits : uint8_t
    {
        eVertex = 1,                  // (Vulkan: Vertex, DX12: Vertex)
        eFragment = 1 << 1,           // (Vulkan: Fragment, DX12: Pixel)
        eCompute = 1 << 2,            // (Vulkan: Compute, DX12: Compute)
        eGeometry = 1 << 3,           // (Vulkan: Geometry, DX12: Geometry)
        eTessellationControl = 1 << 4,       // (Vulkan: Tessellation Control, DX12: Hull)
        eTessellationEvaluation = 1 << 5,    // (Vulkan: Tessellation Evaluation, DX12: Domain)
    };
	using EShaderType = Flag<EShaderTypeBits>;

    enum class EPixelFormat : uint8_t
    {
        eR8G8B8A8Unorm,
        eB8G8R8A8Unorm,
        eR16G16B16A16Sfloat,
        eD32Sfloat,
        eD24UnormS8Uint,
    };  
    

    enum class EVertexFormat : uint8_t
    {
		eUndefined,
        eFloat32,
        eFloat32x2,
        eFloat32x3,
        eFloat32x4,
        eUint32,
        eUint32x2,
        eUint32x3,
        eUint32x4,
	};
 

    enum class EImageUsageBits : uint8_t {
        eUndefined = 0,
        eSampledImage = 1 << 0,         // (Vulkan: SAMPLED, DX12: SRV)
        eStorageImage = 1 << 1,         // (Vulkan: STORAGE, DX12: UAV)
        eColorAttachment = 1 << 2,      // (Vulkan: COLOR_ATTACHMENT, DX12: RTV)
        eDepthStencilAttachment = 1 << 3, // (Vulkan: DEPTH_STENCIL_ATTACHMENT, DX12: DSV)
        eTransferSrc = 1 << 4,          // (Vulkan: TRANSFER_SRC, DX12: Used for copy operations)
        eTransferDst = 1 << 5,          // (Vulkan: TRANSFER_DST, DX12: Used for copy operations)
    };


	using EImageUsage = Flag<EImageUsageBits>;


    enum class EBufferUsageBits : uint8_t {
        eUndefined = 0,
        eVertexBuffer = 1 << 0,     // (Vulkan: VERTEX_BUFFER, DX12: VertexBufferView)
        eIndexBuffer = 1 << 1,      // (Vulkan: INDEX_BUFFER, DX12: IndexBufferView)
        eUniformBuffer = 1 << 2,    // (Vulkan: UNIFORM_BUFFER, DX12: CBV)
        eStorageBuffer = 1 << 3,    // (Vulkan: STORAGE_BUFFER, DX12: UAV)
        eIndirectBuffer = 1 << 4,   // (Vulkan: INDIRECT_BUFFER, DX12: IndirectArguments)
        eTransferSrc = 1 << 5,      // (Vulkan: TRANSFER_SRC, DX12: Used for copy operations)
        eTransferDst = 1 << 6,      // (Vulkan: TRANSFER_DST, DX12: Used for copy operations)
    };

	using EBufferUsage = Flag<EBufferUsageBits>;

   

    enum class EImageViewType : uint8_t
    {
        e1D,
        e2D,
        e3D,
        eCube,
        e1DArray,
        e2DArray,
        eCubeArray
	};


    enum class EImageAspectFlagBits : uint8_t
    {
        eColor = 1,
        eDepth = 1 << 1,
        eStencil = 1 << 2,
	};


	using EImageAspectFlags = Flag<EImageAspectFlagBits>;


    enum class ESwizzleMapping  : uint8_t{
        eIdentity,
        eZero,
        eOne,
        eR,
        eG,
        eB,
		eA
    };


 
    enum class ELoadOp : uint8_t {
        eUndefined = 0,
        eLoad,           ///(Vulkan: LOAD, DX12: PRESERVE)
        eClear,          ///(Vulkan: LOAD, DX12: CLEAR)
        eDiscard,        ///(Vulkan: DONT_CARE, DX12: DISCARD)
    };

    enum class EStoreOp  : uint8_t{
        eUndefined = 0,
        eStore,          ///(Vulkan: STORE, DX12: PRESERVE)
        eDiscard,        ///(Vulkan: DONT_CARE, DX12: DISCARD)
    };

    enum class ESampleCountFlagBits : uint8_t{
        e1 = 1,
        e2 = 1<< 1,
        e3 = 1<< 2,
        e8 = 1<< 3,
        e16= 1<< 4,
        e32= 1<< 5,
        e64= 1<< 6
    };

    using ESampleCountFlags = Flag<ESampleCountFlagBits>;

    enum class EImageLayout  : uint8_t{
        eUndefined,         
        eCommon,            
        eUninitialized,     

        eRenderTarget,      
        eDepthStencilWrite, 
        eDepthStencilRead,  

       
        eShaderResource, 
        eUnorderedAccess,

     
        eCopySource,
        eCopyDestination,

        
        ePresent,            
        eResolveSource,      
        eResolveDestination, 
    
    };

    enum class EAttachmentType  : uint8_t{
        Color,       
        Depth,       
        DepthStencil,
    };
}