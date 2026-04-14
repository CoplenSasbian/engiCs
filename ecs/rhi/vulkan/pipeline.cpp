#include"pipeline.h"
#include "defs.h"
#include "allocator_callbacks.h"
#include "rhi/error_code.h"
nx::Result<nx::CommonPtr<nx::VkPipeline>> nx::VkPipeline::DeviceCreate(vk::Device& d,
                                                                       const PipelineCreateInfo& tInfo) noexcept
{
    std::byte pmrBuffer[4096];
    std::pmr::monotonic_buffer_resource pool(pmrBuffer, sizeof(pmrBuffer));

    // Shader


    VkShader* vertexShader = nullptr;
    std::pmr::vector<vk::PipelineShaderStageCreateInfo> shaderStages{&pool};
    RETURN_ON_ERROR(FillShaderStages(shaderStages,tInfo,vertexShader));
    // Vertex Input
    std::pmr::vector<vk::VertexInputAttributeDescription> vertexInputAttributes{&pool};
    std::pmr::vector<vk::VertexInputBindingDescription> vertexInputBindings{&pool};
    RETURN_ON_ERROR(FillVertexInputState(vertexInputAttributes, vertexInputBindings, vertexShader));
    vk::PipelineVertexInputStateCreateInfo vertexInputState;
    vertexInputState.setVertexAttributeDescriptions(vertexInputAttributes)
                    .setVertexBindingDescriptions(vertexInputBindings);

    // // input assembly
     vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
    // UNEXPECT_ON_RETURN_ERROR(FillInputAssemblyState(inputAssembly,tInfo));
    inputAssembly.setPrimitiveRestartEnable(false);

    // Tessellation
    vk::PipelineTessellationStateCreateInfo tessellationState{};
    RETURN_ON_ERROR(FillTessellationState(tessellationState,tInfo));
    // Rasterization
    vk::PipelineRasterizationStateCreateInfo rasterizationState{};
    RETURN_ON_ERROR(FillRasterizationState(rasterizationState,tInfo));
    // muti sample
    vk::PipelineMultisampleStateCreateInfo multisampleState{};
    RETURN_ON_ERROR(FillMultisampleState(multisampleState,tInfo));

    // depth stencil
    vk::PipelineDepthStencilStateCreateInfo depthStencilState{};
    RETURN_ON_ERROR(FillDepthStencilState(depthStencilState,tInfo));
    // color blend
    vk::PipelineColorBlendStateCreateInfo colorBlendState{};
    RETURN_ON_ERROR(FillColorBlendState(colorBlendState,tInfo));

    // dynamic state
    auto dynamicStates = {
        vk:: DynamicState::eViewport,
        vk:: DynamicState::eScissor,
        vk:: DynamicState::ePolygonModeEXT
    };

    vk::PipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.setDynamicStates(dynamicStates);

    // uniforms

	std::pmr::vector<vk::DescriptorSetLayout> descriptorSetLayouts{ &pool };
	std::pmr::vector<vk::PushConstantRange> pushConstantRanges{ &pool };

    RETURN_ON_ERROR(FillDescriptorSetLayouts(descriptorSetLayouts, tInfo));
    RETURN_ON_ERROR(FillPushConstantRanges(pushConstantRanges, tInfo));

	vk::PipelineLayoutCreateInfo layoutInfo{};
    layoutInfo.setSetLayouts(descriptorSetLayouts)
        .setPushConstantRanges(pushConstantRanges);
    vk::PipelineLayout layout;
    VK_UNEXPECT_ON_ERROR(d.createPipelineLayout(&layoutInfo, GetVulkanAllocatorCallbacks(), &layout));



    vk::GraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.setStages(shaderStages)
                .setPVertexInputState(&vertexInputState)
                .setPInputAssemblyState(&inputAssembly)
                .setPTessellationState(&tessellationState)
                .setPRasterizationState(&rasterizationState)
                .setPMultisampleState(&multisampleState)
                .setPDepthStencilState(&depthStencilState)
                .setPColorBlendState(&colorBlendState)
                .setPDynamicState(&dynamicState)
        .setLayout(layout);




    return Unexpected(nx::make_error_code(EcsErrc::Success));
}

nx::VkPipeline::VkPipeline(vk::Device& d)
    : m_device(d)
{
}

nx::Error nx::VkPipeline::FillShaderStages(std::pmr::vector<vk::PipelineShaderStageCreateInfo>& shaderStages,
                                           const PipelineCreateInfo& tInfo,
                                           VkShader*& vertexShaders) noexcept
{
    shaderStages.reserve(tInfo.shaders.size());
    for (auto& shader : tInfo.shaders)
    {
        auto* vkShader = static_cast<VkShader*>(shader);
        vk::PipelineShaderStageCreateInfo stageInfo{};
        stageInfo.stage = ToVulkan(vkShader->GetType());
        stageInfo.module = vkShader->GetModule();
        stageInfo.pName = vkShader->GetEntry().data(); // assume null-terminated
        shaderStages.push_back(stageInfo);
        if (vkShader->GetType() == EShaderTypeBits::eVertex)
        {
            vertexShaders = vkShader;
        }
    }

    if (vertexShaders == nullptr)return nx::Unexpected(nx::make_error_code(EcsErrc::VertexShaderNotFound));

    return Succeeded;
}

nx::Error nx::VkPipeline::FillVertexInputState(
    std::pmr::vector<vk::VertexInputAttributeDescription>& vertexInputAttributes,
    std::pmr::vector<vk::VertexInputBindingDescription>& vertexInputBindings, VkShader* vertexShader) noexcept
{

    return Succeeded;
}



nx::Error nx::VkPipeline::FillTessellationState(vk::PipelineTessellationStateCreateInfo& tessellationState,
    const PipelineCreateInfo& tInfo) noexcept
{


    return Succeeded;
}

nx::Error nx::VkPipeline::FillRasterizationState(vk::PipelineRasterizationStateCreateInfo& rasterizationState,
    const PipelineCreateInfo& tInfo) noexcept
{


    return Succeeded;
}

nx::Error nx::VkPipeline::FillMultisampleState(vk::PipelineMultisampleStateCreateInfo& multisampleState,
    const PipelineCreateInfo& tInfo) noexcept
{
    return Succeeded;
}

nx::Error nx::VkPipeline::FillDepthStencilState(vk::PipelineDepthStencilStateCreateInfo& depthStencilState,
    const PipelineCreateInfo& tInfo) noexcept
{
    return Succeeded;
}

nx::Error nx::VkPipeline::FillColorBlendState(vk::PipelineColorBlendStateCreateInfo& colorBlendState,
    const PipelineCreateInfo& tInfo) noexcept
{
    return Succeeded;
}

nx::Error nx::VkPipeline::FillDescriptorSetLayouts(std::pmr::vector<vk::DescriptorSetLayout>& descriptorSetLayouts, const PipelineCreateInfo& tInfo) noexcept
{
    return Succeeded;
}

nx::Error nx::VkPipeline::FillPushConstantRanges(std::pmr::vector<vk::PushConstantRange>& pushConstantRanges, const PipelineCreateInfo& tInfo) noexcept
{
    return Succeeded;
}


