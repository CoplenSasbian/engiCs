module;
#include "log/log.h"
#include <ranges>
#include <sstream>
#include <unordered_map>
#include <array>
#include <algorithm>
#include <span>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_win32.h>

#include <memory_resource>
module nx.rhi.vk.device;
import nx.core.log;
import nx.core.memory;
import nx.rhi.vk.queue;
import nx.core.exception;
import nx.rhi.vk.allocator;
import nx.rhi.vk.config;
import nx.rhi.vk.shader;
import nx.rhi.shader;
LOGGER(VK_DEVICE);
namespace nx {
	VkDevice::VkDevice(void *WINDOW_HANDLE)
		:m_hWnd(static_cast<HWND>(WINDOW_HANDLE)){
	}

	VkDevice::~VkDevice() {
		Shutdown();
	}


	void VkDevice::Initialize() {
		if (!IsWindow(m_hWnd)) {
			throw NxError("Invalid window handle");
		}
		m_allocator = CreateVulkanAllocator();
		CreateInstance();
#ifdef _DEBUG
			CreateDebugMessenger();
#endif
		PickPhysicalDevice();
		CreateSurface();
		FindQueueFamilies();
		CreateDevice();
		CreateQueues();
		CreateSwapChain();
	}
	void VkDevice::Shutdown() {}

	void VkDevice::CreateInstance() {
		vk::ApplicationInfo appInfo{
		"ecs",
		VK_MAKE_VERSION(0,0,1),
		"ecs egning",
		VK_MAKE_VERSION(0,0,1),
		VK_API_VERSION_1_4,
		nullptr
		};

		auto needLayer = EngineConfig::GetRequiredInstanceLayers();
		auto needExt = EngineConfig::GetRequiredInstanceExtensions();

		std::pmr::vector<const char*> layers;
		std::pmr::vector<const char*> extension;
		{
			uint32_t layerCount;
			vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
			std::pmr::vector<VkLayerProperties> layerPro{ layerCount };
			vkEnumerateInstanceLayerProperties(&layerCount, layerPro.data());
			auto spLayer = layerPro
				| std::views::transform([](auto& r) {return std::make_pair(std::string_view{ r.layerName }, r); })
				| std::ranges::to<std::unordered_map>();
			layers.reserve(needLayer.size());

			BEGIN_DEBUG_BLOCK();
			BLOCK_OUTPUT("Instance layers info:\n");
			for (auto& i : needLayer)
			{
				if (spLayer.contains(i.name)) {
					layers.push_back(i.name.c_str());
					BLOCK_OUTPUT("\tRequired layer ");
					BLOCK_OUTPUT(i.name);
					BLOCK_OUTPUT(" support\n");
				}
				else if (i.force) {
					throw nx::NxError(std::format("Required layer {} is not support", i.name));
				}
				else {
					BLOCK_OUTPUT("\tLayer ");
					BLOCK_OUTPUT(i.name);
					BLOCK_OUTPUT(" is not support and will be skip\n");
				}
			}
			END_DEBUG_BLOCK(Debug);

		}
		{
			uint32_t extensionCount;
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
			std::vector<VkExtensionProperties> availableExtensions(extensionCount);
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());
			auto spExt = availableExtensions
				| std::views::transform([](auto& r) {return std::make_pair(std::string_view{ r.extensionName }, r); })
				| std::ranges::to<std::unordered_map>();
			extension.reserve(needExt.size());
			BEGIN_DEBUG_BLOCK();
			BLOCK_OUTPUT("Instance extensions info:\n");
			for (auto& i : needExt)
			{
				if (spExt.contains(i.name)) {
					extension.push_back(i.name.c_str());
					BLOCK_OUTPUT("\tRequired extensions ");
					BLOCK_OUTPUT(i.name);
					BLOCK_OUTPUT(" support\n");
				}
				else if (i.force) {
					throw nx::NxError(std::format("Required extensions {} is not support", i.name));
				}
				else {
					BLOCK_OUTPUT("\tExtensions ");
					BLOCK_OUTPUT(i.name);
					BLOCK_OUTPUT(" is not support and will be skip\n");
				}

			}
		}

		vk::InstanceCreateInfo insInfo{
			vk::InstanceCreateFlags::BitsType::eEnumeratePortabilityKHR,
			&appInfo,
			layers,
			extension,
			nullptr
		};
		m_instance = vk::raii::Instance(m_context, insInfo, m_allocator);

	}
	void VkDevice::CreateDebugMessenger() {
		auto cb = [](
			vk::DebugUtilsMessageSeverityFlagBitsEXT		messageSeverity,
			vk::DebugUtilsMessageTypeFlagsEXT				messageTypes,
			const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData) ->vk::Bool32 {
				static char buffer[256];
				std::pmr::monotonic_buffer_resource rs(buffer, sizeof(buffer));

				Level logLevel = Level::Info;
				if (messageSeverity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eError) {
					logLevel = Level::Error;
				}
				else if (messageSeverity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning) {
					logLevel = Level::Warn;
				}
				else if (messageSeverity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo) {
					logLevel = Level::Info;
				}
				else if (messageSeverity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose) {
					logLevel = Level::Debug; // 或 Trace，根据需求调整
				}
				std::pmr::string formattedMsg{ &rs };

				if (messageTypes & vk::DebugUtilsMessageTypeFlagsEXT::BitsType::ePerformance) {
					formattedMsg += "[PERF] ";
				}
				if (messageTypes & vk::DebugUtilsMessageTypeFlagsEXT::BitsType::eValidation) {
					formattedMsg += "[VAL] ";
				}
				if (messageTypes & vk::DebugUtilsMessageTypeFlagsEXT::BitsType::eDeviceAddressBinding) {
					formattedMsg += "[DAB] ";
				}
				if (messageTypes & vk::DebugUtilsMessageTypeFlagsEXT::BitsType::eGeneral) {

					formattedMsg += "[GEN] ";
				}

				formattedMsg += pCallbackData->pMessage;
				_logger.Log(logLevel, formattedMsg);
				return VK_FALSE;
			};

		using sb = vk::DebugUtilsMessageSeverityFlagsEXT::BitsType;
		using utb = vk::DebugUtilsMessageTypeFlagsEXT::BitsType;
		vk::DebugUtilsMessengerCreateInfoEXT info{
			{},
			sb::eError | sb::eInfo | sb::eVerbose | sb::eWarning,
			utb::eDeviceAddressBinding | utb::eGeneral | utb::ePerformance | utb::eValidation,
			cb,
			nullptr,
			nullptr
		};
		m_debugMessenger = vk::raii::DebugUtilsMessengerEXT(m_instance, info, m_allocator);
	}
	void VkDevice::PickPhysicalDevice() {
		auto phy = m_instance.enumeratePhysicalDevices();
#ifdef _DEBUG
			std::pmr::string ss;
			ss.reserve(256);
			ss += "Device Info:\n";
			bool select = false;
			for (auto& i : phy)
			{
				auto pro = i.getProperties();
				ss.assign("\t")
					.append(pro.deviceName)
					.append(std::format("{}.{}.{}", vk::versionMajor(pro.apiVersion), vk::versionMinor(pro.apiVersion), vk::versionPatch(pro.apiVersion)))
					;
				if (!select) {
					select = true;
					ss.append("[✔️]");
				}
				ss.append("\n");
			}
			_logger.Info(ss);
#endif
		m_selectedDevice = phy[0];
	}
	void VkDevice::CreateSurface() {
		vk::Win32SurfaceCreateInfoKHR info{};

		m_surface = vk::raii::SurfaceKHR(m_instance, info, m_allocator);
	}

	void VkDevice::FindQueueFamilies() {
		auto qf = m_selectedDevice.getQueueFamilyProperties();
		constexpr auto notFoundNum = std::numeric_limits<uint32_t>::max();

		auto m_directQueueIndex = notFoundNum;
		auto m_computeQueueIndex = notFoundNum;
		auto m_transferQueueIndex = notFoundNum;
		auto m_presentQueueIndex = notFoundNum;
		uint32_t presentIndex = notFoundNum;

		for (uint32_t i = 0; i < qf.size(); i++) {
			auto flags = qf[i].queueFlags;

			if (m_selectedDevice.getSurfaceSupportKHR(i, m_surface)) {
				presentIndex = i;
			}

			if ((flags & vk::QueueFlagBits::eTransfer) &&
				!(flags & vk::QueueFlagBits::eGraphics) &&
				!(flags & vk::QueueFlagBits::eCompute)) {
				m_transferQueueIndex = i;
			}

			if ((flags & vk::QueueFlagBits::eCompute) &&
				!(flags & vk::QueueFlagBits::eGraphics)) {
				m_computeQueueIndex = i;
			}
		}

		for (uint32_t i = 0; i < qf.size(); i++) {
			auto flags = qf[i].queueFlags;

			if ((flags & vk::QueueFlagBits::eGraphics) &&
				(flags & vk::QueueFlagBits::eCompute) &&
				(flags & vk::QueueFlagBits::eTransfer) &&
				m_directQueueIndex == notFoundNum) {
				m_directQueueIndex = i;

				if (m_selectedDevice.getSurfaceSupportKHR(i, m_surface)) {
					presentIndex = i;
				}
			}

			if (m_computeQueueIndex == notFoundNum &&
				(flags & vk::QueueFlagBits::eCompute)) {
				m_computeQueueIndex = i;
			}

			if (m_transferQueueIndex == notFoundNum &&
				(flags & vk::QueueFlagBits::eTransfer)) {
				m_transferQueueIndex = i;
			}
		}

		if (m_directQueueIndex == notFoundNum) {
			for (uint32_t i = 0; i < qf.size(); i++) {
				if (qf[i].queueFlags & vk::QueueFlagBits::eGraphics) {
					m_directQueueIndex = i;
					break;
				}
			}
			if (m_directQueueIndex == notFoundNum) {
				throw NxError("No suitable graphics queue found");
			}
		}

		if (presentIndex == notFoundNum) {
			throw NxError("No present-capable queue found");
		}

		m_presentQueueIndex = m_selectedDevice.getSurfaceSupportKHR(m_directQueueIndex, m_surface)
			? m_directQueueIndex
			: presentIndex;

		if (m_computeQueueIndex == notFoundNum) {
			m_computeQueueIndex = m_directQueueIndex;
		}
		if (m_transferQueueIndex == notFoundNum) {
			m_transferQueueIndex = m_directQueueIndex;
		}

		m_queueFamilyIndices[QueueType::Direct] = m_directQueueIndex;
		m_queueFamilyIndices[QueueType::Compute] = m_computeQueueIndex;
		m_queueFamilyIndices[QueueType::Transfer] = m_transferQueueIndex;
		m_queueFamilyIndices[QueueType::Present] = m_presentQueueIndex;


	};
	void VkDevice::CreateDevice() {
		float priorities[] = { 1 };
		using enum vk::DeviceQueueCreateFlagBits;
		std::array queueInfo{
			vk::DeviceQueueCreateInfo{ eProtected, m_queueFamilyIndices[QueueType::Direct],1 ,priorities},
			vk::DeviceQueueCreateInfo{ eProtected, m_queueFamilyIndices[QueueType::Compute],1 ,priorities},
			vk::DeviceQueueCreateInfo{ eProtected, m_queueFamilyIndices[QueueType::Present],1 ,priorities},
			vk::DeviceQueueCreateInfo{ eProtected, m_queueFamilyIndices[QueueType::Transfer],1 ,priorities}
		};

		std::pmr::vector<const char*> deviceExtensions;
		std::pmr::vector<const char*> deviceLayers;

		auto reqExt = EngineConfig::GetRequiredDeviceExtensions();
		{
			uint32_t count;
			std::pmr::vector<VkExtensionProperties> availExt;

			vkEnumerateDeviceExtensionProperties(*m_selectedDevice, nullptr, &count, nullptr);
			availExt.resize(count);
			vkEnumerateDeviceExtensionProperties(*m_selectedDevice, nullptr, &count, availExt.data());
			for (auto& i : reqExt) {
				if (auto f = std::find_if(availExt.begin(), availExt.end(), [&](auto& e) {
					 return strcmp(i.name.c_str(), e.extensionName) == 0;
						
					}); f == availExt.end()) {
					if (i.force) {
						throw NxError(std::format("Required device extension {} not available", i.name));
					}
				}
				else {
					deviceExtensions.push_back(i.name.c_str());
				}
			}
		}

		auto reqLayers = EngineConfig::GetRequiredDeviceLayers();
		{
			uint32_t count;
			std::pmr::vector<VkLayerProperties> availLayers;
			vkEnumerateDeviceLayerProperties(*m_selectedDevice, &count, nullptr);
			availLayers.resize(count);
			vkEnumerateDeviceLayerProperties(*m_selectedDevice, &count, nullptr);
			//(*m_selectedDevice, nullptr, &count, availLayers.data());
			for (auto& i : reqLayers) {
				if (auto f = std::find_if(availLayers.begin(), availLayers.end(), [&](auto& e) {
						return i.name == e.layerName;
					
					}); f == availLayers.end()) {
					if (i.force) {
						throw NxError(std::format("Required device layer {} not available", i.name));
					}
					else {
						_logger.Warn(std::format("Required device layer {} not available and will be skip", i.name));
					}
				}
				else {
					deviceLayers.push_back(i.name.c_str());
				}
			}
		}


		vk::DeviceCreateInfo info{ {},
			queueInfo,
			deviceLayers,
			deviceExtensions,
			nullptr,nullptr
		};

		m_device = vk::raii::Device(m_selectedDevice, info, m_allocator);

	}
	void VkDevice::CreateQueues() {
		for (int index = 0; auto& i : m_queueFamilyIndices)
		{
			auto queue = m_device.getQueue(i, 0);
			m_queues[index] = VkQueue(std::move(queue), *m_device, (QueueType)index, i);
			index++;
		}
	}


	Queue* VkDevice::GetQueue(QueueType type) {
		return &m_queues[type];
	}

	std::shared_ptr<Shader> VkDevice::CreateShader(std::span<std::byte> data) {
		vk::ShaderModuleCreateInfo createInfo{};
		createInfo.codeSize = data.size();
		createInfo.pCode = reinterpret_cast<const std::uint32_t*>(data.data());
		auto module = m_device.createShaderModule(createInfo,m_allocator);
		return MakeSharedWidthSynchronizedCache<VkShader>(std::move(module));
	}

	void VkDevice::CreateSwapChain() {

	
		m_swapChainSupport.capabilities =  m_selectedDevice.getSurfaceCapabilitiesKHR(m_surface);

		m_swapChainSupport.formats = m_selectedDevice.getSurfaceFormatsKHR(m_surface);
        m_swapChainSupport.presentModes = m_selectedDevice.getSurfacePresentModesKHR(m_surface);

		if (m_swapChainSupport.formats.empty() || m_swapChainSupport.presentModes.empty()) {
			throw NxError("No supported swap chain formats or present modes found!");
		}

		m_surfaceFormat = SelectSurfaceFormat();
		auto presentMode = SelectPresentMode();
		auto extent = SelectSwapExtent();
		auto imageCount = ChooseSwapImageCount();
		vk::SwapchainCreateInfoKHR createInfo{};
		createInfo.setSurface(m_surface)
		.setMinImageCount(imageCount)
		.setImageFormat(m_surfaceFormat.format)
		.setImageColorSpace(m_surfaceFormat.colorSpace)
		.setImageExtent(extent)
		.setImageArrayLayers(1)
		.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)

		.setPreTransform(m_swapChainSupport.capabilities.currentTransform)

		.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)

		.setPresentMode(presentMode)
		.setClipped(vk::True)
		;

		if (m_queueFamilyIndices[QueueType::Direct] == m_queueFamilyIndices[QueueType::Present]) {
			createInfo.setImageSharingMode(vk::SharingMode::eExclusive)
			.setQueueFamilyIndexCount(0)
			.setPQueueFamilyIndices(nullptr);
		}else {
			createInfo.setImageSharingMode(vk::SharingMode::eConcurrent)
			.setQueueFamilyIndices(m_queueFamilyIndices);
		}

		if (*m_swapchain) {
			createInfo.setOldSwapchain(m_swapchain);
		}

		m_swapchain = m_device.createSwapchainKHR(createInfo,m_allocator);
		m_swapchainImages = m_swapchain.getImages();
		_logger.Info("Swapchain created!");
	}

	void VkDevice::CreateImageViews() {
		m_swapchainImages.clear();
		m_swachainImageViews.reserve(m_swapchainImages.size());

		for (auto& i : m_swapchainImages) {
			vk::ComponentMapping mapping{
			};
			vk::ImageSubresourceRange subresourceRange{};
			subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor)
			.setBaseMipLevel(0)
			.setLayerCount(1)
			.setLevelCount(1)
			.setBaseArrayLayer(0);

			vk::ImageViewCreateInfo info{};
			info.setImage(i)
			.setViewType(vk::ImageViewType::e2D)
			.setFormat(m_surfaceFormat.format)
			.setComponents(mapping)
			.setSubresourceRange(subresourceRange);
			m_swachainImageViews.emplace_back(
				m_device.createImageView(info,m_allocator)
			);
		}

	}

	vk::SurfaceFormatKHR VkDevice::SelectSurfaceFormat()
	{
		for (auto& i : m_swapChainSupport.formats)
		{
			if (i.format == vk::Format::eB8G8R8A8Unorm && i.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
				return i;
			}
		}
		return m_swapChainSupport.formats[0];
	}

	vk::PresentModeKHR VkDevice::SelectPresentMode()
	{
		for (const auto& availablePresentMode : m_swapChainSupport.presentModes) {
			if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
				return availablePresentMode;
			}
		}

		return vk::PresentModeKHR::eFifo ;
	}

	vk::Extent2D VkDevice::SelectSwapExtent()
	{
		if (m_swapChainSupport.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return m_swapChainSupport.capabilities.currentExtent;
		}

		RECT windowRect;
		::GetClientRect(m_hWnd, &windowRect);

		uint32_t windowWidth = windowRect.right - windowRect.left;
		uint32_t windowHeight = windowRect.bottom - windowRect.top;
        return vk::Extent2D{
			std::clamp(windowWidth, m_swapChainSupport.capabilities.minImageExtent.width, m_swapChainSupport.capabilities.maxImageExtent.width),
			std::clamp(windowHeight, m_swapChainSupport.capabilities.minImageExtent.height, m_swapChainSupport.capabilities.maxImageExtent.height)
		};
	}

	uint32_t VkDevice::ChooseSwapImageCount()
	{
		auto minImageCount = m_swapChainSupport.capabilities.minImageCount + 1u;
		auto maxImageCount = m_swapChainSupport.capabilities.maxImageCount;

		return std::clamp(2u,minImageCount,maxImageCount);
	}





}