/***************************************************************************
 * Filename		: Shader.h
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
#pragma once

#include <vulkan/vulkan.h>

namespace Vulkan_Engine
{
	namespace Graphics
	{
		class Shader
		{
		public:
			Shader() = delete;
			Shader(const std::string& filename, VkDevice* logicalDevice);
			~Shader();
		public:
			_NODISCARD VkShaderModule GetShaderModule() const { return m_ShaderModule; }
		private:
			void CreateShaderModule();
		private:
			std::vector<char> m_ShaderData;
			VkDevice* m_LogicalDevice;
			VkShaderModule  m_ShaderModule;
		};
	}
}