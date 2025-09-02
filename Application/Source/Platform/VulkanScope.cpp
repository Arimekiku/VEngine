#include "VulkanScope.h"

#include <print>
#include <stdexcept>
#include <vector>

namespace VEngine
{
	void VulkanScope::Initialize()
	{
		if (s_instance != nullptr)
			return;

		// Setup Application Info
		auto appInfo = VkApplicationInfo();
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "VEngine";
		appInfo.pEngineName = "VEngine";
		appInfo.apiVersion = VK_API_VERSION_1_2;

		// Setup Validation Info
		const std::vector extensions = 
		{
			VK_KHR_SURFACE_EXTENSION_NAME, VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
			VK_EXT_DEBUG_REPORT_EXTENSION_NAME, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
		};

		constexpr VkValidationFeatureEnableEXT enables[] = { VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT };
		auto features = VkValidationFeaturesEXT();
		features.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
		features.enabledValidationFeatureCount = 1;
		features.pEnabledValidationFeatures = enables;

		auto createInfo = VkInstanceCreateInfo();
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pNext = &features;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = (uint32_t)extensions.size();
		createInfo.ppEnabledExtensionNames = extensions.data();
		createInfo.enabledLayerCount = 0;

		uint32_t vulkanExtensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &vulkanExtensionCount, nullptr);
		std::println("Enabled extensions: {}, supported extensions: {}\n", extensions.size(), vulkanExtensionCount);

		const auto validationLayerName = "VK_LAYER_KHRONOS_validation";
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		std::vector<VkLayerProperties> instanceLayerProperties(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, instanceLayerProperties.data());
		bool validationLayerPresent = false;

		std::println("Vulkan Instance Layers:");
		for (const VkLayerProperties& layer : instanceLayerProperties)
		{
			std::println("{}", layer.layerName);
			if (strcmp(layer.layerName, validationLayerName) == 0)
			{
				validationLayerPresent = true;
				break;
			}
		}
		std::println();

		if (validationLayerPresent)
		{
			createInfo.ppEnabledLayerNames = &validationLayerName;
			createInfo.enabledLayerCount = 1;
		}
		else
		{
			std::println("Validation is disabled");
		}

		// Create Instance
		VULKAN_CHECK(vkCreateInstance(&createInfo, nullptr, &s_instance));

		if (validationLayerPresent == false)
			return;

		m_debugger = std::make_unique<VulkanDebugger>();
		m_debugger->SetupDebugMessenger();
	}

	VulkanScope::~VulkanScope()
	{
		if (s_instance == nullptr)
			return;

		if (m_debugger)
		{
			m_debugger->DestroyDebugMessenger();
			m_debugger = nullptr;
		}

		vkDestroyInstance(s_instance, nullptr);
		s_instance = nullptr;
	}
}
