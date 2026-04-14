#include "shader.h"
#include "rhi/error_code.h"
#include "allocator_callbacks.h"
static
nx::Result<nx::EShaderTypeBits> fromSpv(SpvReflectShaderStageFlagBits b) {
    using nx::EShaderTypeBits;
    switch (b)
    {
    case SPV_REFLECT_SHADER_STAGE_VERTEX_BIT:
        return EShaderTypeBits::eVertex;
    case SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT:
        return EShaderTypeBits::eFragment;
    case SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT:
        return EShaderTypeBits::eCompute;
    case SPV_REFLECT_SHADER_STAGE_GEOMETRY_BIT:
        return EShaderTypeBits::eGeometry;
    case SPV_REFLECT_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
        return EShaderTypeBits::eTessellationControl;
    case SPV_REFLECT_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
        return EShaderTypeBits::eTessellationEvaluation;
    default:
        return nx::Unexpected(nx::make_error_code(nx::EcsErrc::UnsuppotShaderType));
    }
    
}

nx::Result<nx::CommonPtr<nx::VkShader>> nx::VkShader::DeviceCreate(vk::Device& d, ShaderCreateInfo tInfo)
{
    auto shader = MakeCommonPtr<VkShader>(d, tInfo);

    SpvReflectResult result = spvReflectCreateShaderModule(
        tInfo.spvData.size() * sizeof(uint32_t),
        tInfo.spvData.data(),
        &shader->m_refMod
    );

    if (result != SPV_REFLECT_RESULT_SUCCESS) {
    
        return Unexpected(makeSpvErrorCode(result));
    }
    ASSIGN_OR_RETURN_ERROR(shader->m_type, fromSpv(shader->m_refMod.shader_stage));
    
    shader->m_entryProt = shader->m_refMod.entry_point_name;

    vk::ShaderModuleCreateInfo info;
    info.setCode(tInfo.spvData);
    VK_UNEXPECT_ON_ERROR(d.createShaderModule(&info, GetVulkanAllocatorCallbacks(), &shader->m_shaderModule));


    return shader;
}

nx::VkShader::~VkShader()
{
    if (m_refMod.capability_count)
        spvReflectDestroyShaderModule(&m_refMod);
    if (m_shaderModule)
        m_device.destroyShaderModule(m_shaderModule);
}

nx::VkShader::VkShader(vk::Device& d, const ShaderCreateInfo& t)
    :m_device(d), m_info(t), m_refMod{}, m_type{}
{
}

nx::EShaderTypeBits nx::VkShader::GetType()noexcept
{
    return m_type;
}

const std::string& nx::VkShader::GetEntry()noexcept
{
    return m_entryProt;
}

vk::ShaderModule& nx::VkShader::GetModule()noexcept
{
    return m_shaderModule;
}

