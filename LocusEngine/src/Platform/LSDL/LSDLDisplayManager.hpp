#pragma once

#include "Core/DisplayManager.hpp"
#include "SDL_video.h"

struct SDL_Window;

namespace Locus
{
	struct LSDLWindowData
	{
		SDL_Window* NativeHandle = nullptr;
		u32 Flags;
		const char* Title = "Locus";
	};
	
	class LSDLDisplayManager : public DisplayManager
	{
	public:
		WindowHandle CreateWindow(const char* Title, u32 Width, u32 Height) override;
		void DestroyWindow(WindowHandle Window) override;
		
		void* GetNativeWindowHandle(WindowHandle Window) override;
		void GetWindowFramebufferSize(WindowHandle Window, u32& Width, u32& Height) override;
		
		bool CreateVulkanSurface(WindowHandle Window, VkInstance Instance, VkSurfaceKHR& OutSurface) const override;
		
		void PollEvents(bool& bShouldQuit) override;
		
	private:
		// Window pooling - May be worth abstraction out to some generic type.
		LSDLWindowData m_WindowData[WINDOW_COUNT_MAX] {};
		WindowHandle  m_WindowHandles[WINDOW_COUNT_MAX] {};
		TArray<u32> m_FreeList; // Def shouldn't be using array here but we dont have queues/stacks yet.
		u32 m_HighestUnoccupiedSlot = 0;
		
	};
}