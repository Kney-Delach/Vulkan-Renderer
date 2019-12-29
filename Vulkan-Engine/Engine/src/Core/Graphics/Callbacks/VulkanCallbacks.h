#pragma once

#include <vulkan/vulkan.h>

namespace Vulkan_Engine
{
	namespace Graphics
	{
		class VulkanCallbacks
		{
		public:
			VulkanCallbacks() = delete;
			~VulkanCallbacks() = delete;
		public:
			static VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilityExtensionCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
																				VkDebugUtilsMessageTypeFlagsEXT messageType,
																				const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
																				void* pUserData);
		};
	}
}
