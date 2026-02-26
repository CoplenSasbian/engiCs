module;

#include "vulkan/vulkan_raii.hpp"
export module nx.rhi.vk.shader;
export import nx.rhi.shader;
export namespace nx {

    vk::ShaderStageFlags ToVulkanStage(ShaderStage stage);


    class VkShader: public Shader {
    public:
        VkShader(vk::raii::ShaderModule&& shader);
        ~VkShader() override= default;
    private:
        vk::raii::ShaderModule m_ShaderModule;

    };
}