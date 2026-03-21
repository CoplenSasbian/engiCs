#pragma once
#include "rhi/render_pass.h"
#include <vulkan/vulkan.hpp>

namespace nx {
	class VkRenderPass:public RhiRenderPass {
	public:

		VkRenderPass(vk::Device& device, vk::RenderPass&& renderPass);
		~VkRenderPass() override;
	private:
		vk::Device& m_device;
		vk::RenderPass m_renderPass;
	};


}
