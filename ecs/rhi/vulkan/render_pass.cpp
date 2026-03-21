#include "render_pass.h"
#include "allocator_callbacks.h"

nx::VkRenderPass::VkRenderPass(vk::Device& device, vk::RenderPass&& renderPass)
	: m_device(device), m_renderPass(std::move(renderPass))
{
}

nx::VkRenderPass::~VkRenderPass()
{
	if (m_renderPass)
		m_device.destroyRenderPass(m_renderPass, GetVulkanAllocatorCallbacks());
}
