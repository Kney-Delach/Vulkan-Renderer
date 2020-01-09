#pragma once
#include "MemoryAllocation.h"

namespace Vulkan_Engine
{
	namespace Graphics
	{
		// forward declarations: classes
		class LogicalDevice;
		class PhysicalDevice;
		class Image;
		
		// forward declarations: structs
		struct WindowGraphicsDetails;

		// Interface for https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/index.html using pimp
		class DeviceMemory
		{
			friend class LogicalDevice;
			class Implementation;
		public:
		private:
			DeviceMemory(PhysicalDevice* physicalDevice, LogicalDevice* logicalDevice, VkCommandBuffer transferCmdBuffer, VkQueue queue);
			~DeviceMemory();
		private:
			Implementation* m_Implementation;
		};
	}
}
