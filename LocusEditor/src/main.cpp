#include "Locus.hpp"

#include "SDL.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_vulkan.h"

using namespace Locus;

i32 main(i32 argc, char* argv[])
{
	Platform::Init();
	
	Window* Window = Window::Create(WindowConfig{"Locus", 1280, 720});
	GraphicsDevice* GraphicsDevice = GraphicsDevice::Create(Window);
	
	bool bShouldQuit = false;
	bool bShouldRender = true;
	while (!bShouldQuit)
	{
		SDL_Event Event;
		while(SDL_PollEvent(&Event) != 0)
		{
			if (Event.type == SDL_QUIT)
			{
				bShouldQuit = true;
			}
			
			if (Event.type == SDL_WINDOWEVENT)
			{
				if (Event.window.event == SDL_WINDOWEVENT_MINIMIZED)
				{
					bShouldRender = false;
				}
				
				if (Event.window.event == SDL_WINDOWEVENT_RESTORED)
				{
					bShouldRender = true;
				}
			}
		}
		
		if (!bShouldRender)
		{
			Platform::SleepThisThread(100);
			continue;
		}
		
		GraphicsDevice->TestDraw();
	}
	
	delete GraphicsDevice;
	delete Window;
	
	return 0;
}