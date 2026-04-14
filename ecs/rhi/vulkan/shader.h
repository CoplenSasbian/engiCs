#pragma once
#include "rhi/shader.h"
#include "core/error_code.h"
#include "core/memory/memory.h"
#include <vulkan/vulkan.hpp>
#include <spirv_reflect.h>

namespace nx{

    class VkShader : public RhiShader
    {
    public:
        static nx::Result<nx::CommonPtr<VkShader>> DeviceCreate(vk::Device& d, ShaderCreateInfo tInfo);

        ~VkShader() override;
        VkShader(vk::Device&,const ShaderCreateInfo& info);
        EShaderTypeBits GetType() noexcept override;
        const std::string& GetEntry()noexcept override;

        vk::ShaderModule& GetModule()noexcept;


    private:
        vk::Device& m_device;
        vk::ShaderModule m_shaderModule;
        ShaderCreateInfo m_info;
        SpvReflectShaderModule m_refMod;
        EShaderTypeBits m_type;
        std::string m_entryProt;
    };

}
