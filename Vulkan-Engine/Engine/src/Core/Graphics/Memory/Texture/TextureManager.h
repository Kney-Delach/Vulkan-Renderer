#pragma once

#include "Core/Graphics/Core/Utility/VulkanUtility.h"

#include "Image.h"

namespace Vulkan_Engine
{	
	namespace Graphics
	{
		// forward declarations: classes 
		class LogicalDevice;
		class Descriptor;
		class TextureSampler;
		class TextureLoader;

		struct SamplerInfoData
		{
			VkFilter MagFilter = VK_FILTER_LINEAR;
			VkFilter MinFilter = VK_FILTER_LINEAR;
			//TODO: Implement a constructor that takes seperate values for each of these
			VkSamplerAddressMode AddressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			VkSamplerAddressMode AddressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			VkSamplerAddressMode AddressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			float AnisotropyLevel = 16; // lower value better performance (limits amount of texel values that can be used)
			VkShaderStageFlags StageFlag = VK_SHADER_STAGE_FRAGMENT_BIT;
			VkBorderColor BorderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK; // black, white, transparent as either int or float
			SamplerInfoData() = default;
			SamplerInfoData(VkShaderStageFlags stageFlag) : StageFlag(stageFlag) { }
			SamplerInfoData(VkFilter magFilter, VkFilter minFilter, VkSamplerAddressMode addressMode, float anisotropy, VkShaderStageFlags stageFlag, VkBorderColor borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK)
				: MagFilter(magFilter), MinFilter(minFilter), AddressModeU(addressMode), AddressModeV(addressMode), AddressModeW(addressMode), AnisotropyLevel(anisotropy), StageFlag(stageFlag), BorderColor(borderColor) { }
		};

		class TextureManager
		{
		public:
			TextureManager(const LogicalDevice* logicalDevice, uint32_t maxTextures, bool descriptorIndexing = false);
			~TextureManager();
		private:
			const LogicalDevice* m_LogicalDevice;
			Descriptor* m_Descriptor;
			const uint32_t m_MaxTextures;
			
			uint32_t m_DescriptorSetIndex;
			const bool m_DescriptorIndexingActive;
			
			TextureLoader* m_TextureLoader;

			VkDescriptorSetLayoutBinding m_DescriptorSetLayoutBinding;
			
			std::queue<uint32_t> m_FreeDescriptors;

			std::unordered_map<std::string, Image*> textures_;
			std::unordered_map<std::string, TextureSampler*> texturesSamplers_;
			std::unordered_map<std::string, std::pair<TextureSampler*, uint32_t>> textureSamplersDI_;
		};
	}
}


//TODO: Maybe include these in the sampler info data
//// which co-ordinate system we want to use to address the texels in the image (true -> 0-texWidth / height] (otherwise, 0-1)
//samplerInfo.compareEnable = VK_FALSE;
//// used for shadowmaps https://developer.nvidia.com/gpugems/gpugems/part-ii-lighting-and-shadows/chapter-11-shadow-map-antialiasing
//samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS
//// Mipmapping sample choices
//samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
//samplerInfo.mipLodBias = 0.0f; // optional
//samplerInfo.minLod = 0.0f; // by changing this we can force sampler to not use mimnimum LOD 
//samplerInfo.maxLod = static_cast<float>(m_MipLevels); // maximum number of mip map levels generated
//
//
// BRB CLEANING PUPPIES' MESS