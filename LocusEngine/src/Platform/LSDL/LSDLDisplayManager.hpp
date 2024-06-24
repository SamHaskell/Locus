#pragma once

#include "Core/DisplayManager.hpp"
#include "SDL_video.h"

struct SDL_Window;

/*
	Display manager handles all platform windowing and input events.
	For now, we're using SDL2 to handle this.
	
	You can have multiple windows, but we keep track of a main window.
	- The main window is the first window created.
	- If the main window is shut down, the application will exit.
	- The main window is used to initialize the graphics API.
	- The main window is used to create the main rendering context.
*/

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