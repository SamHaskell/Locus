#include "Platform/LSDL/LSDLWindow.hpp"

#include "Base/Asserts.hpp"
#include "SDL_video.h"
#include "SDL_vulkan.h"
#include <cstddef>

// TODO: Pull out configuration and Vulkan assumptions & add validation.

namespace Locus
{
	LSDLWindow::LSDLWindow(const WindowConfig& Config)
	{
		// LAssert(LSDL::IsInitialized());
		
		u32 WindowFlags = 0;
		WindowFlags |= SDL_WINDOW_VULKAN;
		WindowFlags |= SDL_WINDOW_RESIZABLE;
		WindowHandle = SDL_CreateWindow(
			Config.Title, 
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			Config.Width, Config.Height, 
			WindowFlags
		);
		
		LAssertMsg(WindowHandle != NULL, SDL_GetError());
	}
	
	LSDLWindow::~LSDLWindow()
	{
		SDL_DestroyWindow(WindowHandle);
	}
	
	void* LSDLWindow::GetNativeHandle() const
	{
		return WindowHandle;
	}
	
	bool LSDLWindow::CreateVulkanSurface(VkInstance Instance, VkSurfaceKHR& OutSurface) const
	{
		return (SDL_TRUE == SDL_Vulkan_CreateSurface(WindowHandle, Instance, &OutSurface));
	}
	
	void LSDLWindow::GetFramebufferSize(u32& Width, u32& Height) const
	{
		i32 W, H;
		SDL_GetWindowSizeInPixels(WindowHandle, &W, &H);
		LLOG(Display, Info, "Window Dimensions: %d, %d", W, H);
		Width = static_cast<u32>(W);
		Height = static_cast<u32>(H);
	}
}