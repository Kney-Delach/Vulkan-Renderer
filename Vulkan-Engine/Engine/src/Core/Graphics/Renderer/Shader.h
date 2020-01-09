// Copyright (c) 2020 [Ori Lazar]
// This file is subject to the terms and conditions defined in
// file 'LICENSE', which is part of this source code package.

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
			VkShaderModule m_ShaderModule;
		};
	}
}
