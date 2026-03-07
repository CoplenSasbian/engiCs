#include "config.h"
#include <sqlite_modern_cpp.h>
#include <format>

sqlite::database& Database()
{
	static sqlite::database db("data/engine.db");
	return db;
}

std::pmr::vector<nx::VulkanExtensionLayerInfo> helper(bool ignoreDisable, bool dev, std::string_view table) {
	std::pmr::vector<nx::VulkanExtensionLayerInfo> extensions{};

	Database() << std::format(
		"SELECT id, name, enable, dev, force FROM {} WHERE 1=1{};",
		table,
		ignoreDisable ? "" : " AND enable = 1",
		dev ? "" : " AND dev = 0"
	) >> [&](int id, std::string name, int enable, int isdev, int force) {
		extensions.emplace_back(id, std::move(name), enable != 0, isdev != 0, force != 0);
		};
	return extensions;
}

std::pmr::vector<nx::VulkanExtensionLayerInfo> nx::EngineConfig::GetRequiredInstanceExtensions(bool ignoreDisable,bool dev)
{

	return helper(ignoreDisable,dev,"vulkan_extensions");
}

std::pmr::vector<nx::VulkanExtensionLayerInfo> nx::EngineConfig::GetRequiredInstanceLayers(bool ignoreDisable, bool dev)
{
	return  helper(ignoreDisable, dev, "vulkan_layers");;
}

std::pmr::vector<nx::VulkanExtensionLayerInfo> nx::EngineConfig::GetRequiredDeviceExtensions(bool ignoreDisabled, bool dev)
{

    return helper(ignoreDisabled, dev, "vulkan_device_extensions");
}

std::pmr::vector<nx::VulkanExtensionLayerInfo> nx::EngineConfig::GetRequiredDeviceLayers(bool ignoreDisabled, bool dev)
{
    return helper(ignoreDisabled, dev, "vulkan_device_layers");
}


