#include "LVKDescriptor.hpp"
#include "Platform/LVK/LVKTypes.hpp"
#include <vulkan/vulkan_core.h>

namespace Locus
{
	void LVKDescriptorLayoutFactory::Push(u32 Binding, VkDescriptorType Type)
	{
		Bindings.Push({
			.binding = Binding,
			.descriptorType = Type,
			.descriptorCount = 1,
			.stageFlags = 0,
			.pImmutableSamplers = nullptr
		});
	}
	
	void LVKDescriptorLayoutFactory::Clear()
	{
		Bindings.Clear();
	}
	
	VkDescriptorSetLayout LVKDescriptorLayoutFactory::Create(
		VkDevice Device, 
		VkShaderStageFlags ShaderStages, 
		void* pNext, 
		VkDescriptorSetLayoutCreateFlags Flags
	)
	{
		for (arch i = 0; i < Bindings.Length(); i++)
		{
			Bindings[i].stageFlags |= ShaderStages;
		}
		
		VkDescriptorSetLayoutCreateInfo CreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.pNext = pNext,
			.flags = Flags,
			.bindingCount = static_cast<u32>(Bindings.Length()),
			.pBindings = Bindings.Data(),
		};
		
		VkDescriptorSetLayout SetLayout;
		VK_CHECK_RESULT(vkCreateDescriptorSetLayout(Device, &CreateInfo, nullptr, &SetLayout));
		return SetLayout;
	}
	
	void LVKDescriptorAllocator::InitPool(VkDevice Device, VkDescriptorPoolSize* Regions, u32 RegionCount)
	{
		u32 MaxSets = 0;
		for (arch i = 0; i < RegionCount; i++)
		{
			MaxSets += Regions[i].descriptorCount;
		}
		
		VkDescriptorPoolCreateInfo PoolCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.flags = 0,
			.maxSets = MaxSets,
			.poolSizeCount = RegionCount,
			.pPoolSizes = Regions
		};
		
		VK_CHECK_RESULT(vkCreateDescriptorPool(Device, &PoolCreateInfo, nullptr, &Pool));
	}
	
	void LVKDescriptorAllocator::DestroyPool(VkDevice Device)
	{
		vkDestroyDescriptorPool(Device, Pool, nullptr);
	}
	
	VkDescriptorSet LVKDescriptorAllocator::AllocateDescriptor(VkDevice Device, VkDescriptorSetLayout Layout)
	{
		VkDescriptorSetAllocateInfo AllocInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.pNext = nullptr,
			.descriptorPool = Pool,
			.descriptorSetCount = 1,
			.pSetLayouts = &Layout
		};
		
		VkDescriptorSet DescriptorSet;
		VK_CHECK_RESULT(vkAllocateDescriptorSets(Device, &AllocInfo, &DescriptorSet));
		
		return DescriptorSet;
	}
	
	void LVKDescriptorAllocator::ClearDescriptors(VkDevice Device)
	{
		vkResetDescriptorPool(Device, Pool, 0);
	}
}