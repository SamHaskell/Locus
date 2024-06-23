#include "LVKBuffer.hpp"

#include "LVKTypes.hpp"

namespace Locus
{
	LVKBuffer LVK::BufferAllocate(arch Size, VkBufferUsageFlags UsageFlags, VmaAllocator Allocator, VmaMemoryUsage MemoryUsage)
	{
		VkBufferCreateInfo BufferInfo = {
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.pNext = nullptr,
			.size = Size,
			.usage = UsageFlags,
		};
		
		VmaAllocationCreateInfo VmaAllocInfo = {
			.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT,
			.usage = MemoryUsage,
		};
		
		LVKBuffer OutBuffer;
		VK_CHECK_RESULT(vmaCreateBuffer(Allocator, &BufferInfo, &VmaAllocInfo, &OutBuffer.Buffer, &OutBuffer.Allocation, &OutBuffer.Info));
		return OutBuffer;
	}
}
