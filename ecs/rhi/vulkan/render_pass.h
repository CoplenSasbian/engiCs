#pragma once
#include "rhi/render_pass.h"
#include <vulkan/vulkan.hpp>

namespace nx {
	class VkRenderPass:public RhiRenderPass {
	public:
		static nx::Result<nx::CommonPtr<nx::RhiRenderPass>> DeviceCreate(vk::Device& d, const RenderPassCreateInfo& tInfo) noexcept;
		VkRenderPass(vk::Device& device, vk::RenderPass&& renderPass);
		~VkRenderPass() override;
	private:
		vk::Device& m_device;
		vk::RenderPass m_renderPass;
	};


}
