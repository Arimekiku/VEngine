#pragma once

#include <memory>
#include <GLFW/glfw3.h>

#include "VulkanDevice.h"
#include "VulkanPipeline.h"

namespace VEngine 
{
	class VulkanSwapChain
	{
	public:
		VulkanSwapChain(const std::shared_ptr<VulkanLogicalDevice>& device, GLFWwindow* window);
		~VulkanSwapChain();

		VkRenderPass GetRenderPass() { return m_renderPass; }
		VkExtent2D GetExtent() const { return m_extent; }

		void Begin();
		void Apply(std::shared_ptr<VulkanPipeline> pipeline);
		void End() const;

	private:
		uint32_t m_ImageIndex;
		VkFormat m_format;
		VkExtent2D m_extent;
		std::vector<VkImage> m_swapChainImages;
		std::vector<VkImageView> m_swapChainImageViews;
		std::vector<VkFramebuffer> m_swapChainFramebuffers;

		VkDevice m_device;

		VkCommandBuffer m_commandBuffer;
		VkCommandPool m_commandPool;
		VkRenderPass m_renderPass;

		VkSurfaceCapabilitiesKHR m_capabilities;
		VkSwapchainKHR m_swapChain;
		VkSurfaceKHR m_surface;

		VkSemaphore m_imageAvailableSemaphore;
		VkSemaphore m_renderFinishedSemaphore;
		VkFence m_inFlightFence;
	};
}
