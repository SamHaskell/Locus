#pragma once

#include "Graphics/GraphicsManager.hpp"

#include "LVKCommon.hpp"
#include "LVKTypes.hpp"
#include "LVKResources.hpp"

#include <deque>
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

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
		
		LVKDeletionQueue DeletionQueue;
	};
	
	class LVKGraphicsManager : public GraphicsManager
	{
	public:
		LVKGraphicsManager();
		~LVKGraphicsManager();
		
		void Init(const WindowHandle Window) override;
		void TestDraw() override;
		
	protected:
		LVKFrameResources& GetCurrentFrame() { return m_FrameResources[m_FrameNumber % FRAMES_IN_FLIGHT]; }
	
		// These should certainly be global
		LVKConfig m_Config = {};
		VkInstance m_Instance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		VkDevice m_Device = VK_NULL_HANDLE;
		
		LVKQueueFamilyIndices m_QueueFamilyIndices {};
		VkQueue m_GraphicsQueue;
		VkQueue m_PresentQueue;
		VkExtent2D m_DrawExtent;
		
		VmaAllocator m_Allocator;
		LVKDeletionQueue m_DeletionQueue;
		LVKSwapchain m_Swapchain;
		
		u32 m_FrameNumber = 0;
		LVKFrameResources m_FrameResources[FRAMES_IN_FLIGHT];
		
		VkPipelineLayout m_TrianglePipelineLayout;
		VkPipeline m_TrianglePipeline;
		
	};
}