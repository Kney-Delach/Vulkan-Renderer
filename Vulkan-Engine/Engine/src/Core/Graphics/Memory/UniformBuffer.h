/***************************************************************************
 * Filename		: MVP.h
 * Name			: Ori Lazar
 * Date			: 04/01/2020
 * Description	: Handles a model view projection struct.
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

#include "glm/mat4x2.hpp"

namespace Vulkan_Engine
{
	namespace Graphics
	{
		struct MVP
		{
			alignas(16) glm::mat4 Model;
			alignas(16) glm::mat4 View;
			alignas(16) glm::mat4 Projection;
		public:
			MVP() = default;
			~MVP() = default;
		};
	}
}
