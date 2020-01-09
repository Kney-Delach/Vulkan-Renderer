// Copyright (c) 2020 [Ori Lazar]
// This file is subject to the terms and conditions defined in
// file 'LICENSE', which is part of this source code package.

#pragma once

#include <optional>
#include <vector>
#include <cstring>
#include <cstdint> // uint32_max 

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h> // for GLFWindow

#include "Core/Logger/Log.h"
#include "Core/Core.h"

#include "OptionalExtensions.h" //TODO: Move this from here

namespace Vulkan_Engine
{
	namespace Graphics
	{
		// Variadic template to get data from vulkan where it calls a function to get a count of the data
		// and then calls the function again to get the data itself.
		// auto x = GetVulkanData<R>(func, arg0, arg1, ..., argN);
		template <class R, typename FP, typename... Args>
		std::vector<R> GetVulkanData(FP functionPointer, Args&& ... args)
		{
			uint32_t count = 0;
			functionPointer(std::forward<Args>(args)..., &count, nullptr);
			if (count == 0)
			{
				VK_CORE_DEBUG(
					("[GetVulkanData]: Found no data for type -> {0}.", std::string(typeid(R).name()).c_str()));
			}
			std::vector<R> result(count);
			functionPointer(std::forward<Args>(args)..., &count, result.data());
			return result;
		}

#define VK_RESULT(result) if (result != VK_SUCCESS) { auto str = "Check VK error code " + std::to_string(static_cast<int>(result)) \
+ " in file " + __FILE__ + " at line " + std::to_string(__LINE__); VK_CORE_CRITICAL(str.c_str()); throw std::runtime_error(str); };

#define NOTHROW_VK_RESULT(result) if (result != VK_SUCCESS) { auto str = "Check VK error code " + std::to_string(static_cast<int>(result)) \
+ " in file " + __FILE__ + " at line " + std::to_string(__LINE__); VK_CORE_CRITICAL(str.c_str()); };

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////// Remove the stuff below this ////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//TODO: Template this to handle any suitability 
		//////////////////////////////////////////////////////////////////////
		// it's actually possible that the queue families supporting drawing commands
		// and the ones supporting presentation do not overlap.
		// Therefore we have to take into account that there could be a distinct presentation queue by modifying the QueueFamilyIndices structure:
		//struct QueueFamilyIndices
		//{
		//	std::optional<uint32_t> GraphicsFamily;
		//	std::optional<uint32_t> PresentFamily;
		//	// used to ensure that a device can present images ot the surface created (queue specific feature)
		//	_NODISCARD bool IsComplete() const { return GraphicsFamily.has_value() && PresentFamily.has_value(); }
		//};

		// 1: Verify that queue family has capability of presenting to chosen window surface
		//QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR& surface)
		//{
		//	QueueFamilyIndices indices;
		//	// Assign index to queue families that could be found
		//	auto queueFamilies = GetVulkanData<VkQueueFamilyProperties>(vkGetPhysicalDeviceQueueFamilyProperties,
		//	                                                            device);

		//	int i = 0;
		//	for (const auto& queueFamily : queueFamilies)
		//	{
		//		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		//		{
		//			indices.GraphicsFamily = i;
		//		}
		//		VkBool32 presentSupport = false;
		//		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
		//		// checks if the queue supports the surface
		//		if (presentSupport)
		//		{
		//			indices.PresentFamily = i;
		//		}
		//		if (indices.IsComplete())
		//		{
		//			break;
		//		}
		//		i++;
		//	}

		//	return indices;
		//}


		// used in swap chain verifications
		//const std::vector<const char*> s_DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		//bool CheckDeviceExtensionSupport(VkPhysicalDevice device)
		//{
		//	const auto availableExtensions = GetVulkanData<VkExtensionProperties>(
		//		vkEnumerateDeviceExtensionProperties, device, nullptr);
		//	std::set<std::string> requiredExtensions(s_DeviceExtensions.begin(), s_DeviceExtensions.end());
		//	for (const auto& extension : availableExtensions)
		//	{
		//		requiredExtensions.erase(extension.extensionName);
		//	}
		//	return requiredExtensions.empty();
		//}

		// swap chain setup		
		//struct SwapChainSupportDetails
		//{
		//	VkSurfaceCapabilitiesKHR Capabilities; // min/max number of images in sc, min/max width / height of images 
		//	std::vector<VkSurfaceFormatKHR> Formats; // pixel format, color space .... 
		//	std::vector<VkPresentModeKHR> PresentModes; // available presentation modes
		//};

		//// populates a swap chain for some physical device  (physical | logical devices are the core components of the swap chain) 
		//SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR& surface)
		//{
		//	SwapChainSupportDetails details;
		//	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.Capabilities);
		//	//  supported capabilities
		//	details.Formats = GetVulkanData<VkSurfaceFormatKHR>(vkGetPhysicalDeviceSurfaceFormatsKHR, device, surface);
		//	// supported surface formats 
		//	details.PresentModes = GetVulkanData<VkPresentModeKHR>(vkGetPhysicalDeviceSurfacePresentModesKHR, device,
		//	                                                       surface);
		//	return details;
		//}

		//bool IsGraphicsVulkanCompatible(VkPhysicalDevice device, VkSurfaceKHR& surface)
		//{
		//	const QueueFamilyIndices indices = FindQueueFamilies(device, surface);
		//	const bool extensionsSupported = CheckDeviceExtensionSupport(device);

		//	VkPhysicalDeviceFeatures supportedFeatures;
		//	vkGetPhysicalDeviceFeatures(device, &supportedFeatures); // query for supported hardware features 

		//	bool swapChainAdequate = false;
		//	if (extensionsSupported)
		//	{
		//		//TODO: Currently only require at least a single supported image format and a single supported presentation mode 
		//		const SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device, surface);
		//		swapChainAdequate = !swapChainSupport.Formats.empty() && !swapChainSupport.PresentModes.empty();
		//	}
		//	return indices.IsComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.
		//		samplerAnisotropy;
		//}


		// ------------------------ Functions to create the best possible swap chain ------------------------  //
		// 3 settings
		// 1: surface format (color depth)
		// 2. presentation mode (conditions for swapping images to the screen)
		// 3. swap extent (resolution of images in swap chain)

		//////////////////////////////////////////////////
		//// Surface Format 
		//////////////////////////////////////////////////
		// prefer srgb colorspace: https://stackoverflow.com/questions/12524623/what-are-the-practical-differences-when-working-with-colors-in-a-linear-vs-a-no
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
		{
			for (const auto& availableFormat : availableFormats)
			{
				if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM // standard rgb color format 
					&&
					availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) // srgb color space 
				{
					return availableFormat;
				}
				return availableFormats[0];
			}
		}

		//////////////////////////////////////////////////
		//// Presentation Mode
		//////////////////////////////////////////////////
		// 4 modes
		// 1. Immediate
		// 2. FIFO (Queue)  [GUARANTEED TO BE AVAILABLE]
		// 3. FIFO Relaxed (Relaxed Queue) 
		// 4. MAILBOX (Best->Triple buffering capabilities.... No queue bottleneck)
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
		{
			//TODO: Figure out how to sync with vertical blank (without using FIFO)
			for (const auto& availablePresentMode : availablePresentModes)
			{
				if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					return availablePresentMode;
				}
			}
			return VK_PRESENT_MODE_FIFO_KHR;
		}

		//////////////////////////////////////////////////
		//// Swap Extent
		//////////////////////////////////////////////////
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* activeWindow)
		{
			if (capabilities.currentExtent.width != UINT32_MAX)
			{
				return capabilities.currentExtent;
			}
			else
			{
				int width, height;
				glfwGetFramebufferSize(activeWindow, &width, &height);

				VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
				//TODO: Abstract this process into getting data from the callback . 
				actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width,
				                                capabilities.maxImageExtent.width);
				actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height,
				                                 capabilities.maxImageExtent.height);
				return actualExtent;
			}
		}

		//////////////////////////////////////////////////
		// vertex buffer related functions
		//////////////////////////////////////////////////
		// Memory heaps are distinct memory resources like dedicated VRAM and swap space in RAM for when VRAM runs out.
		// The different types of memory exist within these heaps.
		// Right now we'll only concern ourselves with the type of memory and not the heap it comes from,
		// but you can imagine that this can affect performance.
		// https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkPhysicalDeviceMemoryProperties.html
		// https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkMemoryType.html
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice& physicalDevice)
		{
			VkPhysicalDeviceMemoryProperties memProperties; // memory types & memory heaps arrays
			vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
			// specify bit field of memory types that are suitable (iterate over them, check if bit is set to 1)
			// as well as check for compatible properties 
			for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
			{
				if (typeFilter & 1 << i && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
					// bitwise AND 
				{
					return i;
				}
			}
			throw std::runtime_error("Failed to find suitable memory type!");
		}

		// depth buffer format stuff

		VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling,
		                             VkFormatFeatureFlags features, VkPhysicalDevice& physicalDevice)
		{
			// linearTilingFeatures: Use cases that are supported with linear tiling
			// optimalTilingFeatures : Use cases that are supported with optimal tiling
			// bufferFeatures : Use cases that are supported for buffers
			for (VkFormat format : candidates)
			{
				VkFormatProperties props;
				vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

				if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
				{
					return format;
				}
				else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
				{
					return format;
				}

				throw std::runtime_error("Failed to find supported format!");
			}
		}

		VkFormat FindDepthFormat(VkPhysicalDevice& physicalDevice)
		{
			return FindSupportedFormat(
				{VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
				VK_IMAGE_TILING_OPTIMAL,
				VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, physicalDevice);
		}

		bool HasStencilComponent(VkFormat format)
		{
			return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
		}
	}
}
