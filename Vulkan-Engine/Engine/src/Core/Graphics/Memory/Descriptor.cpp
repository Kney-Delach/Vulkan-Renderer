#include "vkepch.h"

#include "Descriptor.h"
#include "Core/Graphics/Core/Devices/LogicalDevice.h"

namespace Vulkan_Engine
{
	namespace Graphics
	{
		Descriptor::Descriptor(const LogicalDevice* logicalDevice, const uint32_t maxSets, std::vector<std::pair<VkDescriptorType, uint32_t>> types)
			: m_LogicalDevice(logicalDevice)
		{
			// setup descriptor pool sizes
			std::vector<VkDescriptorPoolSize> poolSizes;
			for (const auto type : types) 
			{
				VkDescriptorPoolSize poolSize = {};
				poolSize.type = type.first;
				poolSize.descriptorCount = type.second;
				poolSizes.push_back(poolSize);
			}
			// setup descriptor pool create infos 
			VkDescriptorPoolCreateInfo poolInfo = {};
			poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
			poolInfo.pPoolSizes = poolSizes.data();
			poolInfo.maxSets = maxSets;

			VK_RESULT(vkCreateDescriptorPool(*m_LogicalDevice, &poolInfo, nullptr, &m_DescriptorPool));
		}

		Descriptor::~Descriptor()
		{
			//TODO: Add destroyer for descriptor set layouts 
			vkDestroyDescriptorPool(*m_LogicalDevice, m_DescriptorPool, nullptr);
			m_DescriptorSets.clear();
		}

		size_t Descriptor::CreateSets(const std::vector<VkDescriptorSetLayout>& layouts)
		{
			
		}

		const VkDescriptorSet Descriptor::GetSet(size_t index)
		{
			
		}

		VkDescriptorSetLayout Descriptor::CreateLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings,
			const void* pNext)
		{
			
		}

		void Descriptor::UpdateDescriptorSets(const std::vector<VkWriteDescriptorSet>& descriptorSets)
		{
			
		}
	}
}