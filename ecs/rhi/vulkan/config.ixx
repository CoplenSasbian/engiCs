module;
#include <vector>
#include <string>

export module nx.rhi.vk.config;

export namespace nx {

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