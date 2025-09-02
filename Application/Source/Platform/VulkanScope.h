#pragma once

#define GLFW_INCLUDE_VULKAN
#include <memory>
#include <GLFW/glfw3.h>
#include "VulkanDebugger.h"

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

		static VkInstance GetInstance() { return s_instance; }

	private:
		inline static VkInstance s_instance = nullptr;
		inline static std::unique_ptr<VulkanDebugger> m_debugger = nullptr;
	};
}