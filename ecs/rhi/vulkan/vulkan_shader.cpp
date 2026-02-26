module;
#include<vulkan/vulkan_raii.hpp>
module nx.rhi.vk.shader;
import nx.rhi.shader;

vk::ShaderStageFlags nx::ToVulkanStage(ShaderStage stage) {
    switch (stage) {
        case ShaderStage::Vertex:
            return vk::ShaderStageFlagBits::eVertex;
        case ShaderStage::Fragment:
            return vk::ShaderStageFlagBits::eFragment;
        default:
           return vk::ShaderStageFlags::BitsType::eAll;
    }
}

nx::VkShader::VkShader(vk::raii::ShaderModule &&shader)
    :m_ShaderModule(std::move(shader))
{

}
