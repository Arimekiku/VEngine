#pragma once

#include <string>
#include <vulkan/vulkan_core.h>

namespace VEngine 
{
	class VulkanShader
	{
	public:
		VulkanShader(const std::string& filename, VkShaderStageFlagBits type);
		VulkanShader(const VulkanShader&) = default; // TODO: Implement proper copy
		VulkanShader(VulkanShader&&) = default; // TODO: Implement proper move
		~VulkanShader();

		const VkPipelineShaderStageCreateInfo& GetCreateInfo() const { return m_createInfo; }

	private:
		VkShaderModule m_module;
		VkPipelineShaderStageCreateInfo m_createInfo;
	};
}
