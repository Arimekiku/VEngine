#include "VulkanScope.h"

#include <print>
#include <vector>

#include "GLFW/glfw3.h"

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

		// Setup Vulkan Instance
		uint32_t extCount = 0;
		const char** exts = glfwGetRequiredInstanceExtensions(&extCount);

		auto extensions = std::vector(exts, exts + extCount);
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

		constexpr VkValidationFeatureEnableEXT validationFeatures[] = { VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT };
		auto features = VkValidationFeaturesEXT();
		features.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
		features.enabledValidationFeatureCount = 1;
		features.pEnabledValidationFeatures = validationFeatures;

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

		// Find Validation Layer
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		auto instanceLayerProperties = std::vector<VkLayerProperties>(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, instanceLayerProperties.data());

		const auto validationLayerName = "VK_LAYER_KHRONOS_validation";
		bool validationLayer = false;
		for (const VkLayerProperties& layer : instanceLayerProperties)
		{
			if (strcmp(layer.layerName, validationLayerName) != 0)
				continue;

			validationLayer = true;

			createInfo.ppEnabledLayerNames = &validationLayerName;
			createInfo.enabledLayerCount = 1;
			break;
		}

		// Create Instance & Debugger if possible
		VULKAN_CHECK(vkCreateInstance(&createInfo, nullptr, &s_instance));

		if (validationLayer == false)
		{
			std::println("Validation is disabled");
			return;
		}

		m_debugger = std::make_unique<VulkanDebugger>();
		m_debugger->SetupDebugMessenger();

        m_physicalDevice = std::make_shared<VulkanPhysicalDevice>();
		m_logicalDevice = std::make_shared<VulkanLogicalDevice>(m_physicalDevice);
	}

	VulkanScope::~VulkanScope()
	{
		if (s_instance == nullptr)
			return;

		m_logicalDevice = nullptr;
		m_physicalDevice = nullptr;

		if (m_debugger)
		{
			m_debugger->DestroyDebugMessenger();
			m_debugger = nullptr;
		}

		vkDestroyInstance(s_instance, nullptr);
		s_instance = nullptr;
	}
}
