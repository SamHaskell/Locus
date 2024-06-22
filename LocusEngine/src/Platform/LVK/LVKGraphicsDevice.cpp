#include "LVKGraphicsDevice.hpp"
#include "Platform/LVK/LVKImage.hpp"
#include <vulkan/vulkan_core.h>

#define VMA_IMPLEMENTATION
#include "vma/vk_mem_alloc.h"

#include "Base/Asserts.hpp"
#include "Core/Window.hpp"
#include "Math/Numerics.hpp"

#include "LVKHelpers.hpp"
#include "SDL_vulkan.h"

namespace Locus
{
	void LVKDeletionQueue::Push(std::function<void()>&& Deletion)
	{
		m_Queue.push_front(Deletion);
	}
	
	void LVKDeletionQueue::Flush()
	{
		for (i32 i = 0; i < m_Queue.size(); i++)
		{
			m_Queue[i]();
		}
		
		m_Queue.clear();
	}
	
	LVKGraphicsDevice::LVKGraphicsDevice(const Window* Window)
	{
          u32 RequiredExtensionCount;
          LCheck(SDL_Vulkan_GetInstanceExtensions(
              (SDL_Window *)Window->GetNativeHandle(), &RequiredExtensionCount,
              NULL));
          m_Config.RequiredExtensions.Reserve(RequiredExtensionCount);
          LCheck(SDL_Vulkan_GetInstanceExtensions(
              (SDL_Window *)Window->GetNativeHandle(), &RequiredExtensionCount,
              m_Config.RequiredExtensions.Data()));
          m_Config.RequiredExtensions.Push(
              VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

          m_Config.ValidationLayers.Push("VK_LAYER_KHRONOS_validation");

          m_Config.RequiredDeviceFeatures = {
              // Anything goes for now!
          };

          m_Config.RequiredDeviceExtensions.Push("VK_KHR_portability_subset");
          m_Config.RequiredDeviceExtensions.Push(
              VK_KHR_SWAPCHAIN_EXTENSION_NAME);

          m_Config.AllowedDeviceTypes.Push(
              VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
          m_Config.AllowedDeviceTypes.Push(
              VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);

          CreateDevice(Window);
          
          Window->GetFramebufferSize(m_DrawExtent.width, m_DrawExtent.height);

          VmaAllocatorCreateInfo AllocatorInfo = {
              .flags = 0,
              .physicalDevice = m_PhysicalDevice,
              .device = m_Device,
              .instance = m_Instance,
          };

          vmaCreateAllocator(&AllocatorInfo, &m_Allocator);
          m_DeletionQueue.Push([&]() { vmaDestroyAllocator(m_Allocator); });
          CreateSwapchain(Window);
          CreateFrameResources();
	}
	
	LVKGraphicsDevice::~LVKGraphicsDevice() 
	{
		DestroyFrameResources();
		m_DeletionQueue.Flush();
		DestroySwapchain();
		DestroyDevice();
	}
	
	void LVKGraphicsDevice::TestDraw()
	{
		LVKFrameResources& Frame = GetCurrentFrame();
		VK_CHECK_RESULT(vkWaitForFences(m_Device, 1, &Frame.InFlightFence, VK_TRUE, UINT64_MAX));
		
		Frame.DeletionQueue.Flush();
		
		VK_CHECK_RESULT(vkResetFences(m_Device, 1, &Frame.InFlightFence));
		
		u32 ImageIndex = 0;
		VK_CHECK_RESULT(vkAcquireNextImageKHR(m_Device, m_Swapchain.Swapchain, UINT64_MAX, Frame.ImageAvailableSemaphore, nullptr, &ImageIndex));
		
		VK_CHECK_RESULT(vkResetCommandBuffer(Frame.GraphicsCommandBuffer, 0));
		VkCommandBufferBeginInfo CommandBufferBeginInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.pNext = nullptr,
			.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
			.pInheritanceInfo = nullptr,
		};
		VK_CHECK_RESULT(vkBeginCommandBuffer(Frame.GraphicsCommandBuffer, &CommandBufferBeginInfo));
		
		LAssert(ImageIndex < m_Swapchain.Details.ImageCount);
		
		{
			VkImageMemoryBarrier Barrier = LVK::ImageMemoryBarrier(
				m_DrawImage.Image, 
				VK_IMAGE_LAYOUT_UNDEFINED, 
				VK_IMAGE_LAYOUT_GENERAL,
				m_QueueFamilyIndices.GraphicsFamilyIndex,
				m_QueueFamilyIndices.GraphicsFamilyIndex
			);
			
			LVK::ImageTransitionLazy(
				Frame.GraphicsCommandBuffer, 
				Barrier
			);
		}
		
		// Draw!
		
		{
			VkClearColorValue ClearColor = {{ 0.0f, 0.0f, Locus::Math::Sin((f32)m_FrameNumber / 120.0f), 1.0f}};
			
			VkImageSubresourceRange ClearRange = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = VK_REMAINING_MIP_LEVELS,
				.baseArrayLayer = 0,
				.layerCount = VK_REMAINING_ARRAY_LAYERS
			};
			
			vkCmdClearColorImage(
				Frame.GraphicsCommandBuffer, 
				m_DrawImage.Image, 
				VK_IMAGE_LAYOUT_GENERAL, 
				&ClearColor, 
				1, 
				&ClearRange
			);
		}
		
		// End Draw!

		{
			VkImageMemoryBarrier Barrier = LVK::ImageMemoryBarrier(
				m_DrawImage.Image, 
				VK_IMAGE_LAYOUT_GENERAL, 
				VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				m_QueueFamilyIndices.GraphicsFamilyIndex,
				m_QueueFamilyIndices.GraphicsFamilyIndex
			);
			
			LVK::ImageTransitionLazy(Frame.GraphicsCommandBuffer, Barrier);
		}
		
		{
			VkImageMemoryBarrier Barrier = LVK::ImageMemoryBarrier(
				m_Swapchain.Images[ImageIndex], 
				VK_IMAGE_LAYOUT_UNDEFINED, 
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				m_QueueFamilyIndices.GraphicsFamilyIndex,
				m_QueueFamilyIndices.GraphicsFamilyIndex
			);
			
			LVK::ImageTransitionLazy(Frame.GraphicsCommandBuffer, Barrier);
		}
			
		LVK::ImageBlit(
			Frame.GraphicsCommandBuffer, 
			m_DrawImage.Image, 
			m_Swapchain.Images[ImageIndex], 
			m_DrawExtent, 
			m_Swapchain.Details.Extent
		);
		
		{
			VkImageMemoryBarrier Barrier = LVK::ImageMemoryBarrier(
				m_Swapchain.Images[ImageIndex], 
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, 
				m_QueueFamilyIndices.GraphicsFamilyIndex,
				m_QueueFamilyIndices.GraphicsFamilyIndex
			);
			
			LVK::ImageTransitionLazy(Frame.GraphicsCommandBuffer, Barrier);
		}
		
		VK_CHECK_RESULT(vkEndCommandBuffer(Frame.GraphicsCommandBuffer));
		
		VkCommandBufferSubmitInfo CommandBufferSubmitInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
			.pNext = nullptr,
			.commandBuffer = Frame.GraphicsCommandBuffer,
			.deviceMask = 0
		};
		
		VkSemaphoreSubmitInfo ImageAvailableSemaphoreSubmitInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
			.pNext = nullptr,
			.semaphore = Frame.ImageAvailableSemaphore,
			.value = 1,
			.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
			.deviceIndex = 0,
		};
		
		VkSemaphoreSubmitInfo RenderFinishedSemaphoreSubmitInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
			.pNext = nullptr,
			.semaphore = Frame.RenderFinishedSemaphore,
			.value = 1,
			.stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
			.deviceIndex = 0,
		};
		
		VkPipelineStageFlags WaitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		
		VkSubmitInfo SubmitInfo = {
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.pNext = nullptr,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &Frame.ImageAvailableSemaphore,
			.pWaitDstStageMask = WaitStages,
			.commandBufferCount = 1,
			.pCommandBuffers = &Frame.GraphicsCommandBuffer,
			.signalSemaphoreCount = 1,
			.pSignalSemaphores = &Frame.RenderFinishedSemaphore,
		};
		
		VK_CHECK_RESULT(vkQueueSubmit(m_GraphicsQueue, 1, &SubmitInfo, Frame.InFlightFence));
		
		VkPresentInfoKHR PresentInfo = {
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.pNext = nullptr,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &Frame.RenderFinishedSemaphore,
			.swapchainCount = 1,
			.pSwapchains = &m_Swapchain.Swapchain,
			.pImageIndices = &ImageIndex,
		};
		
		VK_CHECK_RESULT(vkQueuePresentKHR(m_PresentQueue, &PresentInfo));
		m_FrameNumber ++;
	}
	
	void LVKGraphicsDevice::CreateDevice(const Window* Window)
	{
		LVK::CreateInstance(m_Instance, m_Config.RequiredExtensions, m_Config.ValidationLayers);
		VK_CHECK_HANDLE(m_Instance);

		LVK::CreateSurface(Window, m_Instance, m_Surface, nullptr);
		VK_CHECK_HANDLE(m_Surface);

		LVK::ChoosePhysicalDevice(m_Instance, m_Surface, m_PhysicalDevice, m_Config.RequiredDeviceFeatures, m_Config.AllowedDeviceTypes, m_Config.RequiredDeviceExtensions);
		VK_CHECK_HANDLE(m_PhysicalDevice);
		
		LVK::CreateLogicalDevice(m_PhysicalDevice, m_Surface, m_Device, m_QueueFamilyIndices, m_Config.RequiredDeviceFeatures, m_Config.RequiredDeviceExtensions, m_Config.ValidationLayers);
		VK_CHECK_HANDLE(m_Device);	
		
		vkGetDeviceQueue(m_Device, m_QueueFamilyIndices.GraphicsFamilyIndex, 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_Device, m_QueueFamilyIndices.PresentFamilyIndex, 0, &m_PresentQueue);
	}
	
	void LVKGraphicsDevice::DestroyDevice()
	{
		LVK::DestroyDevice(m_Device);
		LVK::DestroySurface(m_Instance, m_Surface);
		LVK::DestroyInstance(m_Instance);
	}
	
	void LVKGraphicsDevice::CreateSwapchain(const Window* Window)
	{
		// Query for swapchain details
		LVKSwapchainSupportDetails SwapchainSupportDetails = LVK::QuerySwapchainSupport(m_Surface, m_PhysicalDevice);
		
		VkSurfaceFormatKHR SurfaceFormat = LVK::ChooseSwapchainSurfaceFormat(SwapchainSupportDetails.Formats);
		VkPresentModeKHR PresentMode = LVK::ChooseSwapchainPresentMode(SwapchainSupportDetails.PresentModes);
		VkExtent2D Extent = LVK::ChooseSwapchainExtent(Window, SwapchainSupportDetails.Capabilities);
		
		u32 ImageCount = SwapchainSupportDetails.Capabilities.minImageCount + 1;
		if (SwapchainSupportDetails.Capabilities.maxImageCount > 0 && ImageCount > SwapchainSupportDetails.Capabilities.maxImageCount) {
    		ImageCount = SwapchainSupportDetails.Capabilities.maxImageCount;
		}
		
		VkSwapchainCreateInfoKHR SwapchainCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.surface = m_Surface,
			.minImageCount = ImageCount,
			.imageFormat = SurfaceFormat.format,
			.imageColorSpace = SurfaceFormat.colorSpace,
			.imageExtent = Extent,
			.imageArrayLayers = 1,
			.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 0,
			.pQueueFamilyIndices = nullptr,
			.preTransform = SwapchainSupportDetails.Capabilities.currentTransform,
			.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			.presentMode = PresentMode,
			.clipped = VK_TRUE,
			.oldSwapchain = VK_NULL_HANDLE
		};
		
		LVKQueueFamilyIndices QueueFamilyIndices = LVK::FindPhysicalDeviceQueueFamilies(m_PhysicalDevice, m_Surface);
		u32 Indices[] = {QueueFamilyIndices.GraphicsFamilyIndex, QueueFamilyIndices.PresentFamilyIndex};
		if (Indices[0] != Indices[1])
		{
			SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			SwapchainCreateInfo.queueFamilyIndexCount = 2;
			SwapchainCreateInfo.pQueueFamilyIndices = Indices;
		}
		
		VK_CHECK_RESULT(vkCreateSwapchainKHR(m_Device, &SwapchainCreateInfo, nullptr, &m_Swapchain.Swapchain));
		
		// Store the details and grab the images
		
		m_Swapchain.Details.Extent = Extent;
		m_Swapchain.Details.ImageFormat = SurfaceFormat.format;
		m_Swapchain.Details.ImageCount = ImageCount;
		
		vkGetSwapchainImagesKHR(m_Device, m_Swapchain.Swapchain, &m_Swapchain.Details.ImageCount, nullptr);
		m_Swapchain.Images.Reserve(m_Swapchain.Details.ImageCount);
		vkGetSwapchainImagesKHR(m_Device, m_Swapchain.Swapchain, &m_Swapchain.Details.ImageCount, m_Swapchain.Images.Data());
		
		// Create the image views
		
		m_Swapchain.ImageViews.Reserve(m_Swapchain.Details.ImageCount);
		for (arch i = 0; i < m_Swapchain.Images.Length(); i++)
		{
			VkImageViewCreateInfo ViewCreateInfo = {
				.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.image = m_Swapchain.Images[i],
				.viewType = VK_IMAGE_VIEW_TYPE_2D,
				.format = m_Swapchain.Details.ImageFormat,
				.components = {
					.r = VK_COMPONENT_SWIZZLE_IDENTITY,
					.g = VK_COMPONENT_SWIZZLE_IDENTITY,
					.b = VK_COMPONENT_SWIZZLE_IDENTITY,
					.a = VK_COMPONENT_SWIZZLE_IDENTITY,
				},
				.subresourceRange = {
					.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					.baseMipLevel = 0,
					.levelCount = 1,
					.baseArrayLayer = 0,
					.layerCount = 1,
				},
			};
			
			VK_CHECK_RESULT(vkCreateImageView(m_Device, &ViewCreateInfo, nullptr, &m_Swapchain.ImageViews[i]));
		}
		
		// Create the intermediary image we will render to.
		
		VkExtent3D DrawImageExtent = { 0, 0, 1 };
		Window->GetFramebufferSize(DrawImageExtent.width, DrawImageExtent.height);
		
		m_DrawImage.Format = VK_FORMAT_R16G16B16A16_SFLOAT;
		m_DrawImage.Extent = DrawImageExtent;
		
		VkImageUsageFlags DrawImageUsageFlags = 
		{VK_IMAGE_USAGE_TRANSFER_SRC_BIT 
		|VK_IMAGE_USAGE_TRANSFER_DST_BIT
		|VK_IMAGE_USAGE_STORAGE_BIT
		|VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT};
		
		VkImageCreateInfo DrawImageInfo = LVK::ImageCreateInfo(m_DrawImage.Format, DrawImageUsageFlags, m_DrawImage.Extent);
		
		VmaAllocationCreateInfo DrawImageAllocInfo = {
			.usage = VMA_MEMORY_USAGE_GPU_ONLY,
			.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
		};
		
		vmaCreateImage(
			m_Allocator, 
			&DrawImageInfo, 
			&DrawImageAllocInfo, 
			&m_DrawImage.Image, 
			&m_DrawImage.Allocation, 
			nullptr
		);
		
		VkImageViewCreateInfo DrawImageViewInfo = LVK::ImageViewCreateInfo(m_DrawImage.Format, m_DrawImage.Image, VK_IMAGE_ASPECT_COLOR_BIT);
		
		VK_CHECK_RESULT(vkCreateImageView(m_Device, &DrawImageViewInfo, nullptr, &m_DrawImage.View));
		
		m_DeletionQueue.Push([&](){
			vkDestroyImageView(m_Device, m_DrawImage.View, nullptr);
			vmaDestroyImage(m_Allocator, m_DrawImage.Image, m_DrawImage.Allocation);
		});
	}
	
	void LVKGraphicsDevice::DestroySwapchain()
	{
		vkDeviceWaitIdle(m_Device);
		
		vkDestroySwapchainKHR(m_Device, m_Swapchain.Swapchain, nullptr);
		for (arch i = 0; i < m_Swapchain.ImageViews.Length(); i++)
		{
			vkDestroyImageView(m_Device, m_Swapchain.ImageViews[i], nullptr);
		}
	}
	
	void LVKGraphicsDevice::CreateFrameResources()
	{
		VkCommandPoolCreateInfo PoolCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.pNext = nullptr,
			.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			.queueFamilyIndex = m_QueueFamilyIndices.GraphicsFamilyIndex
		};
		
		for (i32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			VK_CHECK_RESULT(vkCreateCommandPool(m_Device, &PoolCreateInfo, nullptr, &m_FrameResources[i].GraphicsCommandPool));
			
			VkCommandBufferAllocateInfo AllocInfo = {
				.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
				.pNext = nullptr,
				.commandPool = m_FrameResources[i].GraphicsCommandPool,
				.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
				.commandBufferCount = 1,
			};
			
			VK_CHECK_RESULT(vkAllocateCommandBuffers(m_Device, &AllocInfo, &m_FrameResources[i].GraphicsCommandBuffer));
		}
		
		VkFenceCreateInfo FenceCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.pNext = nullptr,
			.flags = VK_FENCE_CREATE_SIGNALED_BIT
		};
		
		VkSemaphoreCreateInfo SemaphoreCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0
		};
		
		for (i32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			VK_CHECK_RESULT(vkCreateFence(m_Device, &FenceCreateInfo, nullptr, &m_FrameResources[i].InFlightFence));
			VK_CHECK_RESULT(vkCreateSemaphore(m_Device, &SemaphoreCreateInfo, nullptr, &m_FrameResources[i].ImageAvailableSemaphore));
			VK_CHECK_RESULT(vkCreateSemaphore(m_Device, &SemaphoreCreateInfo, nullptr, &m_FrameResources[i].RenderFinishedSemaphore));
		}
	}
	
	void LVKGraphicsDevice::DestroyFrameResources()
	{
		vkDeviceWaitIdle(m_Device);
		
		for (i32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			vkDestroyFence(m_Device, m_FrameResources[i].InFlightFence, nullptr);
			vkDestroySemaphore(m_Device, m_FrameResources[i].ImageAvailableSemaphore, nullptr);
			vkDestroySemaphore(m_Device, m_FrameResources[i].RenderFinishedSemaphore, nullptr);
			vkDestroyCommandPool(m_Device, m_FrameResources[i].GraphicsCommandPool, nullptr);
			
			m_FrameResources[i].DeletionQueue.Flush();
		}
	}
}