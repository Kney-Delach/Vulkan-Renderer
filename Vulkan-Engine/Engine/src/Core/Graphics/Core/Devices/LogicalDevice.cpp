#include "vkepch.h"

#include "LogicalDevice.h"

namespace Vulkan_Engine
{
	namespace Graphics
	{
		LogicalDevice::LogicalDevice(PhysicalDevice* physicalDevice, VkDevice device,
			const WindowGraphicsDetails& graphicsDetails, DeviceSurfaceCapabilities& surfaceCapabilities,
			std::vector<uint32_t> indices, std::vector<VkQueue> handles)
		: m_PhysicalDevice(physicalDevice), m_Device(device), m_QueueFamilyIndices(indices), m_QueueHandles(handles)
		{
			CreateCommandPools(m_QueueFamilyIndices[static_cast<size_t>(QueueFamilyType::GRAPHICS)], VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, &m_GraphicsCommandPool);
			//CreateCommandPools(m_QueueFamilyIndices[static_cast<size_t>(QueueFamilyType::COMPUTE)], VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, &m_ComputeCommandPool);

			//TODO: Check if this needs to be recreated when re-creating the swap chain
			CreateCommandBuffers(m_GraphicsCommandBuffers, m_GraphicsCommandPool, 4); //TODO: Find out why this is 4...
			//CreateCommandBuffers(m_ComputeCommandBuffers, m_ComputeCommandPool, 1);

			//TODO: Create Descriptors

			//TODO: Create Device Memory
		}

		LogicalDevice::~LogicalDevice()
		{
			vkFreeCommandBuffers(m_Device, m_GraphicsCommandPool, static_cast<uint32_t>(m_GraphicsCommandBuffers.size()), m_GraphicsCommandBuffers.data());
			vkDestroyCommandPool(m_Device, m_GraphicsCommandPool, nullptr);
			//vkFreeCommandBuffers(m_Device, m_ComputeCommandPool, static_cast<uint32_t>(m_ComputeCommandBuffers.size()), m_ComputeCommandBuffers.data());
			//vkDestroyCommandPool(m_Device, m_ComputeCommandPool, nullptr);
			vkDestroyDevice(m_Device, nullptr);

		}

		void LogicalDevice::CreateCommandPools(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags,
			VkCommandPool* commandPool) const
		{
			VkCommandPoolCreateInfo poolInfo = {};
			poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			poolInfo.flags = flags; // (with existing reset flag) -> allow command buffers to be rerecorded individually, without this flag they all have to be reset together
			poolInfo.queueFamilyIndex = queueFamilyIndex;
			VK_RESULT(vkCreateCommandPool(m_Device, &poolInfo, nullptr, commandPool));
		}
		
		void LogicalDevice::CreateCommandBuffers(std::vector<VkCommandBuffer>& buffers, VkCommandPool pool,
			uint32_t count, VkCommandBufferLevel level) const
		{
			// One for drawing each frame plus 1 for buffer/image transfer commands
			const size_t startSize = buffers.size();
			buffers.resize(startSize + count);
			
			VkCommandBufferAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.commandPool = pool;
			allocInfo.level = level;
			allocInfo.commandBufferCount = count;

			VK_RESULT(vkAllocateCommandBuffers(m_Device, &allocInfo, &buffers[startSize]));
		}
	}
}
