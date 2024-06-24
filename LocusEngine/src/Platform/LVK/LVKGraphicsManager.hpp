#pragma once

#include "Graphics/GraphicsManager.hpp"

#include "LVKCommon.hpp"
#include "LVKTypes.hpp"
#include "LVKResources.hpp"
#include "imgui_internal.h"

#include <deque>
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

/*
	TODO:
	- Implement window resizing
	- Implement actual multiple window support
	- Cleanup, sort out the vulkan validation messages
*/

namespace Locus
{
	constexpr u32 FRAMES_IN_FLIGHT = 2;
	
	struct LVKDeletionQueue
	{
		std::deque<std::function<void()>> Deletors;
		void Push(std::function<void()>&& DeletionFunction);
		void Flush();
	};
	
	struct LVKFrameResources
	{
		VkSemaphore ImageAvailableSemaphore;
		VkSemaphore RenderFinishedSemaphore;
		VkFence InFlightFence;
		VkCommandPool CommandPool;
		VkCommandBuffer CommandBuffer;
		LVKDeletionQueue PerFrameDeletionQueue;
	};
	
	struct LVKGraphicsDevice
	{
		LVKConfig Config = {};
		VkInstance Instance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT DebugMessenger = VK_NULL_HANDLE;
		VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
		VkDevice Device = VK_NULL_HANDLE;
		VkDescriptorPool ImGuiDescriptorPool = VK_NULL_HANDLE;
		VmaAllocator Allocator;
		LVKQueueFamilyIndices QueueFamilyIndices {};
		VkQueue GraphicsQueue;
		VkQueue PresentQueue;
		LVKDeletionQueue GlobalDeletionQueue;
	};
	
	struct LVKGraphicsContext
	{
		VkSurfaceKHR Surface;
		LVKSwapchain Swapchain;
		u32 FrameNumber = 0;
		LVKFrameResources FrameResources[FRAMES_IN_FLIGHT];
		LVKDeletionQueue PerContextDeletionQueue;
		ImGuiContext* ImGuiContext = nullptr;
	};
	
	class LVKGraphicsManager : public GraphicsManager
	{
	public:
		LVKGraphicsManager();
		~LVKGraphicsManager();
		
		RenderContextHandle CreateRenderContext(const WindowHandle Window) override;

		void TestDraw() override;		
		
	protected:
		void DrawImGui(ImGuiContext* Context, VkCommandBuffer Cmd, std::function<void()> DrawFunction);
		
		LVKFrameResources& GetCurrentFrame() { return m_GraphicsContext.FrameResources[m_GraphicsContext.FrameNumber % FRAMES_IN_FLIGHT]; }
	
		LVKGraphicsDevice m_GraphicsDevice;
		LVKGraphicsContext m_GraphicsContext;
		
		VkPipelineLayout m_TrianglePipelineLayout;
		VkPipeline m_TrianglePipeline;
	};
}