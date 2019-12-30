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
		const std::vector<const char*> s_ValidationLayers = { "VK_LAYER_KHRONOS_validation" };
		const std::vector<const char*> s_DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

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
			for (const char* layerName : s_ValidationLayers)
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
		// it's actually possible that the queue families supporting drawing commands
		// and the ones supporting presentation do not overlap.
		// Therefore we have to take into account that there could be a distinct presentation queue by modifying the QueueFamilyIndices structure:
		struct QueueFamilyIndices
		{
			std::optional<uint32_t> GraphicsFamily; 
			std::optional<uint32_t> PresentFamily;   // used to ensure that a device can present images ot the surface created (queue specific feature)
			_NODISCARD bool IsComplete() const { return GraphicsFamily.has_value() && PresentFamily.has_value(); }
		};

		// 1: Verify that queue family has capability of presenting to chosen window surface
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR& surface)
		{
			QueueFamilyIndices indices;
			// Assign index to queue families that could be found
			auto queueFamilies = GetVulkanData<VkQueueFamilyProperties>(vkGetPhysicalDeviceQueueFamilyProperties, device);

			int i = 0;
			for (const auto& queueFamily : queueFamilies) {
				if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
				{
					indices.GraphicsFamily = i;
				}
				VkBool32 presentSupport = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport); // checks if the queue supports the surface
				if (presentSupport) 
				{
					indices.PresentFamily = i;
				}
				if (indices.IsComplete()) 
				{
					break;
				}
				i++;
			}

			return indices;
		}

		// used in swap chain verifications
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device)
		{
			const auto availableExtensions = GetVulkanData<VkExtensionProperties>(vkEnumerateDeviceExtensionProperties, device, nullptr);
			std::set<std::string> requiredExtensions(s_DeviceExtensions.begin(), s_DeviceExtensions.end());
			for (const auto& extension : availableExtensions) 
			{
				requiredExtensions.erase(extension.extensionName);
			}
			return requiredExtensions.empty();
		}

		// swap chain setup
		
		struct SwapChainSupportDetails
		{
			VkSurfaceCapabilitiesKHR Capabilities; // min/max number of images in sc, min/max width / height of images 
			std::vector<VkSurfaceFormatKHR> Formats; // pixel format, color space .... 
			std::vector<VkPresentModeKHR> PresentModes; // available presentation modes
		};

		// populates a swap chain for some physical device  (physical | logical devices are the core components of the swap chain) 
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR& surface)
		{
			SwapChainSupportDetails details;
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.Capabilities); //  supported capabilities
			details.Formats = GetVulkanData<VkSurfaceFormatKHR>(vkGetPhysicalDeviceSurfaceFormatsKHR,device,surface); // supported surface formats 
			details.PresentModes = GetVulkanData<VkPresentModeKHR>(vkGetPhysicalDeviceSurfacePresentModesKHR, device, surface);
			return details;
		}

		bool IsGraphicsVulkanCompatible(VkPhysicalDevice device, VkSurfaceKHR& surface)
		{
			const QueueFamilyIndices indices = FindQueueFamilies(device, surface);
			const bool extensionsSupported = CheckDeviceExtensionSupport(device);

			bool swapChainAdequate = false;
			if (extensionsSupported) 
			{
				//TODO: Currently only require at least a single supported image format and a single supported presentation mode 
				const SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device, surface);
				swapChainAdequate = !swapChainSupport.Formats.empty() && !swapChainSupport.PresentModes.empty();
			}
			return indices.IsComplete() && extensionsSupported && swapChainAdequate;
		}
	}
}
