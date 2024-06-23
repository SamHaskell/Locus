#pragma once

#include "LVKCommon.hpp"
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

namespace Locus
{
	struct LVKBuffer
	{
		VkBuffer Buffer;
		VmaAllocation Allocation;
		VmaAllocationInfo Info;
	};
	
}

namespace Locus::LVK
{
	LVKBuffer BufferAllocate(arch Size, VkBufferUsageFlags UsageFlags, VmaAllocator Allocator, VmaMemoryUsage MemoryUsage);
}