#include "LSDLDisplayManager.hpp"

#include "Base/Handles.hpp"
#include "Base/Logging.hpp"

#include "Core/DisplayManager.hpp"

#include "SDL_events.h"
#include "SDL_video.h"
#include "SDL_vulkan.h"


namespace Locus
{
	WindowHandle LSDLDisplayManager::CreateWindow(const char* Title, u32 Width, u32 Height)
	{
		HandleType Index = 0;
		if(m_FreeList.Empty())
		{
			Index = m_HighestUnoccupiedSlot;
			m_HighestUnoccupiedSlot ++;
		}
		else
		{
			Index = m_FreeList.Pop();
		}
		
		m_WindowHandles[Index] = Handles::Regenerate(m_WindowHandles[Index] | Index);
		
		// Create the window.
		
		m_WindowData[Index].Title = Title;
		m_WindowData[Index].Flags = SDL_WINDOW_SHOWN;
		m_WindowData[Index].Flags = SDL_WINDOW_VULKAN;
		
		m_WindowData[Index].NativeHandle = SDL_CreateWindow(
			m_WindowData[Index].Title, 
			SDL_WINDOWPOS_CENTERED, 
			SDL_WINDOWPOS_CENTERED, 
			static_cast<i32>(Width), 
			static_cast<i32>(Height),
		 	m_WindowData[Index].Flags
		);
		
		return m_WindowHandles[Index];
	}
	
	void LSDLDisplayManager::DestroyWindow(WindowHandle Window)
	{
		if (m_WindowHandles[Handles::GetIndex(Window)] == Window)
		{
			// everything is good
			LLOG(Display, Info, "Destroying window.");
			SDL_DestroyWindow(m_WindowData[Handles::GetIndex(Window)].NativeHandle);
			m_WindowHandles[Handles::GetIndex(Window)] = HANDLE_INVALID;
			m_FreeList.Push(Handles::GetIndex(Window));
		}
		else
		{
			LLOG(Display, Warning, "Attempted to destroy a window with an invalid handle!");
			// handle is invalid/outdated
			// do nothing for now
		}
	}
	
	void* LSDLDisplayManager::GetNativeWindowHandle(WindowHandle Window)
	{
		if (m_WindowHandles[Handles::GetIndex(Window)] == Window)
		{
			return (void*)m_WindowData[Handles::GetIndex(Window)].NativeHandle;
		}
		else
		{
			LLOG(Display, Warning, "Attempted to reference a window with an invalid handle!");
			return nullptr;
		}
	}
	
	void LSDLDisplayManager::GetWindowFramebufferSize(WindowHandle Window, u32& Width, u32& Height)
	{
		if (m_WindowHandles[Handles::GetIndex(Window)] == Window)
		{
			i32 W, H;
			SDL_GetWindowSizeInPixels(m_WindowData[Handles::GetIndex(Window)].NativeHandle, &W, &H);
			Width = static_cast<u32>(W);
			Height = static_cast<u32>(H);
		}
	}
	
	bool LSDLDisplayManager::CreateVulkanSurface(WindowHandle Window, VkInstance Instance, VkSurfaceKHR& OutSurface) const 
	{
		if (m_WindowHandles[Handles::GetIndex(Window)] == Window)
		{
			SDL_Vulkan_CreateSurface(m_WindowData[Handles::GetIndex(Window)].NativeHandle, Instance, &OutSurface);
		}
	}
	
	void LSDLDisplayManager::PollEvents(bool& bShouldQuit)
	{
		SDL_Event Event;
		while (SDL_PollEvent(&Event))
		{
			if (Event.type == SDL_QUIT)
			{
				bShouldQuit = true;
			}
			
			if (Event.type == SDL_WINDOWEVENT)
			{
				if (Event.window.event == SDL_WINDOWEVENT_CLOSE)
				{
					for (arch i = 0; i < WINDOW_COUNT_MAX; i++)
					{
						if (SDL_GetWindowID(m_WindowData[i].NativeHandle) == Event.window.windowID)
						{
							DestroyWindow(m_WindowHandles[i]);
						}
					}
				}
			}
		}
	}
}