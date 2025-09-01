#include "Renderer.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace VEngine 
{
	void Renderer::Initialize()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_scope.Initialize();
		m_window = glfwCreateWindow(800, 600, "Vulkan Window", nullptr, nullptr);

		glm::mat4 matrix;
		glm::vec4 vec;
		auto test = matrix * vec;
	}

	void Renderer::Update()
	{
		glfwPollEvents();

		m_isRunning = glfwWindowShouldClose(m_window) == false;
	}

	void Renderer::Shutdown()
	{
		glfwDestroyWindow(m_window);

		glfwTerminate();
	}
}