#include "error_code.h"



const std::error_category& nx::gRhiCategory()noexcept
{
	static EcsRhiCategory c;
	return c;
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
	return "rhi error";
}

std::string nx::EcsRhiCategory::message(int code) const
{
#if HasVulkan
	auto errc = static_cast<vk::Result>(code);
	return vk::to_string(errc);
#endif
}