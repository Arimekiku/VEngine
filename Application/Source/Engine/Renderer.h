#pragma once

#include <GLFW/glfw3.h>

#include "VulkanScope.h"

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

		GLFWwindow* m_window = nullptr;
	};
}
