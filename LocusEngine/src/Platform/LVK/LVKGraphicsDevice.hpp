#pragma once

#include "Graphics/GraphicsDevice.hpp"

#include "LVKImage.hpp"
#include "LVKSwapchain.hpp"
#include "LVKTypes.hpp"

#include <deque>
#include <functional>

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
		LVKGraphicsDevice(const Window* Window);
		~LVKGraphicsDevice() override;
		
		virtual void TestDraw() override;
		
		inline VkInstance GetInstance() const { return m_Instance; }
		inline VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }
		inline VkDevice GetDevice() const { return m_Device; }
		inline VkSurfaceKHR GetSurface() const { return m_Surface; }
		
		LVKFrameResources& GetCurrentFrame() { return m_FrameResources[m_FrameNumber % FRAMES_IN_FLIGHT]; }
	
	private:
		void CreateDevice(const Window* Window);
		void DestroyDevice();
		
		void CreateSwapchain(const Window* Window);
		void DestroySwapchain();
		
		void CreateFrameResources();
		void DestroyFrameResources();
		
		LVKConfig m_Config;
	
		VkInstance m_Instance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		VkDevice m_Device = VK_NULL_HANDLE;
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
		
		VmaAllocator m_Allocator;
		LVKDeletionQueue m_DeletionQueue;
		
		LVKSwapchain m_Swapchain;
		
		LVKImage m_DrawImage;
		VkExtent2D m_DrawExtent;
		
		LVKQueueFamilyIndices m_QueueFamilyIndices;
		VkQueue m_GraphicsQueue;
		VkQueue m_PresentQueue;
		
		u32 m_FrameNumber = 0;
		LVKFrameResources m_FrameResources[FRAMES_IN_FLIGHT];
	};
}