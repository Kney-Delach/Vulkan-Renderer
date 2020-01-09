#pragma once

#include "Core/Graphics/Core/Utility/VulkanUtility.h"

namespace Vulkan_Engine
{
	namespace Graphics
	{
		// forward declarations: classes
		class LogicalDevice;
		class Image;

		class TextureSampler
		{
		public:
			TextureSampler(const LogicalDevice* logicalDevice, const std::string& name, Image* textureImage, VkFilter magFilter, VkFilter minFilter, VkSamplerAddressMode addressMode, float anisotropy) {}
			~TextureSampler() {}
		private:
			const LogicalDevice* m_LogicalDevice;
			Image* m_Image;
			VkSampler m_Sampler;
			VkDescriptorImageInfo m_ImageInfo;
			const std::string m_Name;
		};
	}
}