
module;

#include <vulkan/vulkan_raii.hpp>
module nx.rhi.vk.queue;
nx::VkQueue::VkQueue(vk::Queue &&queue, vk::Device device, QueueType type, uint32_t familyIndex)
: queue(queue), device(device), type(type), familyIndex(familyIndex)
{
}
