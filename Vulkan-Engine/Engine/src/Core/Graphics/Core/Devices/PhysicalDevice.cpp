#include "vkepch.h"

#include "PhysicalDevice.h"
#include "LogicalDevice.h"
#include "../Window/Window.h"

//TODO: Implement MSAA Support

namespace Vulkan_Engine
{
	namespace Graphics
	{
		PhysicalDevice::PhysicalDevice(VkPhysicalDevice device, VkSurfaceKHR surface)
		{
			m_QueueFamilyIndices.resize(static_cast<unsigned int>(QueueFamilyType::ENUM_SIZE));
			m_QueueHandles.resize(static_cast<unsigned int>(QueueFamilyType::ENUM_SIZE));
			for (auto& index : m_QueueFamilyIndices)
			{
				index = m_InvalidIndex;
			}
			VKE_CORE_ASSERT(FindQueueFamilies(device, surface), "No compatible queue families were found!");
			m_Extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		}
		
		bool PhysicalDevice::IsCompatible(DeviceSurfaceCapabilities& surfaceCapabilities, VkSurfaceKHR& surface)
		{
			vkGetPhysicalDeviceProperties(m_Device, &m_Properties);
			vkGetPhysicalDeviceFeatures(m_Device, &m_Features);
			return
				m_Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
				&& HasRequiredExtensions(surfaceCapabilities, surface)
				&& HasRequiredQueueFamilies()
				&& m_Features.samplerAnisotropy;
		}

		bool PhysicalDevice::FindQueueFamilies(VkPhysicalDevice& device, VkSurfaceKHR& surface)
		{
			VKE_CORE_ASSERT(device != VK_NULL_HANDLE && surface != VK_NULL_HANDLE, "Cannot search for queue families without device or surface instances!");
			auto queueFamilies = GetVulkanData<VkQueueFamilyProperties>(vkGetPhysicalDeviceQueueFamilyProperties, device);
			int i = 0;
			for (const auto& queueFamily : queueFamilies)
			{
				if (queueFamily.queueCount > 0 && (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)) 
				{
					m_QueueFamilyIndices[static_cast<size_t>(QueueFamilyType::GRAPHICS)] = i;
				}
				if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) 
				{
					m_QueueFamilyIndices[static_cast<size_t>(QueueFamilyType::COMPUTE)] = i;
				}
				VkBool32 presentSupport = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
				if (queueFamily.queueCount > 0 && presentSupport)
				{
					m_QueueFamilyIndices[static_cast<size_t>(QueueFamilyType::PRESENTATION)] = i;
				}
				if (HasRequiredQueueFamilies())
				{
					return true;
				}
				else
				{
					i++;
				}
			}
			return false; // unable to find compatible data
		}

		bool PhysicalDevice::HasRequiredQueueFamilies()
		{
			for (const auto& index : m_QueueFamilyIndices) 
			{
				if (index == m_InvalidIndex)
				{
					return false;
				}
			}
			return true;
		}

		bool PhysicalDevice::HasRequiredExtensions(DeviceSurfaceCapabilities& surfaceCapabilities, VkSurfaceKHR& surface)
		{
			const auto availableExtensions = GetVulkanData<VkExtensionProperties>(vkEnumerateDeviceExtensionProperties, m_Device, nullptr);
			bool hasSwapChainExt = false;
			for (auto& extension : availableExtensions)
			{
				// descriptor indexing extension
				if (!strcmp(extension.extensionName, VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME)) 
				{
					m_Extensions.push_back(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);
					m_OptionalExtensionsEnabled[OptionalExtensions::DescriptorIndexing] = true;
					VK_CORE_INFO("[Enabled Extension]: Descriptor indexing.");
				}
				// Required
				if (!strcmp(extension.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME)) 
				{
					hasSwapChainExt = true;
				}
			}
			if (hasSwapChainExt)
			{
				vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_Device, surface, &surfaceCapabilities.Capabilities);
				surfaceCapabilities.Formats = GetVulkanData<VkSurfaceFormatKHR>(vkGetPhysicalDeviceSurfaceFormatsKHR, m_Device, surface);
				surfaceCapabilities.PresentModes = GetVulkanData<VkPresentModeKHR>(vkGetPhysicalDeviceSurfacePresentModesKHR, m_Device, surface);
				return !surfaceCapabilities.Formats.empty() && !surfaceCapabilities.PresentModes.empty();
			}
			return false;
		}

		LogicalDevice* PhysicalDevice::CreateLogicalDevice(const WindowGraphicsDetails& graphicsDetails,
			DeviceSurfaceCapabilities& surfaceCapabilities, uint32_t enabledLayerCount,
			const char* const* enabledLayerNames)
		{
			const float queuePriority = 1.0f;
			auto createInfos = GetQueueCreateInfos(&queuePriority);
			
			VkPhysicalDeviceFeatures deviceFeatures = {}; //TODO: Implement a better way to do this
			deviceFeatures.samplerAnisotropy = VK_TRUE;
			
			VkDeviceCreateInfo deviceCreateInfo = {};
			deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			deviceCreateInfo.pQueueCreateInfos = createInfos.data();
			deviceCreateInfo.queueCreateInfoCount = createInfos.size();
			deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
			deviceCreateInfo.enabledLayerCount = enabledLayerCount;
			deviceCreateInfo.ppEnabledLayerNames = enabledLayerNames; //TODO: Check if to include validation layers here.
			deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_Extensions.size());
			deviceCreateInfo.ppEnabledExtensionNames = m_Extensions.data();
			deviceCreateInfo.pNext = nullptr;
			deviceCreateInfo.flags = 0;

			//TODO: Implement descriptor indexing feature extension here
			VkDevice logicalDevice;
			VK_RESULT(vkCreateDevice(m_Device, &deviceCreateInfo, nullptr, &logicalDevice));
			
			m_QueueHandles[static_cast<size_t>(QueueFamilyType::GRAPHICS)] = CreateQueueHandle(logicalDevice, QueueFamilyType::GRAPHICS);
			m_QueueHandles[static_cast<size_t>(QueueFamilyType::PRESENTATION)] = CreateQueueHandle(logicalDevice, QueueFamilyType::PRESENTATION);
			m_QueueHandles[static_cast<size_t>(QueueFamilyType::COMPUTE)] = CreateQueueHandle(logicalDevice, QueueFamilyType::COMPUTE);

			return new LogicalDevice(this, logicalDevice, graphicsDetails, surfaceCapabilities, m_QueueFamilyIndices, m_QueueHandles);
		}

		std::vector<VkDeviceQueueCreateInfo> PhysicalDevice::GetQueueCreateInfos(const float* queuePriority)
		{
			VKE_CORE_ASSERT(queuePriority != nullptr, "Queue priority must have a valid pointer!");
			// see https://en.cppreference.com/w/cpp/algorithm/unique
			std::vector<uint32_t> uniqueIndices = m_QueueFamilyIndices;
			uniqueIndices.erase(std::unique(uniqueIndices.begin(), uniqueIndices.end()), uniqueIndices.end());
			std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
			queueCreateInfos.reserve(uniqueIndices.size());
			for (const auto& index : uniqueIndices) 
			{
				VkDeviceQueueCreateInfo createInfo = {};
				createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				createInfo.queueFamilyIndex = index;
				createInfo.queueCount = 1;
				createInfo.pQueuePriorities = queuePriority;
				createInfo.pNext = nullptr;
				createInfo.flags = 0;
				queueCreateInfos.push_back(createInfo);
			}
			return queueCreateInfos;
		}

		VkQueue PhysicalDevice::CreateQueueHandle(VkDevice logicalDevice, QueueFamilyType type)
		{
			VKE_ASSERT(type != QueueFamilyType::ENUM_SIZE, "Queue family type out of bounds!");
			VkQueue queueHandle;
			const uint32_t familyIndex = m_QueueFamilyIndices[static_cast<size_t>(type)];
			VKE_ASSERT(familyIndex != m_InvalidIndex, "Attempting to create a queue handle for a queue family that doesn't exist!");
			vkGetDeviceQueue(logicalDevice, familyIndex, 0, &queueHandle);
			return queueHandle;
		}
	}
}