#pragma once

#include <vector>
#include <cstring>

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

		//TODO: move this from here into a validation abstraction
		const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
#ifdef VKE_DEBUG
		const bool s_EnableValidationLayers = true;
#else
		const bool s_EnableValidationLayers = false;
#endif

		//todo: abstract these functions with the required extensions & validation layers list....
		bool AreExtensionsAvailable()
		{
			return true;
		}
		inline bool AreValidationLayersAvailable()
		{
			// get the available validation layers
			auto availableLayers = GetVulkanData<VkLayerProperties>(vkEnumerateInstanceLayerProperties);
			for (const char* layerName : validationLayers) 
			{
				bool layerFound = false;
				for (const auto& layerProperties : availableLayers) 
				{
					if (strcmp(layerName, layerProperties.layerName) == 0) {
						layerFound = true;
						break;
					}
				}
				if (!layerFound) 
				{
					return false;
				}
			}
			return true;
		}
	}
}