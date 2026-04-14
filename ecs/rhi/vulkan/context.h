#pragma once
#include "rhi/context.h"
#include "rhi/surface.h"
#include <vulkan/vulkan.hpp>
#include "core/memory/memory.h"
#include "rhi/pipeline.h"

namespace nx {


	namespace 
	{
		struct QueueFamilyIndices
		{
			struct QueueInfo{
				size_t index = 0;
				uint8_t flags = 0;
			};
			std::vector<QueueInfo> queueInfos;
			[[nodiscard]] bool IsComplete() const noexcept
			{
				EQueueType requiredFlags = EQueueType(EQueueTypeBits::eDirect)| EQueueTypeBits::eCompute | EQueueTypeBits::eTransfer | EQueueTypeBits::ePresent;
				uint8_t foundFlags = 0;
				for (const auto& qf : queueInfos)
				{
					foundFlags |= qf.flags;
				}
				return (foundFlags & requiredFlags) == requiredFlags;
			}
		};

		struct SwapChainSupportDetails
		{
			vk::SurfaceCapabilitiesKHR capabilities;
			std::vector<vk::SurfaceFormatKHR> formats;
			std::vector<vk::PresentModeKHR> presentModes;
		};
	}



	class VkContext :public RhiContext
	{
	public:

		VkContext(vk::Instance& instance )noexcept;
		~VkContext()override;
		Error Initialize(RhiSurface*)noexcept override;
		Result<CommonPtr<RhiSwapChain>> CreateSwapChain(RhiSurface*) noexcept override;

		Result<CommonPtr<RhiImage>> CreateImage(const ImageCreateInfo&) noexcept override;
		Result<CommonPtr<RhiBuffer>> CreateBuffer(const BufferCreateInfo&) noexcept override;
		Result<CommonPtr<RhiRenderPass>> CreateRenderPass(const RenderPassCreateInfo&) noexcept override;
		Result<CommonPtr<RhiCmdBuffer>> CreateCommandBuffer( EQueueType) noexcept override;
		Result<CommonPtr<RhiShader>> CreateShader(const ShaderCreateInfo&) noexcept override;
		Result<CommonPtr<RhiPipeline>> CreatePipeline(const PipelineCreateInfo&) noexcept override;
		vk::Queue& GetQueue(EQueueType type)noexcept;
		uint32_t GetQueueIndex(EQueueType type) noexcept ;



	private:
		Error PickPhysicalDevice()noexcept;
		QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice& device) noexcept;
		Error CreateLogicalDevice()noexcept;
		Error CreateQueues()noexcept;
		Error CreateCommandPool()noexcept;



	private:
		vk::Instance& m_instance;
		RhiSurface * m_surface;

		vk::PhysicalDevice m_physicalDevice;
		vk::Device m_device;

		QueueFamilyIndices m_queueFamilyIndices;
		std::vector<vk::Queue> m_queues;
		std::vector<vk::CommandPool> m_cmdPools;




		std::vector<std::pair<EQueueType, uint32_t>> m_queueIndices;

	};



		
}
