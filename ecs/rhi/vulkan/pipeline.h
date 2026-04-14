#pragma once
#include"rhi/pipeline.h"
#include <vulkan/vulkan.hpp>
#include "core/memory/memory.h"
#include "core/error_code.h"
#include "shader.h"

namespace nx {
	
	
	class VkPipeline : public RhiPipeline {
	public:
		static Result<CommonPtr<VkPipeline>> DeviceCreate(vk::Device&, const PipelineCreateInfo&) noexcept;
		
		VkPipeline(vk::Device&);
	private:

		static Error FillShaderStages(
			std::pmr::vector<vk::PipelineShaderStageCreateInfo>& shaderStages,
			const PipelineCreateInfo& tInfo,
			VkShader*& vertexShaders
			) noexcept;

		static Error FillVertexInputState(
			std::pmr::vector<vk::VertexInputAttributeDescription>& vertexInputAttributes,
			std::pmr::vector<vk::VertexInputBindingDescription>& vertexInputBindings,
			VkShader * vertexShader
		) noexcept;




		static Error FillTessellationState(
			vk::PipelineTessellationStateCreateInfo& tessellationState,
			const PipelineCreateInfo& tInfo
		) noexcept;

		static Error FillRasterizationState(
			vk::PipelineRasterizationStateCreateInfo& rasterizationState,
			const PipelineCreateInfo& tInfo
		) noexcept;

		static Error FillMultisampleState(
			vk::PipelineMultisampleStateCreateInfo& multisampleState,
			const PipelineCreateInfo& tInfo
		) noexcept;

		static Error FillDepthStencilState(
			vk::PipelineDepthStencilStateCreateInfo& depthStencilState,
			const PipelineCreateInfo& tInfo
		) noexcept;

		static Error FillColorBlendState(
			vk::PipelineColorBlendStateCreateInfo& colorBlendState,
			const PipelineCreateInfo& tInfo
		) noexcept;

		static Error FillDescriptorSetLayouts(
			std::pmr::vector<vk::DescriptorSetLayout>& descriptorSetLayouts,
			const PipelineCreateInfo& tInfo
		) noexcept;
	
		static Error FillPushConstantRanges(
			std::pmr::vector<vk::PushConstantRange>& pushConstantRanges,
			const PipelineCreateInfo& tInfo
		) noexcept;
		vk::Device& m_device;
	};
}