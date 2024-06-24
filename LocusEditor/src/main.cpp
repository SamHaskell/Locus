#include "Graphics/GraphicsManager.hpp"
#include "Locus.hpp"
#include "Platform/Platform.hpp"

using namespace Locus;

static void TestRun()
{
	WindowHandle Window = DisplayManager::Get().CreateWindow("Window", 1280, 720);
	
	// Note the handle is invalid right now, we need to properly implement contexts.
	// This is just to demonstrate the API we want to expose.
	RenderContextHandle RenderContext = GraphicsManager::Get().CreateRenderContext(Window);
	
	Clock Clock;
	Clock.Start();
	
	bool bShouldQuit = false;
	while (!bShouldQuit)
	{
		DisplayManager::Get().PollEvents(bShouldQuit);
		GraphicsManager::Get().TestDraw();		
		
		f64 DeltaTime = Clock.GetElapsedSeconds();
		Clock.Reset(true);
	}
}

i32 main(i32 argc, char* argv[])
{	
	Engine::Get().Init();
	TestRun();
	Engine::Get().Shutdown();
	return 0;
}