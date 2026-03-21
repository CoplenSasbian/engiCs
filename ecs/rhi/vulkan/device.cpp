#include "device.h"
#include <ranges>
#include "instance.h"
#include "rhi/error_code.h"
#include "allocator_callbacks.h"
#include "config.h"
#include "queue.h"
#include "defs.h"
#include "resource.h"
#include "surface.h"
#include "core/memory/memory.h"
#include "swap_chain.h"
#include "render_pass.h"
#include <list>


nx::VkDevice::VkDevice(vk::Instance& instance ) noexcept
	: m_instance(instance), m_surface(nullptr)
{
}

nx::VkDevice::~VkDevice()
{
	if (m_device)
		m_device.destroy();
}

nx::Error nx::VkDevice::Initialize(RhiSurface * surface)noexcept
{
	m_surface = surface;
	RETURN_ON_ERROR(PickPhysicalDevice());
	RETURN_ON_ERROR(CreateLogicalDevice());
	RETURN_ON_ERROR(CreateQueues());

	return Succeeded;
}

nx::Result<nx::CommonPtr<nx::RhiSwapChain>> nx::VkDevice::CreateSwapChain(RhiSurface*) noexcept
{
	return make_common_ptr<VkSwapChain>(m_device,m_physicalDevice);
}

nx::RhiQueue* nx::VkDevice::GetQueue(EQueueType type) noexcept
{
	auto index = GetQueueIndex(type);
	if (index > m_queueIndices.size()) return  nullptr;
	return m_queues[index].get();
}


nx::Result<nx::CommonPtr<nx::RhiImage>> nx::VkDevice::CreateImage(const ImageCreateInfo& info) noexcept
{

	

	vk::ImageCreateInfo createInfo{};
	createInfo.setExtent(vk::Extent3D{ info.width,info.height,info.depth })
	          .setMipLevels(info.mipLevels)
	          .setArrayLayers(info.arrayLayers)
	          .setFormat(ToVulkan(info.format))
	          .setUsage(ToVulkan(info.usage));

	vk::Image image;
	
	VK_UNEXPECTED_ON_ERROR(m_device.createImage(&createInfo, GetVulkanAllocatorCallbacks(), &image));

	return make_common_ptr<VkImage>(m_device,info,std::move(image));
	
}

nx::Result<nx::CommonPtr<nx::RhiBuffer>> nx::VkDevice::CreateBuffer(const BufferCreateInfo& tInfo) noexcept
{
	vk::BufferCreateInfo info;
	info.setUsage(ToVulkan(tInfo.usage))
	    .setSize(tInfo.size);

	vk::Buffer buffer;
	
	VK_UNEXPECTED_ON_ERROR( m_device.createBuffer(&info, GetVulkanAllocatorCallbacks(), &buffer));

	return make_common_ptr<VkBuffer>(m_device, tInfo, std::move(buffer));
}
nx::Result<nx::CommonPtr<nx::RhiRenderPass>> nx::VkDevice::CreateRenderPass(const RenderPassCreateInfo& tInfo) noexcept
{
    std::byte mem[4096];
    std::pmr::monotonic_buffer_resource resource(mem, sizeof(mem));

    std::pmr::vector<vk::AttachmentDescription> attachments{&resource};
    std::pmr::vector<vk::SubpassDescription> subpasses{&resource};

    attachments.reserve(tInfo.attachments.size());
    subpasses.reserve(tInfo.subpasses.size());

    for (auto& attachment : tInfo.attachments)
    {
        vk::AttachmentDescription desc;

        auto image = static_cast<VkImage*>(attachment.texture);

        desc.setInitialLayout(ToVulkan(attachment.initialLayout))
            .setFinalLayout(ToVulkan(attachment.finalLayout))
            .setFormat(ToVulkan(image->GetImageInfo().format))
            .setLoadOp(ToVulkan(attachment.loadOp))
            .setStoreOp(ToVulkan(attachment.storeOp));

        if (attachment.type == EAttachmentType::DepthStencil) {
            desc.setStencilLoadOp(ToVulkan(attachment.stencilOp.loadOp))
                .setStencilStoreOp(ToVulkan(attachment.stencilOp.storeOp));
        }
        attachments.push_back(desc);
    }

    std::pmr::list<std::pmr::vector<vk::AttachmentReference>> refs{&resource};

    auto pushRef = [](auto& targetRefs, const auto& attachmentDesc) {
        for (const auto& attachment : attachmentDesc)
        {
            vk::AttachmentReference ref;
            ref.setAttachment(attachment.attachmentIndex);
            ref.setLayout(ToVulkan(attachment.layout));
            targetRefs.push_back(ref);
        }
    };


    auto& depthRefs = refs.emplace_back();
    depthRefs.reserve(tInfo.subpasses.size());

    for (auto& subpass_desc : tInfo.subpasses)
    {
        auto& subDesc = subpasses.emplace_back();

        auto& colorRefs = refs.emplace_back();
        colorRefs.reserve(subpass_desc.colorAttachmentIndices.size());
        pushRef(colorRefs, subpass_desc.colorAttachmentIndices);
        subDesc.setColorAttachments(colorRefs);

        auto& inputRefs = refs.emplace_back();
        inputRefs.reserve(subpass_desc.inputAttachmentIndices.size());
        pushRef(inputRefs, subpass_desc.inputAttachmentIndices);
        subDesc.setInputAttachments(inputRefs);

        auto& resolveRefs = refs.emplace_back();
        resolveRefs.reserve(subpass_desc.resolveAttachmentIndices.size());
        pushRef(resolveRefs, subpass_desc.resolveAttachmentIndices);
        subDesc.setResolveAttachments(resolveRefs);

        if (subpass_desc.depthStencilAttachment.attachmentIndex != std::numeric_limits<uint32_t>::max())
        {
            auto& depth_ref = depthRefs.emplace_back();
            depth_ref.setAttachment(subpass_desc.depthStencilAttachment.attachmentIndex);
            depth_ref.setLayout(ToVulkan(subpass_desc.depthStencilAttachment.layout));
            subDesc.setPDepthStencilAttachment(&depth_ref);
        }
    }


    vk::RenderPassCreateInfo info;
    info.setAttachments(attachments)
        .setSubpasses(subpasses);


    vk::RenderPass renderPass;
    VK_UNEXPECTED_ON_ERROR(m_device.createRenderPass(&info, GetVulkanAllocatorCallbacks(), &renderPass));

    return make_common_ptr<VkRenderPass>(m_device, std::move(renderPass));
}

uint32_t nx::VkDevice::GetQueueIndex(EQueueType type) noexcept
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


nx::Error nx::VkDevice::PickPhysicalDevice()noexcept
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
	return nx::make_error_code(vk::Result::eErrorDeviceLost);

}

nx::QueueFamilyIndices nx::VkDevice::findQueueFamilies(const vk::PhysicalDevice& device) noexcept
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

nx::Error nx::VkDevice::CreateLogicalDevice()noexcept
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

	VK_RETURN_ON_ERROR( m_physicalDevice.createDevice(&createInfo, GetVulkanAllocatorCallbacks(), &m_device));


	return Succeeded;
}

nx::Error nx::VkDevice::CreateQueues()noexcept
{
	for (auto& info : m_queueFamilyIndices.queueInfos)
	{
		auto queue = m_device.getQueue(info.index, 0);
		auto  common_ptr = make_common_ptr<VkQueue>(m_device, std::move(queue), EQueueType{info.flags},info.index);
		m_queues.push_back(std::move(common_ptr));
		m_queueIndices.emplace_back(info.flags, m_queues.size() - 1);
	}
	return Succeeded;
}








