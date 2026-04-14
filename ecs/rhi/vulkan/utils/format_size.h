#include <cstdint>
#include <vulkan/vulkan.hpp>

namespace nx {
	uint32_t FormatSize(VkFormat format);


	inline
	uint32_t FormatSize(vk::Format format) {
		return FormatSize(static_cast<VkFormat>(format));
	}
}



