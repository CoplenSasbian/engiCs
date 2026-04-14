//
// Created by futurvo on 2026/3/22.
//

#include "command_buffer.h"

nx::VkGraphicsCmdView::VkGraphicsCmdView() : m_cmdBuffer(nullptr)
{
}

nx::VkComputeCmdView::VkComputeCmdView() : m_cmdBuffer(nullptr)
{
}

nx::VkTransferCmdView::VkTransferCmdView() : m_cmdBuffer(nullptr)
{
}

nx::VkCmdBuffer::~VkCmdBuffer()
{
    
}

nx::VkCmdBuffer::VkCmdBuffer(vk::Device& d, vk::CommandPool& p, vk::Queue& q,EQueueType type)
    :m_device(d),m_cmdPool(p),m_queue(q),m_type(type)
{
}

void nx::VkCmdBuffer::Begin()
{
}

void nx::VkCmdBuffer::End()
{
}

void nx::VkCmdBuffer::Submit()
{
}

nx::RhiGraphicsCmdView* nx::VkCmdBuffer::GetGraphicsCmdView()
{
    assert(m_type& EQueueType::Bits::eDirect);

    m_graphicsCmdView.m_cmdBuffer = &m_cmdBuffer;
    return &m_graphicsCmdView;
}

nx::RhiComputeCmdView* nx::VkCmdBuffer::GetComputeCmdView()
{
    assert(m_type& EQueueType::Bits::eCompute);

    m_computeCmdView.m_cmdBuffer = &m_cmdBuffer;
    return &m_computeCmdView;
}

nx::RhiTransferCmdView* nx::VkCmdBuffer::GetTransferCmdView()
{
    assert(m_type& EQueueType::Bits::eTransfer);
    m_transferCmdView.m_cmdBuffer = &m_cmdBuffer;
    return &m_transferCmdView;
}
