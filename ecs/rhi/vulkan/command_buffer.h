#pragma once
#include "rhi/command_buffer.h"
#include <vulkan/vulkan.hpp>

#include "rhi/defs.h"

namespace nx
{
    class VkCmdBuffer;

    class VkGraphicsCmdView:public RhiGraphicsCmdView
    {
    public:
        VkGraphicsCmdView( );
    private:
        friend class VkCmdBuffer;
        vk::CommandBuffer* m_cmdBuffer;
    };

    class VkComputeCmdView:public RhiComputeCmdView
    {
    public:
        VkComputeCmdView();
    private:
        friend class VkCmdBuffer;
        vk::CommandBuffer* m_cmdBuffer;
    };

    class VkTransferCmdView:public RhiTransferCmdView
    {
    public:
        VkTransferCmdView();
    private:
        friend class VkCmdBuffer;
        vk::CommandBuffer* m_cmdBuffer;
    };


    class VkCmdBuffer:public RhiCmdBuffer
    {
    public:
        ~VkCmdBuffer() override;
        VkCmdBuffer(vk::Device&, vk::CommandPool&, vk::Queue&,EQueueType type);

        void Begin() override;
        void End() override;
        void Submit() override;
        RhiGraphicsCmdView* GetGraphicsCmdView() override;
        RhiComputeCmdView* GetComputeCmdView() override;
        RhiTransferCmdView* GetTransferCmdView() override;

    private:
        vk::Device& m_device;
        vk::CommandPool& m_cmdPool;
        vk::Queue& m_queue;

        EQueueType m_type;

        vk::CommandBuffer m_cmdBuffer;

        VkGraphicsCmdView m_graphicsCmdView;
        VkComputeCmdView m_computeCmdView;
        VkTransferCmdView m_transferCmdView;

    };
}
