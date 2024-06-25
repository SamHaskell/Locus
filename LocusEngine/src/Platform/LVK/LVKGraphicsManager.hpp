#pragma once

#include "Core/DisplayManager.hpp"
#include "Graphics/GraphicsManager.hpp"

#include "LVKCommon.hpp"
#include "LVKTypes.hpp"
#include "LVKResources.hpp"
#include "imgui_internal.h"

#include <deque>
#include <map>
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

/*
	TODO:
	- Implement window resizing
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
	
	struct LVKRenderContext
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
		
		virtual RenderContextHandle CreateRenderContext(const WindowHandle Window) override;
		virtual void DestroyRenderContext(RenderContextHandle RenderContext) override;
		
		virtual void BeginFrame(RenderContextHandle RenderContext) override;
		virtual void EndFrame(RenderContextHandle RenderContext) override;
		
		virtual void BeginFrameImGui() override;
		virtual void EndFrameImGui() override;
		
		virtual ImGuiContext* GetImGuiContext(RenderContextHandle RenderContext) override;
		
		virtual void TestDraw(RenderContextHandle RenderContext) override;		
		
	protected:
		LVKGraphicsDevice m_GraphicsDevice;
		Pool<LVKRenderContext> m_RenderContextPool;
		
		u32 m_ActiveImageIndex = 0;
		bool m_ImGuiInProgress = false;
		
		std::map<RenderContextHandle, VkPipelineLayout> m_TrianglePipelineLayouts;
		std::map<RenderContextHandle, VkPipeline> m_TrianglePipelines;

		void MakePipelines(RenderContextHandle RenderContext);
		LVKFrameResources& GetCurrentFrame(RenderContextHandle RenderContext);
	};
}