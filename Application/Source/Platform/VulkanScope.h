#pragma once

#include "VulkanDebugger.h"
#include "VulkanDevice.h"

namespace VEngine 
{
	class VulkanScope
	{
	public:
		VulkanScope() = default;
		VulkanScope(const VulkanScope&) = delete;
		VulkanScope(VulkanScope&&) = delete;
		~VulkanScope();

		void Initialize();

		std::shared_ptr<VulkanLogicalDevice>& GetVulkanDevice() { return m_logicalDevice; }

		static VkInstance GetVulkanInstance() { return s_instance; }

	private:
		std::shared_ptr<VulkanPhysicalDevice> m_physicalDevice = nullptr;
		std::shared_ptr<VulkanLogicalDevice> m_logicalDevice = nullptr;

		inline static VkInstance s_instance = nullptr;
		inline static std::unique_ptr<VulkanDebugger> m_debugger = nullptr;
	};
}
