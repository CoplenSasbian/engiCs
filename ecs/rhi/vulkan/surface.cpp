#include "surface.h"
#include "allocator_callbacks.h"

nx::VkSurface::VkSurface(vk::Instance& i, vk::SurfaceKHR&& s)
	: m_instance(i), m_surface(std::move(s))
{
}


nx::VkSurface::~VkSurface()
{
	if(m_surface)
		m_instance.destroySurfaceKHR(m_surface, GetVulkanAllocatorCallbacks());
}

const vk::SurfaceKHR& nx::VkSurface::Get() const
{
	return  m_surface;
}
