#pragma once

#include "Core/Graphics/Core/Utility/VulkanUtility.h"

namespace Vulkan_Engine
{
	namespace Graphics
	{
		enum class QueueFamilyType : size_t
		{
			GRAPHICS = 0,
			PRESENTATION = 1,
			COMPUTE = 2,
			ENUM_SIZE = 3 // Do not index with this, this is the size of the structure
		};

		struct DeviceSurfaceCapabilities
		{
			VkSurfaceCapabilitiesKHR Capabilities; // min/max number of images in sc, min/max width / height of images 
			std::vector<VkSurfaceFormatKHR> Formats; // pixel format, color space .... 
			std::vector<VkPresentModeKHR> PresentModes; // available presentation modes
		};
		
		class LogicalDevice
		{
			friend class PhysicalDevice;
		public:
			LogicalDevice() = default;
			~LogicalDevice() = default;
			operator VkDevice() const { return m_Device; } // used to interpret this object as a logical VkDevice
		public:
			VkDevice GetLogicalDevice() const;
			VkPhysicalDevice getPhysicalDevice() const;
			VkQueue getQueueHandle(QueueFamilyType type) const;
		private:
			VkDevice m_Device;
		};
	}
}