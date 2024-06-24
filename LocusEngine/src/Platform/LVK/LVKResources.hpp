#pragma once

#include "LVKCommon.hpp"

namespace Locus
{
	// SWAPCHAIN
	
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
	
	// PIPELINE
	
	struct LVKPipelineFactory
	{
		LVKPipelineFactory() { Clear(); }
		
		void Clear();
		VkPipeline Create(VkDevice Device, VkRenderPass RenderPass);

		TArray<VkPipelineShaderStageCreateInfo> ShaderStages;
		
		VkPipelineColorBlendAttachmentState ColorBlendAttachment;
		VkPipelineVertexInputStateCreateInfo VertexInput;
		VkPipelineInputAssemblyStateCreateInfo InputAssembly;
		VkPipelineRasterizationStateCreateInfo Rasterizer;
		VkPipelineDepthStencilStateCreateInfo DepthStencil;
		VkPipelineMultisampleStateCreateInfo Multisampling;
		
		VkPipelineLayout Layout;
		
		bool bDynamicViewport = true;
		VkViewport Viewport;
		VkRect2D Scissor;
	};
	
	// IMAGE
	
	struct LVKImage
	{
		VkImage Image;
		VkImageView View;
		VmaAllocation Allocation;
		VkExtent3D Extent;
		VkFormat Format;
		
		static VkImageCreateInfo CreateInfo(VkFormat Format, VkImageUsageFlags UsageFlags, VkExtent3D Extent);
		static VkImageViewCreateInfo ViewCreateInfo(VkFormat Format, VkImage Image, VkImageAspectFlags AspectFlags);
		
		static VkImageMemoryBarrier MemoryBarrier(VkImage Image, VkImageLayout Initial, VkImageLayout Final, u32 InitialQueue, u32 FinalQueue);
		static void TransitionLazy(VkCommandBuffer Cmd, const VkImageMemoryBarrier& ImageBarrier);
		static void Blit(VkCommandBuffer Cmd, VkImage Src, VkImage Dst, VkExtent2D SrcExtent, VkExtent2D DstExtent);
	};
	
	// BUFFER
	
	struct LVKBuffer
	{
		VkBuffer Buffer;
		VmaAllocation Allocation;
		VmaAllocationInfo Info;
		
		static LVKBuffer Allocate(arch Size, VkBufferUsageFlags UsageFlags, VmaAllocator Allocator, VmaMemoryUsage MemoryUsage);
	};
};