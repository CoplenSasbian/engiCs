module;
#include "vulkan/vulkan.hpp"
export module nx.rhi.vk.queue;
export import nx.rhi.queue;

export namespace nx {

	class VkQueue : public Queue {
	public:
		VkQueue() = default;

		VkQueue(vk::Queue&& queue, vk::Device device, QueueType type, uint32_t familyIndex);

		VkQueue(VkQueue&& other) noexcept = default;
		VkQueue& operator=(VkQueue&& other) noexcept = default;
        ~VkQueue() override = default;

		vk::Queue queue;
		vk::Device device;
		QueueType type;
		uint32_t familyIndex{};
	};


}