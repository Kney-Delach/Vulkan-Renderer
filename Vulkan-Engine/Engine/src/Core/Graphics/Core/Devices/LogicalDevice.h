#pragma once

#include "Core/Graphics/Core/Utility/VulkanUtility.h"

namespace Vulkan_Engine
{
	namespace Graphics
	{
		// forward declarations: class
		class Window;
		class PhysicalDevice;
		class Descriptor;
		class DeviceMemory;

		// forward declarations: structs
		struct WindowGraphicsDetails;

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
			friend class Window;
			friend class PhysicalDevice;
		public:
			operator VkDevice() const { return m_Device; } // used to interpret this object as a logical VkDevice
			//VkDevice GetLogicalDevice() const;
			//VkPhysicalDevice GetPhysicalDevice() const;
			//const uint32_t GetFamilyIndex(QueueFamilyType type) const;
			//const bool SupportsOptionalExtension(OptionalExtensions ext) const;
			//VkCommandBuffer GetComputeCommandBuffer() const;
			VkQueue GetQueueHandle(QueueFamilyType type) const;
			DeviceMemory* GetDeviceMemory() const;
			Descriptor* GetGraphicsDescriptor() const;

		private:
			LogicalDevice(PhysicalDevice* physicalDevice, VkDevice device, const WindowGraphicsDetails& graphicsDetails, DeviceSurfaceCapabilities& surfaceCapabilities,
				std::vector<uint32_t> indices, std::vector<VkQueue> handles);
			~LogicalDevice();
		private:
			// Each command pool can only allocate command buffers that are submitted on a single type of queue.
			void CreateCommandPools(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags, VkCommandPool* commandPool) const;
			void CreateCommandBuffers(std::vector<VkCommandBuffer>& buffers, VkCommandPool pool, uint32_t count, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY) const;
			void CreateGraphicsDescriptor();
			void LoadDescriptorData(std::vector<std::pair<VkDescriptorType, uint32_t>>& descriptorTypes, uint32_t& setCount);

		private:
			VkDevice m_Device;
			VkCommandPool m_GraphicsCommandPool;
			VkCommandPool m_ComputeCommandPool;
			std::vector<VkCommandBuffer> m_GraphicsCommandBuffers;
			std::vector<VkCommandBuffer> m_ComputeCommandBuffers;
			std::vector<uint32_t> m_QueueFamilyIndices; // use QueueFamilyType to index these 
			std::vector<VkQueue> m_QueueHandles;  // store handle to graphics and presentation queues
			
			PhysicalDevice* m_PhysicalDevice; // Holds physical device so only this object needs to be passed around
			Descriptor* m_GraphicsDescriptor;
			DeviceMemory* m_DeviceMemory;
		};
	}
}

// VkCommandBufferAllocateInfo struct:
// - level: specifies if the allocated command buffers are primary or secondary command buffers.
// -|- VK_COMMAND_BUFFER_LEVEL_PRIMARY   : Can be submitted to a queue for execution, but cannot be called from other command buffers.
// -|- VK_COMMAND_BUFFER_LEVEL_SECONDARY : Cannot be submitted directly, but can be called from primary command buffers. (helpful to reuse common operations from primary command buffers.)
