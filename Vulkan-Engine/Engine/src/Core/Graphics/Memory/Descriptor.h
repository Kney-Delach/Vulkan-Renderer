#pragma once

#include "Core/Graphics/Core/Utility/VulkanUtility.h"

namespace Vulkan_Engine
{
	namespace Graphics
	{
		// forward declarations: classes
		class LogicalDevice;
		
		class Descriptor
		{
		public:
			Descriptor(const LogicalDevice* logicalDevice, const uint32_t maxSets, std::vector<std::pair<VkDescriptorType, uint32_t>> types);
			~Descriptor();
			// Return the index of the first set created with the first layout in layouts, with sets allocated for each layout given.
			// Increment the index returned appropriately to match a set.
			size_t CreateSets(const std::vector<VkDescriptorSetLayout>& layouts);
			const VkDescriptorSet GetSet(size_t index);
			VkDescriptorSetLayout CreateLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings, const void* pNext = nullptr);
			void UpdateDescriptorSets(const std::vector<VkWriteDescriptorSet>& descriptorSets);
		private:
			VkDescriptorPool m_DescriptorPool;
			std::vector<VkDescriptorSet> m_DescriptorSets;
			std::vector<VkDescriptorSetLayout> m_DescriptorLayouts;
			const LogicalDevice* m_LogicalDevice;
		};
	}
}
