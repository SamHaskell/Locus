#include "LVKGraphicsManager.hpp"

#include "LVKHelpers.hpp"
#include "LVKResources.hpp"

#include "Core/DisplayManager.hpp"
#include "Platform/Platform.hpp"

#define VMA_IMPLEMENTATION
#include "vma/vk_mem_alloc.h"

#include "SDL_video.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_vulkan.h"

namespace Locus
{
	void LVKDeletionQueue::Push(std::function<void()>&& DeletionFunction)
	{
		Deletors.push_back(DeletionFunction);
	}
	
	void LVKDeletionQueue::Flush()
	{
		for (auto It = Deletors.rbegin(); It != Deletors.rend(); It++)
		{
			(*It)();
		}
		Deletors.clear();
	}
	
	LVKGraphicsManager::LVKGraphicsManager()
	{
		
	}
	
	LVKGraphicsManager::~LVKGraphicsManager()
	{
		m_DeletionQueue.Flush();
	}
	
	void LVKGraphicsManager::Init(const WindowHandle Window)
	{
		// Config
		
		DisplayManager::Get().GetVulkanInstanceExtensions(Window, m_Config.RequiredExtensions);
		
        m_Config.ValidationLayers.Push("VK_LAYER_KHRONOS_validation");

        m_Config.RequiredDeviceFeatures = {/* Anything goes for now! */};

        m_Config.RequiredDeviceExtensions.Push("VK_KHR_portability_subset");
        m_Config.RequiredDeviceExtensions.Push(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        m_Config.AllowedDeviceTypes.Push(VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
        m_Config.AllowedDeviceTypes.Push(VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
		
        // Device
        
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
		
        DisplayManager::Get().GetWindowFramebufferSize(Window, m_DrawExtent.width, m_DrawExtent.height);
		
		m_DeletionQueue.Push([&](){
			LVK::DestroyDevice(m_Device);
			LVK::DestroySurface(m_Instance, m_Surface);
			LVK::DestroyInstance(m_Instance);	
		});
		
		// Allocator
		
		VmaAllocatorCreateInfo AllocatorInfo = {
            .flags = 0,
            .physicalDevice = m_PhysicalDevice,
            .device = m_Device,
            .instance = m_Instance,
        };
        vmaCreateAllocator(&AllocatorInfo, &m_Allocator);
        m_DeletionQueue.Push([&]() { vmaDestroyAllocator(m_Allocator); });
        
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
		
		m_DeletionQueue.Push([&](){
			vkDeviceWaitIdle(m_Device);
			vkDestroySwapchainKHR(m_Device, m_Swapchain.Swapchain, nullptr);
			for (arch i = 0; i < m_Swapchain.ImageViews.Length(); i++)
			{
				vkDestroyImageView(m_Device, m_Swapchain.ImageViews[i], nullptr);
			}
		});
		
		// Renderpass
		
		VkAttachmentDescription ColorAttachment = {
			.format = m_Swapchain.Details.ImageFormat,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		};
		
		VkAttachmentReference ColorAttachmentRef = {
			.attachment = 0,
			.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		};
		
		VkSubpassDescription Subpass = {
			.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
			.colorAttachmentCount = 1,
			.pColorAttachments = &ColorAttachmentRef
		};
		
		VkRenderPassCreateInfo RenderPassInfo = {
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			.attachmentCount = 1,
			.pAttachments = &ColorAttachment,
			.subpassCount = 1,
			.pSubpasses = &Subpass
		};
		
		VK_CHECK_RESULT(vkCreateRenderPass(m_Device, &RenderPassInfo, nullptr, &m_Swapchain.RenderPass));
		
		m_DeletionQueue.Push([&](){
			vkDestroyRenderPass(m_Device, m_Swapchain.RenderPass, nullptr);
		});
		
		// Create framebuffers
		
		m_Swapchain.Framebuffers.Reserve(m_Swapchain.Details.ImageCount);
		for (arch i = 0; i < m_Swapchain.Framebuffers.Length(); i++)
		{
			VkImageView Attachments[] = {m_Swapchain.ImageViews[i]};
			VkFramebufferCreateInfo FramebufferInfo = {
				.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
				.renderPass = m_Swapchain.RenderPass,
				.attachmentCount = 1,
				.pAttachments = Attachments,
				.width = m_Swapchain.Details.Extent.width,
				.height = m_Swapchain.Details.Extent.height,
				.layers = 1
			};
			
			VK_CHECK_RESULT(vkCreateFramebuffer(m_Device, &FramebufferInfo, nullptr, &m_Swapchain.Framebuffers[i]));
		}
      
		m_DeletionQueue.Push([&](){
			for (arch i = 0; i < m_Swapchain.Framebuffers.Length(); i++)
			{
				vkDestroyFramebuffer(m_Device, m_Swapchain.Framebuffers[i], nullptr);
			}
		});
		
		// Frame resources
		
		VkCommandPoolCreateInfo PoolCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.pNext = nullptr,
			.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			.queueFamilyIndex = m_QueueFamilyIndices.GraphicsFamilyIndex
		};
		
		for (i32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			VK_CHECK_RESULT(vkCreateCommandPool(m_Device, &PoolCreateInfo, nullptr, &m_FrameResources[i].CommandPool));
			
			VkCommandBufferAllocateInfo AllocInfo = {
				.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
				.pNext = nullptr,
				.commandPool = m_FrameResources[i].CommandPool,
				.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
				.commandBufferCount = 1,
			};
			
			VK_CHECK_RESULT(vkAllocateCommandBuffers(m_Device, &AllocInfo, &m_FrameResources[i].CommandBuffer));
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
		
		m_DeletionQueue.Push([&](){
			vkDeviceWaitIdle(m_Device);
			
			for (i32 i = 0; i < FRAMES_IN_FLIGHT; i++)
			{
				vkDestroyFence(m_Device, m_FrameResources[i].InFlightFence, nullptr);
				vkDestroySemaphore(m_Device, m_FrameResources[i].ImageAvailableSemaphore, nullptr);
				vkDestroySemaphore(m_Device, m_FrameResources[i].RenderFinishedSemaphore, nullptr);
				vkDestroyCommandPool(m_Device, m_FrameResources[i].CommandPool, nullptr);
				
				m_FrameResources[i].DeletionQueue.Flush();
			}
		});
		
		VkPipelineLayoutCreateInfo TriangleLayout = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.setLayoutCount = 0,
			.pSetLayouts = nullptr
		};
		
		VK_CHECK_RESULT(vkCreatePipelineLayout(m_Device, &TriangleLayout, nullptr, &m_TrianglePipelineLayout));
		
		const char* VertShaderPath = "./build/LocusEngine/content/shaders/triangle.vert.spv";
		
		arch VertCodeSize;
		LAssert(Platform::FileGetSize(VertShaderPath, VertCodeSize));
		u8 VertShaderCode[VertCodeSize];
		LAssert(Platform::FileReadBytes(VertShaderPath, VertShaderCode, VertCodeSize));
		
		VkShaderModule VertShader = LVK::CreateShaderModule(m_Device, nullptr, VertShaderCode, VertCodeSize);
		
		const char* FragShaderPath = "./build/LocusEngine/content/shaders/triangle.frag.spv";
		
		arch FragCodeSize;
		LAssert(Platform::FileGetSize(FragShaderPath, FragCodeSize));
		u8 FragShaderCode[FragCodeSize];
		LAssert(Platform::FileReadBytes(FragShaderPath, FragShaderCode, FragCodeSize));
		
		VkShaderModule FragShader = LVK::CreateShaderModule(m_Device, nullptr, FragShaderCode, FragCodeSize);
		
		LVKPipelineFactory PipelineFactory;
		PipelineFactory.Layout = m_TrianglePipelineLayout;
		PipelineFactory.InputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		PipelineFactory.Rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		
		PipelineFactory.ShaderStages.Push({
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.stage = VK_SHADER_STAGE_VERTEX_BIT,
			.module = VertShader,
			.pName = "main",
			.pSpecializationInfo = nullptr,
		});
		
		PipelineFactory.ShaderStages.Push({
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
			.module = FragShader,
			.pName = "main",
			.pSpecializationInfo = nullptr,
		});
		
		m_TrianglePipeline = PipelineFactory.Create(m_Device, m_Swapchain.RenderPass);
		
		vkDestroyShaderModule(m_Device, VertShader, nullptr);
		vkDestroyShaderModule(m_Device, FragShader, nullptr);
		
		m_DeletionQueue.Push([&](){
			vkDeviceWaitIdle(m_Device);
			vkDestroyPipelineLayout(m_Device, m_TrianglePipelineLayout, nullptr);
			vkDestroyPipeline(m_Device, m_TrianglePipeline, nullptr);
		});
		
		// ImGui
		
		VkDescriptorPoolSize PoolSizes[] = { { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 } };
		
		VkDescriptorPoolCreateInfo PoolInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
			.maxSets = 1000,
			.poolSizeCount = 11,
			.pPoolSizes = PoolSizes
		};
		
		VkDescriptorPool ImGuiPool;
		VK_CHECK_RESULT(vkCreateDescriptorPool(m_Device, &PoolInfo, nullptr, &ImGuiPool));
		
		// Init ImGui
		
		ImGui::CreateContext();
		ImGui_ImplSDL2_InitForVulkan((SDL_Window*)DisplayManager::Get().GetNativeWindowHandle(Window));
		
		ImGui_ImplVulkan_InitInfo InitInfo = {
			.Instance = m_Instance,
			.PhysicalDevice = m_PhysicalDevice,
			.Device = m_Device,
			.Queue = m_GraphicsQueue,
			.DescriptorPool = ImGuiPool,
			.RenderPass = m_Swapchain.RenderPass,
			.MinImageCount = 3,
			.ImageCount = 3,
			.MSAASamples = VK_SAMPLE_COUNT_1_BIT,
			.UseDynamicRendering = false,
			.PipelineRenderingCreateInfo = {
				.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
				.colorAttachmentCount = 1,
				.pColorAttachmentFormats = &m_Swapchain.Details.ImageFormat
			},
		};
		
		ImGui_ImplVulkan_Init(&InitInfo);
		ImGui_ImplVulkan_CreateFontsTexture();
		
		m_DeletionQueue.Push([=](){
			ImGui_ImplVulkan_Shutdown();
			vkDestroyDescriptorPool(m_Device, ImGuiPool, nullptr);
		});
	}
	
	void LVKGraphicsManager::TestDraw()
	{
		LVKFrameResources& Frame = GetCurrentFrame();
		VK_CHECK_RESULT(vkWaitForFences(m_Device, 1, &Frame.InFlightFence, VK_TRUE, UINT64_MAX));
		
		Frame.DeletionQueue.Flush();
		
		VK_CHECK_RESULT(vkResetFences(m_Device, 1, &Frame.InFlightFence));
		
		u32 ImageIndex = 0;
		VK_CHECK_RESULT(vkAcquireNextImageKHR(m_Device, m_Swapchain.Swapchain, UINT64_MAX, Frame.ImageAvailableSemaphore, nullptr, &ImageIndex));
		
		VkCommandBuffer Cmd = Frame.CommandBuffer;
		
		VK_CHECK_RESULT(vkResetCommandBuffer(Cmd, 0));
		VkCommandBufferBeginInfo CommandBufferBeginInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.pNext = nullptr,
			.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
			.pInheritanceInfo = nullptr,
		};
		VK_CHECK_RESULT(vkBeginCommandBuffer(Cmd, &CommandBufferBeginInfo));
		
		LAssert(ImageIndex < m_Swapchain.Details.ImageCount);
		
		VkClearValue ClearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
		
		VkRenderPassBeginInfo RenderPassBeginInfo = {
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			.renderPass = m_Swapchain.RenderPass,
			.framebuffer = m_Swapchain.Framebuffers[ImageIndex],
			.renderArea = {
				.offset = {0, 0},
				.extent = m_Swapchain.Details.Extent,
			},
			.clearValueCount = 1,
			.pClearValues = &ClearColor
		};
		
		vkCmdBeginRenderPass(Cmd, &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE); 
		
		vkCmdBindPipeline(Cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_TrianglePipeline);
		
		VkViewport Viewport = {
			.x = 0.0f,
			.y = 0.0f,
			.width = static_cast<f32>(m_Swapchain.Details.Extent.width),
			.height = static_cast<f32>(m_Swapchain.Details.Extent.height),
			.minDepth = 0.0f,
			.maxDepth = 1.0f
		};
		
		vkCmdSetViewport(Cmd, 0, 1, &Viewport);
		
		VkRect2D Scissor = {
			.offset = {0, 0},
			.extent = m_Swapchain.Details.Extent
		};
		
		vkCmdSetScissor(Cmd, 0, 1, &Scissor);
		
		vkCmdDraw(Cmd, 3, 1, 0, 0);

		vkCmdEndRenderPass(Cmd);
					
		VK_CHECK_RESULT(vkEndCommandBuffer(Cmd));
		
		VkCommandBufferSubmitInfo CommandBufferSubmitInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
			.pNext = nullptr,
			.commandBuffer = Cmd,
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
			.pCommandBuffers = &Cmd,
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
}