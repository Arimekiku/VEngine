#pragma once

#include "VulkanShader.h"

#include <memory>

namespace VEngine 
{
	struct VulkanPipelineLayout
	{
		std::shared_ptr<VulkanShader> Fragment = nullptr;
		std::shared_ptr<VulkanShader> Vertex = nullptr;
		VkRenderPass RenderPass = nullptr;
		VkExtent2D Extent = { 0, 0 };
	};

	class VulkanPipeline
	{
	public:
		VulkanPipeline(const VulkanPipelineLayout& layout);
		~VulkanPipeline();

		VkPipeline GetPipeline() const { return m_pipeline; }

	private:
		VkPipelineLayout m_layout;
		VkPipeline m_pipeline;
	};
}
