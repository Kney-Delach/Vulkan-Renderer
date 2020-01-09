#pragma once

#include "Core/Graphics/Core/Utility/VulkanUtility.h"
#include "Core/Graphics/Core/Utility/OptionalExtensions.h"

namespace Vulkan_Engine
{
	namespace Graphics
	{
		// forward declarations: class
		class Window;
		class LogicalDevice;

		// forward declarations: structs
		struct DeviceSurfaceCapabilities;
		struct WindowGraphicsDetails;

		// forward declarations: enums
		enum class QueueFamilyType : size_t;

		class PhysicalDevice
		{
			friend class Window;
			friend class LogicDevice;
		private:
			PhysicalDevice(VkPhysicalDevice device, VkSurfaceKHR surface);
		public:
			PhysicalDevice() = delete;
			~PhysicalDevice() = default;
			bool IsCompatible(DeviceSurfaceCapabilities& surfaceCapabilities, VkSurfaceKHR& surface);
			LogicalDevice* CreateLogicalDevice(const WindowGraphicsDetails& graphicsDetails, DeviceSurfaceCapabilities& surfaceCapabilities, uint32_t enabledLayerCount, const char* const* enabledLayerNames);
			VkPhysicalDevice GetPhysicalDevice() const;
			const VkPhysicalDeviceLimits& GetDeviceLimits() const;
		private:
			bool FindQueueFamilies(VkPhysicalDevice& device, VkSurfaceKHR& surface);
			bool HasRequiredQueueFamilies();
			bool HasRequiredExtensions(DeviceSurfaceCapabilities& surfaceCapabilities, VkSurfaceKHR& surface);
			std::vector<VkDeviceQueueCreateInfo> GetQueueCreateInfos(const float* queuePriority);
			VkQueue CreateQueueHandle(VkDevice logicalDevice, QueueFamilyType type);
		private:
			VkPhysicalDevice m_Device;
			VkPhysicalDeviceFeatures m_Features;
			VkPhysicalDeviceProperties m_Properties;
			std::vector<const char*> m_Extensions;
			std::unordered_map<OptionalExtensions, bool> m_OptionalExtensionsEnabled;
			std::vector<uint32_t> m_QueueFamilyIndices; // use QueueFamilyType to index these 
			std::vector<VkQueue> m_QueueHandles;  // store handle to graphics and presentation queues
		private:
			static const uint32_t m_InvalidIndex = std::numeric_limits<uint32_t>::max();
		};
	}
}
