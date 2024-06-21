#pragma once

#include "LVKCommon.hpp"

namespace Locus
{
	struct LVKImage
	{
		VkImage Image;
		VkImageView View;
		VmaAllocation Allocation;
		VkExtent3D Extent;
		VkFormat Format;
	};
}

namespace LVK
{
	VkImageCreateInfo ImageCreateInfo(VkFormat Format, VkImageUsageFlags UsageFlags, VkExtent3D Extent);
	VkImageViewCreateInfo ImageViewCreateInfo(VkFormat Format, VkImage Image, VkImageAspectFlags AspectFlags);
	
	VkImageMemoryBarrier ImageMemoryBarrier(VkImage Image, VkImageLayout Initial, VkImageLayout Final, u32 InitialQueue, u32 FinalQueue);
	void ImageTransitionLazy(VkCommandBuffer Cmd, const VkImageMemoryBarrier& ImageBarrier);
	void ImageCopyTransfer(VkCommandBuffer Cmd, VkImage Src, VkImage Dst, VkExtent2D SrcExtent, VkExtent2D DstExtent);
}