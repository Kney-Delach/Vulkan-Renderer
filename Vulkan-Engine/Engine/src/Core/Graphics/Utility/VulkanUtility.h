#pragma once

#include <vector>
#include <vulkan/vulkan.h>

#include "Core/Logger/Log.h"

namespace Vulkan_Engine
{
	namespace Graphics
	{
		// Variadic template to get data from vulkan where it calls a function to get a count of the data
		// and then calls the function again to get the data itself.
		// auto x = GetVulkanData<R>(func, arg0, arg1, ..., argN);
		template<class R, typename FP, typename... Args>
		inline std::vector<R> GetVulkanData(FP functionPointer, Args&& ... args)
		{
			uint32_t count = 0;
			functionPointer(std::forward<Args>(args)..., &count, nullptr);
			if (count == 0) 
			{
				VK_CORE_DEBUG(("[GetVulkanData]: Found no data for type -> {0}.", std::string(typeid(R).name())));
			}
			std::vector<R> result(count);
			functionPointer(std::forward<Args>(args)..., &count, result.data());
			return result;
		}
	}
}