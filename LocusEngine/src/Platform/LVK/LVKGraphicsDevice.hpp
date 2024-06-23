#pragma once

#include "Core/DisplayManager.hpp"
#include "Graphics/GraphicsDevice.hpp"

#include "LVKImage.hpp"
#include "LVKSwapchain.hpp"
#include "LVKTypes.hpp"
#include "LVKDescriptor.hpp"

#include <deque>
#include <functional>
#include <vulkan/vulkan_core.h>

namespace Locus
{
	// TODO: Separate instance creation and debug utils to some GraphicsBackend initialisation
	// We should be able to have multiple graphics devices, which each have their own draw loop management structures
	
	class LVKDeletionQueue
	{
	public:
		LVKDeletionQueue() = default;
		void Push(std::function<void()>&& Deletion);
		void Flush();
	
	private:
		std::deque<std::function<void()>> m_Queue;
	};
	
	struct LVKFrameResources 
	{
		VkCommandPool GraphicsCommandPool;
		VkCommandBuffer GraphicsCommandBuffer;
		VkSemaphore ImageAvailableSemaphore;
		VkSemaphore RenderFinishedSemaphore;
		VkFence InFlightFence;
		
		LVKDeletionQueue DeletionQueue;
	}; 
	
	class LVKGraphicsDevice : public GraphicsDevice 
	{
	public:
		LVKGraphicsDevice(const WindowHandle Window);
		~LVKGraphicsDevice() override;
		
		virtual void TestDraw() override;
		void DrawImGui(VkCommandBuffer Cmd, VkImageView TargetView);
		
		inline VkInstance GetInstance() const { return m_Instance; }
		inline VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }
		inline VkDevice GetDevice() const { return m_Device; }
		inline VkSurfaceKHR GetSurface() const { return m_Surface; }
		LVKFrameResources& GetCurrentFrame() { return m_FrameResources[m_FrameNumber % FRAMES_IN_FLIGHT]; }
	
	private:
		void SetupDevice(const WindowHandle Window);
		void SetupAllocator();
		void SetupSwapchain(const WindowHandle Window);
		void SetupFrameResources();
		void SetupDescriptors();
		void SetupPipelines();
		void SetupBackgroundPipelines();
		
		void SetupTrianglePipeline();
		
		void SetupImGui(const WindowHandle Window);
		
		LVKConfig m_Config;
	
		// Device?
		
		VkInstance m_Instance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		VkDevice m_Device = VK_NULL_HANDLE;
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
		
		LVKQueueFamilyIndices m_QueueFamilyIndices;
		VkQueue m_GraphicsQueue;
		VkQueue m_PresentQueue;
		
		VmaAllocator m_Allocator;
		LVKDeletionQueue m_DeletionQueue;
		
		// Swapchain?
		
		LVKSwapchain m_Swapchain;
		LVKImage m_DrawImage;
		VkExtent2D m_DrawExtent;
		
		// ???
		
		u32 m_FrameNumber = 0;
		LVKFrameResources m_FrameResources[FRAMES_IN_FLIGHT];
		
		LVKDescriptorAllocator m_DescriptorAllocator;
		VkDescriptorSet m_DrawImageDescriptorSet;
		VkDescriptorSetLayout m_DrawImageDescriptorLayout;
		
		VkPipeline m_GradientPipeline;
		VkPipelineLayout m_GradientPipelineLayout;
		
		VkPipeline m_TrianglePipeline;
		VkPipelineLayout m_TrianglePipelineLayout;
	};
}