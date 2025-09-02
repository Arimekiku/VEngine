#pragma once

#include <memory>
#include <vulkan/vulkan_core.h>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

namespace VEngine 
{
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> GraphicsFamily;
	};

	class VulkanPhysicalDevice
	{
	public:
		VulkanPhysicalDevice();
		~VulkanPhysicalDevice();

		const VkPhysicalDevice& GetPhysicalDevice() const { return m_physicalDevice; }
		const VkPhysicalDeviceFeatures& GetFeatures() const { return m_deviceFeatures; }

		QueueFamilyIndices& GetQueueFamilyIndices() { return m_queueFamilyIndices; }

		const std::vector<VkDeviceQueueCreateInfo>& GetQueueFamilyInfos() const { return m_queueCreateInfos; }

	private:
		QueueFamilyIndices FindQueueFamilyIndices() const;
		QueueFamilyIndices m_queueFamilyIndices;

		VkPhysicalDevice m_physicalDevice = nullptr;
		VkPhysicalDeviceProperties m_deviceProperties;
		VkPhysicalDeviceFeatures m_deviceFeatures;
		VkPhysicalDeviceMemoryProperties m_deviceMemoryProperties;

		std::unordered_set<std::string> m_supportedExtensions;
		std::vector<VkDeviceQueueCreateInfo> m_queueCreateInfos;
		std::vector<VkQueueFamilyProperties> m_queueFamilyProperties;
	};

	class VulkanLogicalDevice
	{
	public:
		VulkanLogicalDevice(const std::shared_ptr<VulkanPhysicalDevice>& physicalDevice);
		~VulkanLogicalDevice();

	private:
		VkDevice m_logicalDevice = nullptr;

		VkQueue m_graphicsQueue = nullptr;

		std::shared_ptr<VulkanPhysicalDevice> m_physicalDevice = nullptr;
	};
}
