#include "LVKImage.hpp"
#include <vulkan/vulkan_core.h>

VkImageCreateInfo Locus::LVK::ImageCreateInfo(VkFormat Format, VkImageUsageFlags UsageFlags, VkExtent3D Extent)
{
	return {
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.pNext = nullptr,
		.imageType = VK_IMAGE_TYPE_2D,
		.format = Format,
		.extent = Extent,
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.usage = UsageFlags,
	};
}

VkImageViewCreateInfo Locus::LVK::ImageViewCreateInfo(VkFormat Format, VkImage Image, VkImageAspectFlags AspectFlags)
{
	return {
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = nullptr,
		.image = Image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = Format,
		.subresourceRange = {
			.aspectMask = AspectFlags,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1,
		},
	};
}

VkImageMemoryBarrier Locus::LVK::ImageMemoryBarrier(VkImage Image, VkImageLayout Initial, VkImageLayout Final, u32 InitialQueue, u32 FinalQueue)
{
	VkImageAspectFlags AspectMask = (Final == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
	return {
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.pNext = nullptr,
		.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT,
		.dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT,
		.oldLayout = Initial,
		.newLayout = Final,
		.srcQueueFamilyIndex = InitialQueue,
		.dstQueueFamilyIndex = FinalQueue,
		.image = Image,
		.subresourceRange = {
			.aspectMask = AspectMask,
			.baseMipLevel = 0,
			.levelCount = VK_REMAINING_MIP_LEVELS,
			.baseArrayLayer = 0,
			.layerCount = VK_REMAINING_ARRAY_LAYERS
		},
	};
}

void Locus::LVK::ImageTransitionLazy(VkCommandBuffer Cmd, const VkImageMemoryBarrier& ImageBarrier)
{
	vkCmdPipelineBarrier(
		Cmd, 
		VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &ImageBarrier
	);	
}

void Locus::LVK::ImageBlit(VkCommandBuffer Cmd, VkImage Src, VkImage Dst, VkExtent2D SrcExtent, VkExtent2D DstExtent)
{
	VkImageBlit BlitRegion = {
		.srcSubresource = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.mipLevel = 0,
			.baseArrayLayer = 0,
			.layerCount = 1
		},
		.srcOffsets = {{0, 0, 0}, {static_cast<i32>(SrcExtent.width), static_cast<i32>(SrcExtent.height), 1}},
		.dstSubresource = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.mipLevel = 0,
			.baseArrayLayer = 0,
			.layerCount = 1
		},
		.dstOffsets = {{0, 0, 0}, {static_cast<i32>(DstExtent.width), static_cast<i32>(DstExtent.height), 1}}
	};
	
	vkCmdBlitImage(
		Cmd,
		Src,
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		Dst,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&BlitRegion,
		VK_FILTER_LINEAR
	);
}