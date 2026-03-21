#pragma once
#include "rhi/command_pool.h"
#include <vulkan/vulkan.hpp>

namespace nx
{
    class VkCommandPoll : public RhiCommandPool
    {
    public:
        ~VkCommandPoll() override;
        Error Initialize(RhiQueue*) noexcept override;
        RhiCommandBuffer* GetBuffer() noexcept override;
        Error Reset() noexcept override;
        Error Submit() noexcept override;
    private:
        RhiQueue* m_queue = nullptr;
        vk::Device* m_device = nullptr;
        vk::Queue* m_vkQueue = nullptr;
        vk::CommandPool m_commandPool;
        

    };
}