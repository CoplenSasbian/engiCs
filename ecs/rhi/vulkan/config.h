#pragma once
#include <vector>
#include <string>
#include <vulkan/vulkan.hpp>


 namespace nx {
    static constexpr auto VulkanVersion = VK_API_VERSION_1_3;

    struct VulkanExtensionLayerInfo {
        int id;
        std::string name;
        bool enable;
        bool dev;
        bool force;
    };



#ifdef _DEBUG
    constexpr  bool IsDebug = false;
#else
    constexpr  bool IsDebug = true;
#endif


    class EngineConfig {

    public:
        static std::pmr::vector<VulkanExtensionLayerInfo> GetRequiredInstanceExtensions(bool ignoreDisabled = false,bool dev=IsDebug);
        static std::pmr::vector<VulkanExtensionLayerInfo> GetRequiredInstanceLayers(bool ignoreDisabled = false, bool dev = IsDebug);

        static std::pmr::vector<VulkanExtensionLayerInfo> GetRequiredDeviceExtensions(bool ignoreDisabled = false,bool dev=IsDebug);
        static std::pmr::vector<VulkanExtensionLayerInfo> GetRequiredDeviceLayers(bool ignoreDisabled = false, bool dev = IsDebug);

    };
}