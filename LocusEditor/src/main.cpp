#include "Locus.hpp"

#include "SDL.h"

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
		SDL_Event event;
		while(SDL_PollEvent(&event) != 0)
		{
			if (event.type == SDL_QUIT)
			{
				bShouldQuit = true;
			}
			
			if (event.type == SDL_WINDOWEVENT)
			{
				if (event.window.event == SDL_WINDOWEVENT_MINIMIZED)
				{
					bShouldRender = false;
				}
				
				if (event.window.event == SDL_WINDOWEVENT_RESTORED)
				{
					bShouldRender = true;
				}
			}
		}
		
		if (!bShouldRender)
		{
			Platform::SleepThisThread(100);
		}
		
		GraphicsDevice->TestDraw();
	}
	
	delete GraphicsDevice;
	delete Window;
	
	return 0;
}