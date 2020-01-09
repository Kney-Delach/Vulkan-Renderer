#include "vkepch.h"

#include "DeviceMemory.h"

#include "Core/Graphics/Core/Devices/LogicalDevice.h"
#include "Core/Graphics/Core/Devices/PhysicalDevice.h"

// vulkan memory allocator include
#include "vk_mem_alloc.h"

namespace Vulkan_Engine
{
	namespace Graphics
	{
		////////////////////////////////////////////////
		// Device Memory Implementation
		////////////////////////////////////////////////
		class DeviceMemory::Implementation
		{
			friend class DeviceMemory;
		private:
			Implementation(PhysicalDevice* physicalDevice, LogicalDevice* logicalDevice, VkCommandBuffer transferCmdBuffer, VkQueue queue);
			~Implementation();
		private:
			VmaAllocator m_Allocator;
			AllocationID m_AvailableID; // 0 is reserved for the invalid id
			VkQueue m_TranferQueue;
			VkCommandBuffer m_TransferCommandBuffer;
			LogicalDevice* m_LogicalDevice;
			std::map<AllocationID, VmaAllocation> m_AllocationsMap;
		};

		DeviceMemory::Implementation::Implementation(PhysicalDevice* physicalDevice, LogicalDevice* logicalDevice,
			VkCommandBuffer transferCmdBuffer, VkQueue queue)
			: m_AvailableID(1), m_LogicalDevice(logicalDevice), m_TransferCommandBuffer(transferCmdBuffer), m_TranferQueue(queue)
		{
			VmaAllocatorCreateInfo allocatorCreateInfo = {};
			allocatorCreateInfo.physicalDevice = physicalDevice->GetPhysicalDevice();
			allocatorCreateInfo.device = *logicalDevice;
			vmaCreateAllocator(&allocatorCreateInfo, &m_Allocator);
		}

		DeviceMemory::Implementation::~Implementation()
		{
			vmaDestroyAllocator(m_Allocator);
		}

		////////////////////////////////////////////////
		// Device Memory Wrapper
		////////////////////////////////////////////////
		DeviceMemory::DeviceMemory(PhysicalDevice* physicalDevice, LogicalDevice* logicalDevice,
			VkCommandBuffer transferCmdBuffer, VkQueue queue)
			: m_Implementation(new Implementation(physicalDevice, logicalDevice, transferCmdBuffer, queue))
		{}

		DeviceMemory::~DeviceMemory()
		{
			delete m_Implementation;
		}
	}
}
