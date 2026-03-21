#include "command_pool.h"
#include "rhi/error_code.h"
#include "queue.h"
#include "allocator_callbacks.h"
nx::VkCommandPoll::~VkCommandPoll()
{
}

nx::Error nx::VkCommandPoll::Initialize(RhiQueue*  queue) noexcept
{
    m_queue = queue;
    auto  vk_queue = dynamic_cast<VkQueue*>(m_queue);
    m_vkQueue = & (vk_queue->GetQueue());
    m_device = & (vk_queue->GetDevice());

    vk::CommandPoolCreateInfo createInfo;
    createInfo.setQueueFamilyIndex(vk_queue->GetIndex());
    VK_RETURN_ON_ERROR(m_device->createCommandPool(&createInfo, GetVulkanAllocatorCallbacks(), &m_commandPool));
    return Succeeded;
}

nx::RhiCommandBuffer* nx::VkCommandPoll::GetBuffer() noexcept
{
    vk::CommandBufferAllocateInfo info{};
    m_device->allocateCommandBuffers(&info,)
}

nx::Error nx::VkCommandPoll::Reset() noexcept
{
}

nx::Error nx::VkCommandPoll::Submit() noexcept
{
}
