#pragma once

#include "LVKCommon.hpp"

namespace Locus
{
	struct LVKDescriptorLayoutFactory
	{
		TArray<VkDescriptorSetLayoutBinding> Bindings;
		
		void Push(u32 Binding, VkDescriptorType Type);
		void Clear();
		
		VkDescriptorSetLayout Create(
			VkDevice Device, 
			VkShaderStageFlags ShaderStages, 
			void* pNext = nullptr, 
			VkDescriptorSetLayoutCreateFlags Flags = 0
		);
	};
	
	struct LVKDescriptorAllocator
	{
		VkDescriptorPool Pool;
		
		void InitPool(VkDevice Device, VkDescriptorPoolSize* Regions, u32 RegionCount);
		void DestroyPool(VkDevice Device);
		
		VkDescriptorSet AllocateDescriptor(VkDevice Device, VkDescriptorSetLayout Layout);
		void ClearDescriptors(VkDevice Device);
	};
};