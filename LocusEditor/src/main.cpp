#include "Graphics/GraphicsManager.hpp"
#include "Locus.hpp"

using namespace Locus;

static void TestRun()
{
	WindowHandle Window = DisplayManager::Get().CreateWindow("Window", 1280, 720);
	GraphicsManager::Get().Init(Window);
	
	bool bShouldQuit = false;
	while (!bShouldQuit)
	{
		DisplayManager::Get().PollEvents(bShouldQuit);
		GraphicsManager::Get().TestDraw();
	}
}

i32 main(i32 argc, char* argv[])
{	
	Engine::Get().Init();
	TestRun();
	Engine::Get().Shutdown();
	return 0;
}