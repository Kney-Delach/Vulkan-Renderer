#pragma once

#include "Core/Graphics/Core/Utility/VulkanUtility.h"

namespace Vulkan_Engine
{
	namespace Graphics
	{
		using AllocationID = uint64_t;

		enum class MemoryAllocationPattern
		{
			// Resources frequently read/write on GPU. Should use transfer access with this.
			RenderTarget,
			// Resources filled once/rarely on CPU and read frequently on GPU. Should use transfer access with this.
			StaticResource,
			// Change frequently (e.g. per frame/draw call)
			DynamicResource,
			// Written by GPU, read by CPU
			Readback,
			// CPU side for transfer, memory access type is irrelevant when using this pattern
			Staging
		};

		// If direct access is to be used it must be HOST_VISIBLE
		enum class MemoryAccessType
		{
			// Transfer needs a staging buffer on CPU and copies across buffer to buffer
			Transfer,
			// Direct access using map and unmapping
			Direct,
			// Persistent mapping
			Persistent
		};

//#pragma warning (push)
//#pragma warning (disable : 26595)
		struct MemoryAllocationDetails
		{
			union
			{
				VkBuffer Buffer;
				VkImage Image;
			};
			AllocationID Id = 0;
			MemoryAccessType AccessType;
			void* Data = nullptr; // used if access type is Persistent
			VkDeviceSize Size = 0; 
		};
//#pragma warning (pop)
	}
}