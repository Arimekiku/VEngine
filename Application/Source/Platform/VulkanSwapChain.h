#pragma once

#include <memory>
#include <GLFW/glfw3.h>

#include "VulkanDevice.h"

namespace VEngine 
{
	class VulkanSwapChain
	{
	public:
		VulkanSwapChain(const std::shared_ptr<VulkanLogicalDevice>& device, GLFWwindow* window);
		~VulkanSwapChain();

	private:
		std::vector<VkSurfaceFormatKHR> m_formats;
		std::vector<VkPresentModeKHR> m_presentModes;

		VkDevice m_device;

		VkSurfaceCapabilitiesKHR m_capabilities;
		VkSwapchainKHR m_swapChain;
		VkSurfaceKHR m_surface;
	};
}
