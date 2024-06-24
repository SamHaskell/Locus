#include "LSDLDisplayManager.hpp"

#include "Base/Asserts.hpp"
#include "Base/Handles.hpp"
#include "Base/Logging.hpp"

#include "Core/DisplayManager.hpp"

#include "SDL.h"
#include "SDL_video.h"
#include "SDL_vulkan.h"
#include "imgui_impl_sdl2.h"

namespace Locus
{
	LSDLDisplayManager::LSDLDisplayManager() : m_WindowPool(WINDOW_COUNT_MAX)
	{
		LAssert(SDL_InitSubSystem(SDL_INIT_VIDEO) == 0);
	}
	
	LSDLDisplayManager::~LSDLDisplayManager()
	{
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
	}
	
	WindowHandle LSDLDisplayManager::CreateWindow(const char* Title, u32 Width, u32 Height)
	{
		LSDLWindow Window = {
			.Flags = SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN,
			.Title = Title
		};
				
		Window.NativeHandle = SDL_CreateWindow(
			Window.Title, 
			SDL_WINDOWPOS_CENTERED, 
			SDL_WINDOWPOS_CENTERED, 
			static_cast<i32>(Width), 
			static_cast<i32>(Height),
		 	Window.Flags
		);
		
		LAssert(Window.NativeHandle != NULL);
		
		WindowHandle WindowHandle = m_WindowPool.Create(Window);
		return WindowHandle;
	}
	
	void LSDLDisplayManager::DestroyWindow(WindowHandle Window)
	{
		if (!m_WindowPool.IsValid(Window))
		{
			LLOG(Display, Warning, "Attempted to destroy a window with an invalid handle!");
			return;
		}
		
		SDL_DestroyWindow(m_WindowPool.Get(Window).NativeHandle);
		m_WindowPool.Destroy(Window);
	}
	
	void* LSDLDisplayManager::GetNativeWindowHandle(WindowHandle Window)
	{
		if (!m_WindowPool.IsValid(Window))
		{
			LLOG(Display, Warning, "Attempted to reference a window with an invalid handle!");
			return nullptr;
		}
		
		return m_WindowPool.Get(Window).NativeHandle;
	}
	
	void LSDLDisplayManager::GetWindowFramebufferSize(WindowHandle Window, u32& Width, u32& Height)
	{
		if (!m_WindowPool.IsValid(Window))
		{
			LLOG(Display, Warning, "Attempted to get window size with an invalid handle!");
			Width = 0;
			Height = 0;
			return;
		}
		
		SDL_GetWindowSize(m_WindowPool.Get(Window).NativeHandle, (i32*)&Width, (i32*)&Height);
	}
	
	void LSDLDisplayManager::GetVulkanInstanceExtensions(WindowHandle Window, TArray<const char*>& OutExtensions) const 
	{
		if (!m_WindowPool.IsValid(Window))
		{
			LLOG(Display, Warning, "Attempted to dereference an invalid handle!");
			OutExtensions.Clear();
			return;
		}
		
		u32 RequiredExtensionCount;
		SDL_Window* WindowHandle = m_WindowPool.Get(Window).NativeHandle;
		SDL_Vulkan_GetInstanceExtensions(WindowHandle, &RequiredExtensionCount, NULL);
		OutExtensions.Reserve(RequiredExtensionCount);
		SDL_Vulkan_GetInstanceExtensions(WindowHandle, &RequiredExtensionCount, OutExtensions.Data());
		OutExtensions.Push(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
	}
	
	bool LSDLDisplayManager::CreateVulkanSurface(WindowHandle Window, VkInstance Instance, VkSurfaceKHR& OutSurface) const 
	{
		if (!m_WindowPool.IsValid(Window))
		{
			LLOG(Display, Warning, "Attempted to dereference an invalid handle!");
			return false;
		}

		SDL_Window* WindowHandle = m_WindowPool.Get(Window).NativeHandle;
		return SDL_Vulkan_CreateSurface(WindowHandle, Instance, &OutSurface) == SDL_TRUE;
	}
	
	void LSDLDisplayManager::PollEvents(bool& bShouldQuit)
	{
		SDL_Event Event;
		while (SDL_PollEvent(&Event))
		{
			ImGui_ImplSDL2_ProcessEvent(&Event);
			
			if (Event.type == SDL_QUIT)
			{
				bShouldQuit = true;
			}
			
			if (Event.type == SDL_WINDOWEVENT)
			{
				if (Event.window.event == SDL_WINDOWEVENT_CLOSE)
				{
					for (arch i = 0; i < m_WindowPool.Count(); i++)
					{
						if (m_WindowPool.IsValidAt(i))
						{
							if (SDL_GetWindowID(m_WindowPool.GetValueAt(i).NativeHandle) == Event.window.windowID)
							{
								DestroyWindow(m_WindowPool.GetHandleAt(i));
							}
						}
					}
				}
			}
			
			if (Event.type == SDL_KEYDOWN)
			{
				if (Event.key.keysym.sym == SDLK_SPACE)
				{
					// Create a window
					CreateWindow("Test", 800, 600);
				}
			}
		}
	}
}