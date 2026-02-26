module;
#include <windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_win32.h>
#include <array>
#include <memory>


export module nx.rhi.vk.device;
export import nx.rhi.device;
export import nx.rhi.vk.queue;
export import nx.rhi.vk.shader;
export
namespace nx {

	struct SwapChainSupportDetails {
		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> presentModes;
	};

	class VkDevice :public Device {
	public:

		VkDevice(void* WINDOW_HANDLE);
		~VkDevice() override;
		VkDevice(const VkDevice&) = delete;
		VkDevice(VkDevice&&) = delete;
		void Initialize() override;
		Queue* GetQueue(QueueType type) override;
		std::shared_ptr<Shader> CreateShader(std::span<std::byte> data) override;


private:
		void Shutdown();
		void CreateInstance();
		void CreateDebugMessenger();
		void PickPhysicalDevice();
		void CreateSurface();
		void FindQueueFamilies();
		void CreateDevice();
		void CreateQueues();
		void CreateSwapChain();
		void CreateImageViews();

		vk::SurfaceFormatKHR SelectSurfaceFormat();
        vk::PresentModeKHR SelectPresentMode();
		vk::Extent2D SelectSwapExtent();
		uint32_t ChooseSwapImageCount();


		vk::AllocationCallbacks m_allocator;
		vk::raii::Context m_context{};
		vk::raii::Instance m_instance = nullptr;
		vk::raii::DebugUtilsMessengerEXT m_debugMessenger = nullptr;
		vk::raii::PhysicalDevice m_selectedDevice = nullptr;
		vk::raii::SurfaceKHR m_surface = nullptr;
		vk::raii::Device m_device = nullptr;
		vk::raii::SwapchainKHR m_swapchain = nullptr;
		vk::SurfaceFormatKHR m_surfaceFormat;
		std::vector<vk::Image> m_swapchainImages;
		std::vector<vk::raii::ImageView> m_swachainImageViews;


		HWND m_hWnd = nullptr;

		std::array<uint32_t, 4> m_queueFamilyIndices;
		std::array<nx::VkQueue, 4> m_queues;

		SwapChainSupportDetails m_swapChainSupport;
	};
	
}