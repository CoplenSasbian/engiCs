#pragma once
#include"rhi/surface.h"
#include <vulkan/vulkan.hpp>

namespace nx
{
	class VkSurface : public RhiSurface
	{
	public:

		VkSurface(vk::Instance& ,vk::SurfaceKHR&&);
		~VkSurface() override;

		[[nodiscard]] const vk::SurfaceKHR& Get() const ;
	private:
		vk::Instance& m_instance;
		vk::SurfaceKHR m_surface;
	};
}