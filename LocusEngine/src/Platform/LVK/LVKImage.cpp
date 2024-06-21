#include "LVKImage.hpp"

VkImageCreateInfo LVK::ImageCreateInfo(VkFormat Format, VkImageUsageFlags UsageFlags, VkExtent3D Extent)
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

VkImageViewCreateInfo LVK::ImageViewCreateInfo(VkFormat Format, VkImage Image, VkImageAspectFlags AspectFlags)
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

VkImageMemoryBarrier LVK::ImageMemoryBarrier(VkImage Image, VkImageLayout Initial, VkImageLayout Final, u32 InitialQueue, u32 FinalQueue)
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

void LVK::ImageTransitionLazy(VkCommandBuffer Cmd, const VkImageMemoryBarrier& ImageBarrier)
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

void LVK::ImageCopyTransfer(VkCommandBuffer Cmd, VkImage Src, VkImage Dst, VkExtent2D SrcExtent, VkExtent2D DstExtent)
{

}