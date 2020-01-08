/***************************************************************************
 * Filename		: Shader.cpp
 * Name			: Ori Lazar
 * Date			: 30/12/2019
 * Description	: Handles shaders in this engine
     .---.
   .'_:___".
   |__ --==|
   [  ]  :[|
   |__| I=[|
   / / ____|
  |-/.____.'
 /___\ /___\
***************************************************************************/
#include "vkepch.h"

#include "Shader.h"

#include "Core/Logger/Log.h"

namespace Vulkan_Engine
{
	namespace Graphics
	{
		Shader::Shader(const std::string& filename, VkDevice* logicalDevice)
			: m_LogicalDevice(logicalDevice)
		{
			// move away from constructor 
			std::ifstream file(filename, std::ios::ate | std::ios::binary);
			if (!file.is_open()) 
			{
				static const std::string message = "[GraphicsSystem::Shader::Constructor]: Failed to open shader file!";
				VK_CORE_CRITICAL("{0} -> In file:  {1} ", message, filename);
				throw std::runtime_error(message);
			}
			const size_t fileSize = (size_t)file.tellg();
			m_ShaderData = std::vector<char>(fileSize);
			file.seekg(0);
			file.read(m_ShaderData.data(), fileSize);
			file.close();

			// move to init
			CreateShaderModule();
		}

		Shader::~Shader()
		{
			vkDestroyShaderModule(*m_LogicalDevice, m_ShaderModule, nullptr);
		}	

		void Shader::CreateShaderModule()
		{
			VkShaderModuleCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.codeSize = m_ShaderData.size();
			// note, ensure that data matches alignment 
			createInfo.pCode = reinterpret_cast<const uint32_t*>(m_ShaderData.data()); // reinterpret due to bytecode usage vs char usage
			if (vkCreateShaderModule(*m_LogicalDevice, &createInfo, nullptr, &m_ShaderModule) != VK_SUCCESS) 
			{
				static const std::string message = "[GraphicsSystem::Shader::Constructor]: Failed to create shader module!";
				VK_CORE_CRITICAL(message);
				throw std::runtime_error(message);
			}
		}
	}
}