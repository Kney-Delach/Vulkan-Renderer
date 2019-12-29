#pragma once

#include <optional>
#include <vector>
#include <cstring>

#include <vulkan/vulkan.h>

#include "Core/Logger/Log.h"

#include "Core/Graphics/Callbacks/VulkanCallbacks.h"

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
				VK_CORE_DEBUG(("[GetVulkanData]: Found no data for type -> {0}.", std::string(typeid(R).name()).c_str()));
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

		//TODO: Move this to proxy function handler
		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
		{
			// will return nullptr if function can't be loaded
			auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
			if (func != nullptr) 
			{
				return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
			}
			else {
				return VK_ERROR_EXTENSION_NOT_PRESENT;
			}
		}

		void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
		{
			auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
			if (func != nullptr) 
			{
				func(instance, debugMessenger, pAllocator);
			}
		}

		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo, void* userData)
		{
			createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			createInfo.pfnUserCallback = VulkanCallbacks::DebugUtilityExtensionCallback;
			createInfo.pUserData = userData; 
		}

		//TODO: Template this to handle any suitability 
		//////////////////////////////////////////////////////////////////////
		///
		struct QueueFamilyIndices
		{
			std::optional<uint32_t> GraphicsFamily;
			bool IsComplete() const { return GraphicsFamily.has_value(); }
		};
		
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device)
		{
			QueueFamilyIndices indices;
			// Assign index to queue families that could be found
			auto queueFamilies = GetVulkanData<VkQueueFamilyProperties>(vkGetPhysicalDeviceQueueFamilyProperties, device);

			int i = 0;
			for (const auto& queueFamily : queueFamilies) 
			{
				if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
				{
					indices.GraphicsFamily = i;
				}

				if (indices.IsComplete()) 
				{
					break;
				}
				i++;
			}
			return indices;
		}

		bool IsGraphicsVulkanCompatible(VkPhysicalDevice device)
		{
			//VkPhysicalDeviceProperties deviceProperties;
			//vkGetPhysicalDeviceProperties(device, &deviceProperties);
			//VkPhysicalDeviceFeatures deviceFeatures; // use this to query additional feature support (64b-float, compressions, viewports)
			//vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
			//return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;

			const QueueFamilyIndices indices = FindQueueFamilies(device);
			return indices.IsComplete();
		}

		//////////////////////////////////////////////////////////////////////
		//TODO: Implement something like below, for choosing optimal card
		//void pickPhysicalDevice() {
		//	...

		//		// Use an ordered map to automatically sort candidates by increasing score
		//		std::multimap<int, VkPhysicalDevice> candidates;

		//	for (const auto& device : devices) {
		//		int score = rateDeviceSuitability(device);
		//		candidates.insert(std::make_pair(score, device));
		//	}

		//	// Check if the best candidate is suitable at all
		//	if (candidates.rbegin()->first > 0) {
		//		physicalDevice = candidates.rbegin()->second;
		//	}
		//	else {
		//		throw std::runtime_error("failed to find a suitable GPU!");
		//	}
		//}

		//int rateDeviceSuitability(VkPhysicalDevice device) {
		//	...

		//		int score = 0;

		//	// Discrete GPUs have a significant performance advantage
		//	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
		//		score += 1000;
		//	}

		//	// Maximum possible size of textures affects graphics quality
		//	score += deviceProperties.limits.maxImageDimension2D;

		//	// Application can't function without geometry shaders
		//	if (!deviceFeatures.geometryShader) {
		//		return 0;
		//	}

		//	return score;
		//}
		////////////////////////////////////////////////////////////////////////
	}
}
