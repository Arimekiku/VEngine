#include "VulkanSwapChain.h"

#include <algorithm>

#include "VulkanScope.h"

namespace VEngine
{
	VulkanSwapChain::VulkanSwapChain(const std::shared_ptr<VulkanLogicalDevice>& device, GLFWwindow* window)
	{
		const auto instance = VulkanScope::GetVulkanInstance();
		const auto physicalDevice = device->GetPhysicalDevice()->GetDevice();
		m_device = device->GetDevice();

		// Setup surface
		VULKAN_CHECK(glfwCreateWindowSurface(instance, window, nullptr, &m_surface));

		// Setup details
		VULKAN_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_surface, &m_capabilities));

		uint32_t formatCount;
		VULKAN_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_surface, &formatCount, nullptr));

		if (formatCount != 0) 
		{
			m_formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_surface, &formatCount, m_formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_surface, &presentModeCount, nullptr);

		if (presentModeCount != 0) 
		{
			m_presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_surface, &presentModeCount, m_presentModes.data());
		}

		VkSurfaceFormatKHR selectedFormat = m_formats[0];
		for (const auto& availableFormat : m_formats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) 
			{
				selectedFormat = availableFormat;
				break;
			}
		}

		VkPresentModeKHR selectedPresentMode = VK_PRESENT_MODE_FIFO_KHR;
		for (const auto& availablePresentMode : m_presentModes) 
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) 
			{
				selectedPresentMode = availablePresentMode;
				break;
			}
		}

		VkExtent2D selectedExtent = m_capabilities.currentExtent;
		if (m_capabilities.currentExtent.width == std::numeric_limits<uint32_t>::max()) 
		{
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);

			VkExtent2D actualExtent = 
			{
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actualExtent.width = std::clamp(actualExtent.width, m_capabilities.minImageExtent.width, m_capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, m_capabilities.minImageExtent.height, m_capabilities.maxImageExtent.height);

			selectedExtent = actualExtent;
		}

		uint32_t imageCount = std::min(m_capabilities.minImageCount + 1, m_capabilities.maxImageCount);

		auto createInfo = VkSwapchainCreateInfoKHR();
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = selectedFormat.format;
		createInfo.imageColorSpace = selectedFormat.colorSpace;
		createInfo.imageExtent = selectedExtent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.preTransform = m_capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = selectedPresentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		VULKAN_CHECK(vkCreateSwapchainKHR(device->GetDevice(), &createInfo, nullptr, &m_swapChain))
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		const auto instance = VulkanScope::GetVulkanInstance();

		vkDeviceWaitIdle(m_device);

		vkDestroySwapchainKHR(m_device, m_swapChain, nullptr);
		vkDestroySurfaceKHR(instance, m_surface, nullptr);
	}

}
