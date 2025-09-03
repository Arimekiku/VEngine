#pragma once

#include <GLFW/glfw3.h>

#include "VulkanPipeline.h"
#include "VulkanScope.h"
#include "VulkanSwapChain.h"

namespace VEngine 
{
	class Renderer 
	{
	public:
		Renderer() = default;
		Renderer(Renderer&) = delete;
		Renderer(Renderer&&) = delete;
		~Renderer() = default;

		void Initialize();
		void Update();
		void Shutdown();

		bool IsRunning() const { return m_isRunning; }

		static VulkanScope& GetScope() { return m_scope; }

	private:
		bool m_isRunning = true;

		inline static VulkanScope m_scope;

		std::shared_ptr<VulkanSwapChain> m_swapChain = nullptr;
		std::shared_ptr<VulkanPipeline> m_testPipeline = nullptr;
		GLFWwindow* m_window = nullptr;
	};
}
