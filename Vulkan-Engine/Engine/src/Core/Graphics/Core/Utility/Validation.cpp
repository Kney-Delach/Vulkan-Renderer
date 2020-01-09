// Copyright (c) 2020 [Ori Lazar]
// This file is subject to the terms and conditions defined in
// file 'LICENSE', which is part of this source code package.

#include "vkepch.h"

#include "Validation.h"

#include "Core/Graphics/Core/Window/WindowProperties.h"
#include "Core/Graphics/Core/Devices/LogicalDevice.h"

namespace Vulkan_Engine
{
	namespace Graphics
	{
		const std::array<const char*, 1> Validation::s_Layers = {"VK_LAYER_KHRONOS_validation"};
		const std::array<const char*, 1> Validation::s_Extensions = {VK_EXT_DEBUG_UTILS_EXTENSION_NAME,};
		bool Validation::s_ValidationEnabledSuccess = false;
		PFN_vkSetDebugUtilsObjectNameEXT Validation::s_DebugNameFunction = nullptr;

		Validation::Validation(VkInstance instance, VkDebugUtilsMessageSeverityFlagBitsEXT severityFlag, void* userData)
			: m_CallbackInstance(instance), m_SeverityFlag(severityFlag), m_CallbackHandle(nullptr)
		{
			VKE_CORE_ASSERT(instance != VK_NULL_HANDLE, "Validation Constructor instance is a null handle!")
			auto data = static_cast<WindowData*>(userData);
			data->VulkanProperties.Severity = m_SeverityFlag;
			if (s_ValidationEnabledSuccess)
			{
				VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
				createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
				createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
					VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
				createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
					VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
				createInfo.pfnUserCallback = DebugUtilityExtensionCallback;
				createInfo.pUserData = userData;

				const auto createFunction = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
					m_CallbackInstance, "vkCreateDebugUtilsMessengerEXT");
				if (createFunction == nullptr || createFunction(m_CallbackInstance, &createInfo, nullptr,
				                                                &m_CallbackHandle) != VK_SUCCESS)
				{
					VK_CORE_ERROR("[Warning]: Validation callback setup failed.");
				}
				else
				{
					VK_CORE_INFO("[Enabled]: Validation Layer.");
				}
				s_DebugNameFunction = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetInstanceProcAddr(
					m_CallbackInstance, "vkSetDebugUtilsObjectNameEXT");
			}
		}

		Validation::~Validation()
		{
			if (m_CallbackHandle != nullptr)
			{
				const auto destroyFunction = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
					m_CallbackInstance, "vkDestroyDebugUtilsMessengerEXT");
				if (destroyFunction != nullptr)
				{
					destroyFunction(m_CallbackInstance, m_CallbackHandle, nullptr);
				}
			}
		}

		bool Validation::EnableValidation(std::vector<const char*>& extensions, uint32_t& enabledLayerCount,
		                                  const char* const*& ppEnabledLayerNames)
		{
			enabledLayerCount = 0;
			ppEnabledLayerNames = nullptr;
#ifndef VKE_ENABLE_VALIDATION
			return false;
#endif
			IsExtensionAvailable();
			IsValidationLayerAvailable();
			enabledLayerCount = static_cast<uint32_t>(s_Layers.size());
			ppEnabledLayerNames = s_Layers.data();
			for (auto extension : s_Extensions)
			{
				extensions.push_back(extension);
			}
			s_ValidationEnabledSuccess = true;
			return true;
		}

		VkBool32 Validation::DebugUtilityExtensionCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		                                                   VkDebugUtilsMessageTypeFlagsEXT messageType,
		                                                   const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		                                                   void* pUserData)
		{
			WindowData* data = static_cast<WindowData*>(pUserData);
			//TODO: Implement usage with structure severity.
			//if (messageSeverity >= data->VulkanProperties.Severity)
			//{
			std::string message = "[Severity::" + messageSeverity;
			message += "]: ";
			message += pCallbackData->pMessage;
			VK_CORE_ERROR(message);
			return VK_TRUE;
			//}
			//return VK_FALSE;
		}

		void Validation::AddDebugName(const LogicalDevice* logicDevice, VkObjectType type, uint64_t handle,
		                              const std::string& name)
		{
			if (s_ValidationEnabledSuccess && s_DebugNameFunction != nullptr && !name.empty())
			{
				VkDebugUtilsObjectNameInfoEXT nameInfo = {
					VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT, nullptr, type, handle, name.c_str()
				};
				NOTHROW_VK_RESULT(s_DebugNameFunction(*logicDevice, &nameInfo));
			}
		}

		bool Validation::IsExtensionAvailable()
		{
			auto availableExtensions = GetVulkanData<VkExtensionProperties>(
				vkEnumerateInstanceExtensionProperties, nullptr);
			bool found = false;
			for (auto ext : s_Extensions)
			{
				for (const auto& avaExt : availableExtensions)
				{
					if (strcmp(ext, avaExt.extensionName) == 0)
					{
						found = true;
						break;
					}
				}
				if (!found)
				{
					return false;
				}
			}
			return true;
		}

		bool Validation::IsValidationLayerAvailable()
		{
			auto availableLayerProperties = GetVulkanData<VkLayerProperties>(vkEnumerateInstanceLayerProperties);
			for (auto layer : s_Layers)
			{
				bool found = false;
				for (const auto& layerProperties : availableLayerProperties)
				{
					if (strcmp(layer, layerProperties.layerName) == 0)
					{
						found = true;
						break;
					}
				}
				if (!found)
					return false;
			}
			return true;
		}
	}
}
