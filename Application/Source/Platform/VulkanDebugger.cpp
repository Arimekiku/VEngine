#include "VulkanDebugger.h"
#include "VulkanScope.h"

#include <print>
#include <vulkan/vulkan_core.h>

namespace VEngine
{
	static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
	{
		std::println("Validation: {}", pCallbackData->pMessage);

		return VK_FALSE;
	}

	VulkanDebugger::VulkanDebugger()
	{

	}

	void VulkanDebugger::SetupDebugMessenger()
	{
		const auto instance = VulkanScope::GetVulkanInstance();

		VkDebugUtilsMessengerCreateInfoEXT debugUtilsCreateInfo{};
		debugUtilsCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugUtilsCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugUtilsCreateInfo.pfnUserCallback = VulkanCallback;
		debugUtilsCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

		const auto vkCreateDebugUtilsMessengerExt = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		VULKAN_CHECK(vkCreateDebugUtilsMessengerExt(instance, &debugUtilsCreateInfo, nullptr, &m_debugMessenger));
	}

	void VulkanDebugger::DestroyDebugMessenger()
	{
		const auto instance = VulkanScope::GetVulkanInstance();

		auto vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		vkDestroyDebugUtilsMessengerEXT(instance, m_debugMessenger, nullptr);
		m_debugMessenger = nullptr;
	}

	VulkanDebugger::~VulkanDebugger()
	{
		
	}
}
