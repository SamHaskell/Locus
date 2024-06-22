#pragma once

#include "LVKCommon.hpp"

namespace Locus
{
	struct LVKSwapchainDetails
	{
		VkFormat ImageFormat;
		VkExtent2D Extent;
		u32 ImageCount;
	};
	
	struct LVKSwapchain 
	{
		LVKSwapchainDetails Details = {};		
		VkSwapchainKHR Swapchain = VK_NULL_HANDLE;
		TArray<VkImage> Images = {};
		TArray<VkImageView> ImageViews = {};
	};
}