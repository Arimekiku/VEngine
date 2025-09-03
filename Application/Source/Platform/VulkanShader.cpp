#include "VulkanShader.h"

#include <fstream>
#include <stdexcept>
#include <vector>

#include "VulkanDebugger.h"
#include "VulkanScope.h"
#include "Renderer.h"

namespace VEngine
{
    static std::vector<uint32_t> ReadFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);
		if (!file.is_open())
			throw std::runtime_error("failed to open file!");

		const uint32_t fileSize = file.tellg();
		auto buffer = std::vector<uint32_t>(fileSize / sizeof(uint32_t));

		file.seekg(0);
		file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
		file.close();

		return buffer;
    }

	VulkanShader::VulkanShader(const std::string& filename, VkShaderStageFlagBits type)
	{
		const auto device = Renderer::GetScope().GetVulkanDevice()->GetDevice();

		const auto code = ReadFile(filename);

		auto createInfo = VkShaderModuleCreateInfo();
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = 4 * code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VULKAN_CHECK(vkCreateShaderModule(device, &createInfo, nullptr, &m_module));

		auto vertShaderStageInfo = VkPipelineShaderStageCreateInfo();
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = type;
		vertShaderStageInfo.module = m_module;
		vertShaderStageInfo.pName = "main";

		m_createInfo = vertShaderStageInfo;
	}

	VulkanShader::~VulkanShader()
	{
		if (m_module == nullptr)
			return;

		const auto device = Renderer::GetScope().GetVulkanDevice()->GetDevice();

		vkDestroyShaderModule(device, m_module, nullptr);
		m_module = nullptr;
	}
}
