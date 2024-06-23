#pragma once

#include "LVKCommon.hpp"
#include <vulkan/vulkan_core.h>

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
		VkRenderPass RenderPass;
		TArray<VkImage> Images = {};
		TArray<VkImageView> ImageViews = {};
		TArray<VkFramebuffer> Framebuffers = {};
	};
}