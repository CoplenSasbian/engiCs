#include "error_code.h"
#include <spirv_reflect.h>


const std::error_category& nx::gRhiCategory()noexcept
{
	static EcsRhiCategory c;
	return c;
}

const std::error_category& nx::gSpvRefCategory() noexcept
{
	static nx::SpvRefCategory c;
	return c;
}

std::error_code nx::makeSpvErrorCode(int e) noexcept
{
	return std::error_code(static_cast<int>(e),gSpvRefCategory());
}

#if HasVulkan
std::error_code nx::make_error_code(vk::Result e)noexcept
{
	return std::error_code(static_cast<int>(e), gRhiCategory());
}
std::error_code nx::make_error_code(VkResult e)noexcept
{
	return  std::error_code(static_cast<int>(e), gRhiCategory());
}
#endif


const char* nx::EcsRhiCategory::name() const noexcept
{
	return "rhi vulkan error";
}

std::string nx::EcsRhiCategory::message(int code) const
{
#if HasVulkan
	auto errc = static_cast<vk::Result>(code);
	return vk::to_string(errc);
#endif
}


const char* nx::SpvRefCategory::name() const noexcept
{
	return "rhi Ref error";
}

std::string nx::SpvRefCategory::message(int code) const
{
	auto spvRes = static_cast<SpvReflectResult>(code);

    switch (spvRes) {
    case SPV_REFLECT_RESULT_SUCCESS:                      return "Success";
    case SPV_REFLECT_RESULT_NOT_READY:                    return "Not Ready";
    case SPV_REFLECT_RESULT_ERROR_PARSE_FAILED:           return "Parse Failed";
    case SPV_REFLECT_RESULT_ERROR_ALLOC_FAILED:           return "Memory Allocation Failed";
    case SPV_REFLECT_RESULT_ERROR_RANGE_EXCEEDED:         return "Range Exceeded";
    case SPV_REFLECT_RESULT_ERROR_NULL_POINTER:           return "Null Pointer";
    case SPV_REFLECT_RESULT_ERROR_INTERNAL_ERROR:         return "Internal Error";
    case SPV_REFLECT_RESULT_ERROR_COUNT_MISMATCH:         return "Count Mismatch";
    case SPV_REFLECT_RESULT_ERROR_ELEMENT_NOT_FOUND:      return "Element Not Found";
    case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_CODE_SIZE:return "Invalid SPIR-V Code Size";
    case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_MAGIC_NUMBER: return "Invalid SPIR-V Magic Number (File corrupted or not SPIR-V)";
    case SPV_REFLECT_RESULT_ERROR_SPIRV_UNEXPECTED_EOF:   return "Unexpected End of File";
    case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_ID_REFERENCE: return "Invalid ID Reference";
    case SPV_REFLECT_RESULT_ERROR_SPIRV_SET_NUMBER_OVERFLOW: return "Set Number Overflow";
    case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_STORAGE_CLASS: return "Invalid Storage Class";
    case SPV_REFLECT_RESULT_ERROR_SPIRV_RECURSION:        return "Recursion Detected";
    case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_INSTRUCTION: return "Invalid Instruction";
    case SPV_REFLECT_RESULT_ERROR_SPIRV_UNEXPECTED_BLOCK_DATA: return "Unexpected Block Data";
    case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_BLOCK_MEMBER_REFERENCE: return "Invalid Block Member Reference";
    case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_ENTRY_POINT: return "Invalid Entry Point (Name mismatch?)";
    case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_EXECUTION_MODE: return "Invalid Execution Mode";
    default:                                              return "Unknown Error";
    }
}