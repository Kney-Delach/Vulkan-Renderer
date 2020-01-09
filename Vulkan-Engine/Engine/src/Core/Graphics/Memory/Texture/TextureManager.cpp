#include "vkepch.h"

#include "TextureManager.h"
#include "TextureLoader.h"
#include "TextureSampler.h"

#include "Core/Graphics/Core/Devices/LogicalDevice.h"
#include "Core/Graphics/Memory/Descriptor.h"

namespace Vulkan_Engine
{
	namespace Graphics
	{
		TextureManager::TextureManager(const LogicalDevice* logicalDevice, uint32_t maxTextures, bool descriptorIndexing)
			: m_LogicalDevice(logicalDevice), m_Descriptor(logicalDevice->GetGraphicsDescriptor()), m_MaxTextures(maxTextures),
			m_DescriptorSetIndex(0), m_DescriptorIndexingActive(descriptorIndexing), m_TextureLoader(new TextureLoader(logicalDevice))
		{
			m_DescriptorSetLayoutBinding = {};
			m_DescriptorSetLayoutBinding.binding = 1;
			m_DescriptorSetLayoutBinding.descriptorCount = maxTextures; 
			m_DescriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			m_DescriptorSetLayoutBinding.pImmutableSamplers = nullptr;
			m_DescriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS; // set descriptor to be active for all stages 

			for (uint32_t i = 0; i < maxTextures; ++i) 
			{
				m_FreeDescriptors.push(i);
			}

		}

		TextureManager::~TextureManager()
		{
			VKE_SAFE_DELETE(m_TextureLoader);

		}
	}
}