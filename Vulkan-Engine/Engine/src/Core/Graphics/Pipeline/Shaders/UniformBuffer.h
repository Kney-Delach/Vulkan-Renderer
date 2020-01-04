/***************************************************************************
 * Filename		: UniformBuffer.h
 * Name			: Ori Lazar
 * Date			: 04/01/2020
 * Description	: Handles uniform buffers.
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
#include "glm/mat4x2.hpp"

namespace Vulkan_Engine
{
	namespace Graphics
	{
		struct UniformBuffer
		{
			alignas(16) glm::mat4 Model;
			alignas(16) glm::mat4 View;
			alignas(16) glm::mat4 Projection;
		public:
			UniformBuffer() = default;
			~UniformBuffer() = default;
		};
	}
}
