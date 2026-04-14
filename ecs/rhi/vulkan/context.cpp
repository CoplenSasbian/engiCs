#include "context.h"
#include <ranges>
#include "instance.h"
#include "rhi/error_code.h"
#include "allocator_callbacks.h"
#include "config.h"
#include "defs.h"
#include "resource.h"
#include "surface.h"
#include "core/memory/memory.h"
#include "swap_chain.h"
#include "render_pass.h"
#include "command_buffer.h"
#include "shader.h"
#include "pipeline.h"


nx::VkContext::VkContext(vk::Instance& instance ) noexcept
	: m_instance(instance), m_surface(nullptr)
{
}

nx::VkContext::~VkContext()
{
	if (m_device)
		m_device.destroy();
}

nx::Error nx::VkContext::Initialize(RhiSurface * surface)noexcept
{
	m_surface = surface;
	return PickPhysicalDevice()
		.and_then([&]() { return CreateLogicalDevice(); })
		.and_then([&]() { return CreateQueues(); })
		.and_then([&]() { return CreateCommandPool(); });

}

nx::Result<nx::CommonPtr<nx::RhiSwapChain>> nx::VkContext::CreateSwapChain(RhiSurface*) noexcept
{
	return MakeCommonPtr<VkSwapChain>(m_device,m_physicalDevice);
}

vk::Queue& nx::VkContext::GetQueue(EQueueType type) noexcept
{
	auto index = GetQueueIndex(type);
	return m_queues[index];
}


nx::Result<nx::CommonPtr<nx::RhiImage>> nx::VkContext::CreateImage(const ImageCreateInfo& info) noexcept
{
	return VkImage::DeviceCreate(m_device, info);
}

nx::Result<nx::CommonPtr<nx::RhiBuffer>> nx::VkContext::CreateBuffer(const BufferCreateInfo& tInfo) noexcept
{
	return VkBuffer::DeviceCreate(m_device, tInfo);
}
nx::Result<nx::CommonPtr<nx::RhiRenderPass>> nx::VkContext::CreateRenderPass(const RenderPassCreateInfo& tInfo) noexcept
{
	return VkRenderPass::DeviceCreate(m_device, tInfo);
}

uint32_t nx::VkContext::GetQueueIndex(EQueueType type) noexcept
{
	auto requiredFlags = type.value;
	uint32_t result = std::numeric_limits<uint32_t>::max();
	int count = std::numeric_limits<int>::max();
	for (auto& [flags,index] : m_queueIndices)
	{
		if ((flags & requiredFlags) == requiredFlags) {
			auto ext = static_cast<std::make_unsigned_t<decltype(flags.value)>>(flags & ~requiredFlags);
			auto extCount = std::popcount(ext);
			if (extCount == 0) {
				return index;
			}
			if (extCount < count) {
				result = index;
				count = extCount;
			}
		}
	}
	return result;
}


nx::Error nx::VkContext::PickPhysicalDevice()noexcept
{
	auto& nativeInstance = m_instance;

	auto deviceExtensionsConfig = EngineConfig::GetRequiredDeviceExtensions(true);
	auto devices = nativeInstance.enumeratePhysicalDevices();
	for (auto& device : devices)
	{
		auto props = device.getProperties();
		if (props.deviceType != vk::PhysicalDeviceType::eDiscreteGpu) {
			continue;
		}

		if (props.apiVersion < VulkanVersion) {
			continue;
		}
		auto q=  findQueueFamilies(device);
		if (!q.IsComplete() ){
			continue;
		}
		m_physicalDevice = device;
		m_queueFamilyIndices.queueInfos = std::move(q.queueInfos);
		return Succeeded;
	}
	return nx::Unexpected(nx::make_error_code(vk::Result::eErrorDeviceLost));

}

nx::QueueFamilyIndices nx::VkContext::findQueueFamilies(const vk::PhysicalDevice& device) noexcept
{
	QueueFamilyIndices indices;

	uint32_t i = 0;
	for (const auto& queueFamily : device.getQueueFamilyProperties()) {
		
		auto flags = FromVulkan(queueFamily.queueFlags);

		auto surface = dynamic_cast<VkSurface*>(m_surface);
		if (surface&&m_physicalDevice.getSurfaceSupportKHR(i,surface->Get())) {
			flags |= EQueueTypeBits::ePresent;
		}

		indices.queueInfos.push_back({ i, flags });
		i++;
	}

	return indices;
}

nx::Error nx::VkContext::CreateLogicalDevice()noexcept
{
	float queuePriority = 1.0f;

	auto queueIndexes = m_queueFamilyIndices.queueInfos
		| std::views::transform(
			[&](const QueueFamilyIndices::QueueInfo& qf) {
				return vk::DeviceQueueCreateInfo (
					vk::DeviceQueueCreateFlags(),
					qf.index,
					1,
					&queuePriority
				);
			})
		| std::ranges::to<std::vector>();

	auto extensionsConfig = EngineConfig::GetRequiredDeviceExtensions(true);
	auto extensions = extensionsConfig | std::views::transform([](const VulkanExtensionLayerInfo& info) { return info.name.c_str(); }) | std::ranges::to<std::vector<const char*>>();

	auto layersConfig = EngineConfig::GetRequiredDeviceLayers(true);
	auto layers = layersConfig | std::views::transform([](const VulkanExtensionLayerInfo& info) { return info.name.c_str(); }) | std::ranges::to<std::vector<const char*>>();

	vk::DeviceCreateInfo createInfo{};
	createInfo.setEnabledExtensionCount(static_cast<uint32_t>(extensions.size()));
	createInfo.setPpEnabledExtensionNames(extensions.data());
	createInfo.setEnabledLayerCount(static_cast<uint32_t>(layers.size()));
	createInfo.setPpEnabledLayerNames(layers.data());
	createInfo.setQueueCreateInfoCount(static_cast<uint32_t>(queueIndexes.size()));
	createInfo.setPQueueCreateInfos(queueIndexes.data());

	VK_RETURN_IF_ERROR( m_physicalDevice.createDevice(&createInfo, GetVulkanAllocatorCallbacks(), &m_device));


	return Succeeded;
}

nx::Error nx::VkContext::CreateQueues()noexcept
{
	for (auto& info : m_queueFamilyIndices.queueInfos)
	{
		auto queue = m_device.getQueue(info.index, 0);
		m_queues.push_back(queue);
		m_queueIndices.emplace_back(info.flags, m_queues.size() - 1);
	}
	return Succeeded;
}

nx::Error nx::VkContext::CreateCommandPool() noexcept
{
	m_cmdPools.reserve(m_queues.size());
	for (size_t i = 0; i < m_queues.size(); i++)
	{
		auto& queue = m_queues[i];
		vk::CommandPoolCreateInfo info{};
		info.setQueueFamilyIndex(i);
		vk::CommandPool pool;
		VK_RETURN_IF_ERROR(m_device.createCommandPool(&info, GetVulkanAllocatorCallbacks(), &pool));
		m_cmdPools.emplace_back(std::move(pool));
	}
	return Succeeded;
}

nx::Result<nx::CommonPtr<nx::RhiCmdBuffer>> nx::VkContext::CreateCommandBuffer(const EQueueType type) noexcept
{
	auto index = GetQueueIndex(type);
	if (index >= m_cmdPools.size()) return Unexpected(make_error_code(EcsErrc::InvalidQueueType));
	auto& pool = m_cmdPools[index];
	auto& queue = m_queues[index];
	return MakeCommonPtr<VkCmdBuffer>(m_device, pool, queue,type);
	
}

nx::Result<nx::CommonPtr<nx::RhiShader>> nx::VkContext::CreateShader(
	const ShaderCreateInfo& tInfo) noexcept
{
	return  VkShader::DeviceCreate(m_device,tInfo);
}

nx::Result<nx::CommonPtr<nx::RhiPipeline>> nx::VkContext::CreatePipeline(
	const PipelineCreateInfo& pipeline_create_info) noexcept
{
	return VkPipeline::DeviceCreate(m_device, pipeline_create_info);
}








