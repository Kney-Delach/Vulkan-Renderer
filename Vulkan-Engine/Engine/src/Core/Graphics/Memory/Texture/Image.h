#pragma once

#include "Core/Graphics/Memory/MemoryAllocation.h"

namespace Vulkan_Engine
{
	namespace Graphics
	{
		// forward declarations: classes
		class LogicalDevice;
		class TextureSampler;
		
		struct ImageParameters
		{
			VkImageViewType Type;
			VkImageAspectFlagBits Flags;
			VkImageLayout Layout;
			ImageParameters(VkImageViewType viewType, VkImageAspectFlagBits flags, VkImageLayout layout) : Type(viewType), Flags(flags), Layout(layout) {}
		};

		class Image
		{
		public:
			Image(const LogicalDevice* logicalDevice, VkImageCreateInfo createInfo, MemoryAllocationPattern pattern, ImageParameters imageParameters, std::string debugName = "");
			~Image();
		private:
		};
	}
}