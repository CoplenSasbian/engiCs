#pragma once
#include "rhi/device.h"
#include "rhi/surface.h"
#include <vulkan/vulkan.hpp>
#include "core/memory/memory.h"


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



	class VkDevice :public RhiDevice
	{
	public:

		VkDevice(vk::Instance& instance )noexcept;
		~VkDevice()override;
		Error Initialize(RhiSurface*)noexcept override;
		Result<CommonPtr<RhiSwapChain>> CreateSwapChain(RhiSurface*) noexcept override;

		RhiQueue* GetQueue(EQueueType type)noexcept override;
		Result<CommonPtr<RhiImage>> CreateImage(const ImageCreateInfo&) noexcept override;
		Result<CommonPtr<RhiBuffer>> CreateBuffer(const BufferCreateInfo&) noexcept override;
		Result<CommonPtr<RhiRenderPass>> CreateRenderPass(const RenderPassCreateInfo&) noexcept override;


		uint32_t GetQueueIndex(EQueueType type) noexcept ;



	private:
		Error PickPhysicalDevice()noexcept;
		QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice& device) noexcept;
		Error CreateLogicalDevice()noexcept;
		Error CreateQueues()noexcept;



	private:
		vk::Instance& m_instance;
		RhiSurface * m_surface;

		vk::PhysicalDevice m_physicalDevice;
		vk::Device m_device;

		QueueFamilyIndices m_queueFamilyIndices;
		std::vector<CommonPtr<RhiQueue>> m_queues;
		std::vector<std::pair<EQueueType, uint32_t>> m_queueIndices;
	};



		
}