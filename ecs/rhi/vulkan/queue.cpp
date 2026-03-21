#include "queue.h"

nx::VkQueue::VkQueue(vk::Device& d,vk::Queue&& q ,EQueueType type,uint32_t index)
	: m_index(index),m_type(type) ,m_queue(std::move(q)),m_device(d)
{
}

nx::VkQueue::~VkQueue()
{
}

nx::EQueueType nx::VkQueue::GetType() noexcept
{
	return  m_type;
}

vk::Device& nx::VkQueue::GetDevice()  noexcept
{
	return m_device;
}

vk::Queue& nx::VkQueue::GetQueue()  noexcept
{
	return m_queue;
}

uint32_t nx::VkQueue::GetIndex() const noexcept
{
	return m_index;
}
