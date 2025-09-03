#pragma once

#include <GLFW/glfw3.h>

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

	private:
		bool m_isRunning = true;

		VulkanScope m_scope;

		std::shared_ptr<VulkanSwapChain> m_swapChain = nullptr;
		GLFWwindow* m_window = nullptr;
	};
}
