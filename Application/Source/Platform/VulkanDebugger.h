#pragma once

#include <stdexcept>
#include <string>
#include <vulkan/vulkan_core.h>

namespace VEngine 
{
	class VulkanDebugger
	{
	public:
		VulkanDebugger();
		VulkanDebugger(const VulkanDebugger&) = delete;
		VulkanDebugger(VulkanDebugger&&) = delete;
		~VulkanDebugger();

		void SetupDebugMessenger();
		void DestroyDebugMessenger();

		static void VulkanCheck(const VkResult result)
		{
			if (result == VK_SUCCESS || result == VK_INCOMPLETE)
				return;

			throw std::runtime_error("Vulkan error: " + std::to_string(result));
		}

	private:
		VkDebugUtilsMessengerEXT m_debugMessenger = nullptr;
	};
}

#define VULKAN_CHECK(x)	::VEngine::VulkanDebugger::VulkanCheck(x);