// Copyright (c) 2020 [Ori Lazar]
// This file is subject to the terms and conditions defined in
// file 'LICENSE', which is part of this source code package.

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
