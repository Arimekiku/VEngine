#include "VulkanSwapChain.h"

#include <algorithm>

#include "Renderer.h"
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

		auto formats = std::vector<VkSurfaceFormatKHR>(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_surface, &formatCount, formats.data());

		uint32_t presentModeCount;
		VULKAN_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_surface, &presentModeCount, nullptr));

		auto presentModes = std::vector<VkPresentModeKHR>(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_surface, &presentModeCount, presentModes.data());

		VkSurfaceFormatKHR selectedFormat = formats[0];
		for (const auto& availableFormat : formats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) 
			{
				selectedFormat = availableFormat;
				break;
			}
		}
		m_format = selectedFormat.format;

		VkPresentModeKHR selectedPresentMode = VK_PRESENT_MODE_FIFO_KHR;
		for (const auto& availablePresentMode : presentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) 
			{
				selectedPresentMode = availablePresentMode;
				break;
			}
		}

		m_extent = m_capabilities.currentExtent;
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

			m_extent = actualExtent;
		}

		// Create Images
		uint32_t imageCount = std::min(m_capabilities.minImageCount + 1, m_capabilities.maxImageCount);

		auto createInfo = VkSwapchainCreateInfoKHR();
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = selectedFormat.format;
		createInfo.imageColorSpace = selectedFormat.colorSpace;
		createInfo.imageExtent = m_extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.preTransform = m_capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = selectedPresentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		VULKAN_CHECK(vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapChain))

		vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, nullptr);
		m_swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, m_swapChainImages.data());

		m_swapChainImageViews.resize(imageCount);
		for (size_t i = 0; i < m_swapChainImages.size(); i++)
		{
			auto viewCreateInfo = VkImageViewCreateInfo();
			viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewCreateInfo.image = m_swapChainImages[i];
			viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewCreateInfo.format = m_format;
			viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewCreateInfo.subresourceRange.baseMipLevel = 0;
			viewCreateInfo.subresourceRange.levelCount = 1;
			viewCreateInfo.subresourceRange.baseArrayLayer = 0;
			viewCreateInfo.subresourceRange.layerCount = 1;

			VULKAN_CHECK(vkCreateImageView(m_device, &viewCreateInfo, nullptr, &m_swapChainImageViews[i]));
		}

		// Create Render Pass
		auto colorAttachmentRef = VkAttachmentReference();
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		auto subPass = VkSubpassDescription();
		subPass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subPass.colorAttachmentCount = 1;
		subPass.pColorAttachments = &colorAttachmentRef;

		auto dependency = VkSubpassDependency();
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		auto colorAttachment = VkAttachmentDescription();
		colorAttachment.format = m_format;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		auto renderPassInfo = VkRenderPassCreateInfo();
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subPass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		VULKAN_CHECK(vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_renderPass));

		// Create Framebuffers
		m_swapChainFramebuffers.resize(imageCount);
		for (size_t i = 0; i < m_swapChainImageViews.size(); i++)
		{
			VkImageView attachments[] = 
			{
				m_swapChainImageViews[i]
			};

			auto framebufferInfo = VkFramebufferCreateInfo();
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = m_renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = m_extent.width;
			framebufferInfo.height = m_extent.height;
			framebufferInfo.layers = 1;

			VULKAN_CHECK(vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &m_swapChainFramebuffers[i]));
		}

		// Create Command Buffer
		const auto graphicsQueueIndex = device->GetPhysicalDevice()->GetQueueFamilyIndices().GraphicsFamily;
		uint32_t presentQueueIndex;

		uint32_t queueCount;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, nullptr);
		for (uint32_t i = 0; i < queueCount; i++)
		{
			VkBool32 presentIndex;
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, m_surface, &presentIndex);

			presentQueueIndex = presentIndex == VK_TRUE ? i : presentQueueIndex;

			if (graphicsQueueIndex == presentQueueIndex)
				break;
		}

		auto poolInfo = VkCommandPoolCreateInfo();
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = presentQueueIndex;

		VULKAN_CHECK(vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_commandPool));

		auto allocInfo = VkCommandBufferAllocateInfo();
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		VULKAN_CHECK(vkAllocateCommandBuffers(m_device, &allocInfo, &m_commandBuffer));

		// Synchronization objects
		auto semaphoreInfo = VkSemaphoreCreateInfo();
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		auto fenceInfo = VkFenceCreateInfo();
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		VULKAN_CHECK(vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphore));
		VULKAN_CHECK(vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphore));
		VULKAN_CHECK(vkCreateFence(m_device, &fenceInfo, nullptr, &m_inFlightFence));
	}

	void VulkanSwapChain::Begin()
	{
		vkWaitForFences(m_device, 1, &m_inFlightFence, VK_TRUE, UINT64_MAX);
		vkResetFences(m_device, 1, &m_inFlightFence);

		vkAcquireNextImageKHR(m_device, m_swapChain, UINT64_MAX, m_imageAvailableSemaphore, VK_NULL_HANDLE, &m_ImageIndex);
		vkResetCommandBuffer(m_commandBuffer, 0);

		auto beginInfo = VkCommandBufferBeginInfo();
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		VULKAN_CHECK(vkBeginCommandBuffer(m_commandBuffer, &beginInfo))

		constexpr VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
		auto renderPassInfo = VkRenderPassBeginInfo();
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_renderPass;
		renderPassInfo.framebuffer = m_swapChainFramebuffers[m_ImageIndex];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_extent;
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(m_commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanSwapChain::Apply(std::shared_ptr<VulkanPipeline> pipeline)
	{
		vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipeline());

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_extent.width);
		viewport.height = static_cast<float>(m_extent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(m_commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = m_extent;
		vkCmdSetScissor(m_commandBuffer, 0, 1, &scissor);

		vkCmdDraw(m_commandBuffer, 3, 1, 0, 0);
	}


	void VulkanSwapChain::End() const
	{
		vkCmdEndRenderPass(m_commandBuffer);

		VULKAN_CHECK(vkEndCommandBuffer(m_commandBuffer))

		const VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphore };
		const VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphore };
		constexpr VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		auto submitInfo = VkSubmitInfo();
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_commandBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		const auto queue = Renderer::GetScope().GetVulkanDevice()->GetGraphicsQueue();
		VULKAN_CHECK(vkQueueSubmit(queue, 1, &submitInfo, m_inFlightFence))

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { m_swapChain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &m_ImageIndex;

		vkQueuePresentKHR(queue, &presentInfo);

		vkDeviceWaitIdle(m_device);
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		const auto instance = VulkanScope::GetVulkanInstance();

		vkDeviceWaitIdle(m_device);
		vkDestroySemaphore(m_device, m_imageAvailableSemaphore, nullptr);
		vkDestroySemaphore(m_device, m_renderFinishedSemaphore, nullptr);
		vkDestroyFence(m_device, m_inFlightFence, nullptr);

		vkDestroyRenderPass(m_device, m_renderPass, nullptr);

		for (size_t i = 0; i < m_swapChainImages.size(); i++)
		{
			vkDestroyImageView(m_device, m_swapChainImageViews[i], nullptr);
			vkDestroyFramebuffer(m_device, m_swapChainFramebuffers[i], nullptr);
		}

		vkDestroySwapchainKHR(m_device, m_swapChain, nullptr);
		vkDestroySurfaceKHR(instance, m_surface, nullptr);
	}

}
