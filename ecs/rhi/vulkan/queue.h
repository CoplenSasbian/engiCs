#pragma once
#include "rhi/queue.h"
#include "vulkan/vulkan.hpp"
namespace nx
{
	
	class VkQueue : public RhiQueue
	{
	public:
		VkQueue(vk::Device& d,vk::Queue&&,EQueueType  type,uint32_t  index);
		~VkQueue() override;

		EQueueType GetType() noexcept override;

		vk::Device& GetDevice() noexcept ;
		vk::Queue& GetQueue() noexcept ;
		uint32_t GetIndex() const noexcept;

	private:
		uint32_t m_index;
		EQueueType m_type;
		vk::Queue m_queue;
		vk::Device& m_device;
	};

}