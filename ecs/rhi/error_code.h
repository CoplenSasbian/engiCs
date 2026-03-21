#pragma once
#include "core/error_code.h"
#if __has_include(<vulkan/vulkan.hpp>)
#include <vulkan/vulkan.hpp>
#define HasVulkan 1
#endif


namespace nx
{
	

	class EcsRhiCategory : public std::error_category
	{
	public:
		[[nodiscard]] const char* name() const noexcept override;;
		[[nodiscard]] std::string message(int code) const override;;
	};

	const std::error_category& gRhiCategory()noexcept;

#if HasVulkan
	std::error_code make_error_code(vk::Result e) noexcept;
	std::error_code make_error_code(VkResult e)noexcept;
#endif


}

namespace std {
	template <>
	struct is_error_code_enum<nx::EcsRhiCategory> : true_type {};
}
#if HasVulkan
#define VK_RETURN_ON_ERROR(expr) if(vk::Result _err = (expr); _err != vk::Result::eSuccess) { return nx::make_error_code(_err); }
#define C_VK_RETURN_ON_ERROR(expr) if(VkResult _err = (expr); _err != VK_SUCCESS) { return nx::make_error_code(_err); }

#define VK_UNEXPECTED_ON_ERROR(expr) if(vk::Result _err = (expr); _err != vk::Result::eSuccess) { return Unexpected(nx::make_error_code(_err)); }
#define C_VK_UNEXPECTED_ON_ERROR(expr) if(VkResult _err = (expr); _err != VK_SUCCESS) { return Unexpected(nx::make_error_code(_err)); }

#endif