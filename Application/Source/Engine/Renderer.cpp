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
		m_swapChain = std::make_shared<VulkanSwapChain>(m_scope.GetVulkanDevice(), m_window);

		auto vertShader = std::make_shared<VulkanShader>("Resources/Shaders/triangle.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
		auto fragShader = std::make_shared<VulkanShader>("Resources/Shaders/triangle.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

		VulkanPipelineLayout layout = 
		{
			fragShader,
			vertShader,
			m_swapChain->GetRenderPass(),
			m_swapChain->GetExtent()
		};

		m_testPipeline = std::make_shared<VulkanPipeline>(layout);

		glm::mat4 matrix;
		glm::vec4 vec;
		auto test = matrix * vec;
	}

	void Renderer::Update()
	{
		m_swapChain->Begin();

		m_swapChain->Apply(m_testPipeline);

		m_swapChain->End();

		glfwPollEvents();
		m_isRunning = glfwWindowShouldClose(m_window) == false;
	}

	void Renderer::Shutdown()
	{
		m_swapChain = nullptr;

		glfwDestroyWindow(m_window);

		glfwTerminate();
	}
}