// Copyright (c) 2020 [Ori Lazar]
// This file is subject to the terms and conditions defined in
// file 'LICENSE', which is part of this source code package.

#pragma once

#include "VulkanUtility.h"

namespace Vulkan_Engine
{
	namespace Graphics
	{
		// forward declarations
		class LogicalDevice;

		class Validation final
		{
		public:
			// Any warnings of less importance than the warning bit will be ignored
			// Precondition: instance must have been correctly and successfully created, and is not VK_NULL_HANDLE.
			Validation(VkInstance instance, VkDebugUtilsMessageSeverityFlagBitsEXT severityFlag, void* userData);

			~Validation();

		public:
			// This must be called before creating a Validation instance so that the Vulkan instance has validation layers enabled
			static bool EnableValidation(std::vector<const char*>& extensions, uint32_t& enabledLayerCount,
			                             const char* const*& ppEnabledLayerNames);
			static VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilityExtensionCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
				VkDebugUtilsMessageTypeFlagsEXT messageType,
				const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
				void* pUserData);
			static void AddDebugName(const LogicalDevice* logicDevice, VkObjectType type, uint64_t handle, const std::string& name);
		private:
			static bool IsExtensionAvailable();

			static bool IsValidationLayerAvailable();

		private:
			static bool s_ValidationEnabledSuccess;
			static const std::array<const char*, 1> s_Layers;
			static const std::array<const char*, 1> s_Extensions;
			static PFN_vkSetDebugUtilsObjectNameEXT s_DebugNameFunction;
		private:
			VkInstance m_CallbackInstance;
			const VkDebugUtilsMessageSeverityFlagBitsEXT m_SeverityFlag;
			VkDebugUtilsMessengerEXT m_CallbackHandle;
		};
	}
}
