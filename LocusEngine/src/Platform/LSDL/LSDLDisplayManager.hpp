#pragma once

#include "Core/DisplayManager.hpp"
#include "SDL_video.h"

struct SDL_Window;

namespace Locus
{
	struct LSDLWindow
	{
		SDL_Window* NativeHandle = nullptr;
		u32 Flags;
		const char* Title = "Locus";
	};
	
	class LSDLDisplayManager : public DisplayManager
	{
	public:
		LSDLDisplayManager();
		~LSDLDisplayManager();
	
		WindowHandle CreateWindow(const char* Title, u32 Width, u32 Height) override;
		void DestroyWindow(WindowHandle Window) override;
		
		void* GetNativeWindowHandle(WindowHandle Window) override;
		void GetWindowFramebufferSize(WindowHandle Window, u32& Width, u32& Height) override;
		
		void GetVulkanInstanceExtensions(WindowHandle Window, TArray<const char*>& OutExtensions) const override;
		bool CreateVulkanSurface(WindowHandle Window, VkInstance Instance, VkSurfaceKHR& OutSurface) const override;
		
		void PollEvents(bool& bShouldQuit) override;
		
	private:
		Pool<LSDLWindow> m_WindowPool;
		
	};
}