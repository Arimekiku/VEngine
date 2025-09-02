#include "VulkanDevice.h"
#include "VulkanScope.h"

#include <print>

namespace VEngine
{
	VulkanPhysicalDevice::VulkanPhysicalDevice()
	{
		const auto instance = VulkanScope::GetInstance();

		// Find Discrete GPU
		uint32_t gpuCount = 0;
		VULKAN_CHECK(vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr));

		if (gpuCount == 0)
			throw std::runtime_error("Failed to find GPUs with Vulkan support!");

		auto physicalDevices = std::vector<VkPhysicalDevice>();
		physicalDevices.resize(gpuCount);
		VULKAN_CHECK(vkEnumeratePhysicalDevices(instance, &gpuCount, physicalDevices.data()));

		auto selectedPhysicalDevice = physicalDevices.back();
		for (const auto physicalDevice : physicalDevices)
		{
			vkGetPhysicalDeviceProperties(physicalDevice, &m_deviceProperties);

			if (m_deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			{
				selectedPhysicalDevice = physicalDevice;
				break;
			}
		}
		m_physicalDevice = selectedPhysicalDevice;

		// Get Properties and Features
		vkGetPhysicalDeviceFeatures(m_physicalDevice, &m_deviceFeatures);
		vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &m_deviceMemoryProperties);

		uint32_t extCount = 0;
		auto extensions = std::vector<VkExtensionProperties>();

		VULKAN_CHECK(vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &extCount, nullptr));
		if (extCount > 0)
		{
			std::println("Selected physical device has {} extensions.\n", extensions.size());
			for (const auto& [extensionName, _] : extensions)
			{
				m_supportedExtensions.emplace(extensionName);
				std::println("{}", extensionName);
			}
		}

		// Setup Queue Families
		uint32_t queueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, nullptr);

		m_queueFamilyProperties.resize(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, m_queueFamilyProperties.data());

		static constexpr float defaultQueuePriority = 1.0f;
		m_queueFamilyIndices = FindQueueFamilyIndices();

		// Graphics Family
		auto queueInfo = VkDeviceQueueCreateInfo();
		queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueInfo.queueFamilyIndex = m_queueFamilyIndices.GraphicsFamily.value();
		queueInfo.queueCount = 1;
		queueInfo.pQueuePriorities = &defaultQueuePriority;
		m_queueCreateInfos.push_back(queueInfo);
	}

	QueueFamilyIndices VulkanPhysicalDevice::FindQueueFamilyIndices() const
	{
		QueueFamilyIndices indices;

		for (size_t i = 0; i < m_queueFamilyProperties.size(); i++)
		{
			if (m_queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.GraphicsFamily = (int32_t)i;
				break;
			}
		}

		return indices;
	}

	VulkanPhysicalDevice::~VulkanPhysicalDevice()
	{
		
	}

	VulkanLogicalDevice::VulkanLogicalDevice(const std::shared_ptr<VulkanPhysicalDevice>& physicalDevice)
	{
		m_physicalDevice = physicalDevice;
		const auto qInfos = m_physicalDevice->GetQueueFamilyInfos();

		auto createInfo = VkDeviceCreateInfo();
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = qInfos.data();
		createInfo.queueCreateInfoCount = (uint32_t)qInfos.size();
		createInfo.pEnabledFeatures = &physicalDevice->GetFeatures();

		VULKAN_CHECK(vkCreateDevice(physicalDevice->GetPhysicalDevice(), &createInfo, nullptr, &m_logicalDevice));

		const auto graphicsFamilyIndex = physicalDevice->GetQueueFamilyIndices().GraphicsFamily;
		if (graphicsFamilyIndex.has_value() == false)
			throw std::runtime_error("There's available graphics family queue on GPU!");

		vkGetDeviceQueue(m_logicalDevice, graphicsFamilyIndex.value(), 0, &m_graphicsQueue);
	}

	VulkanLogicalDevice::~VulkanLogicalDevice()
	{
		vkDestroyDevice(m_logicalDevice, nullptr);
		m_logicalDevice = nullptr;
	}
}
