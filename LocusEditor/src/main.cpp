#include "Core/DisplayManager.hpp"
#include "Graphics/GraphicsManager.hpp"
#include "Locus.hpp"
#include "Platform/Platform.hpp"
#include "imgui.h"

using namespace Locus;

f64 s_DeltaTime = 0.0;

static void Draw(RenderContextHandle RenderContext)
{
	GraphicsManager::Get().TestDraw(RenderContext);
}

static void DrawGUI()
{
	if(ImGui::Begin("Debug"))
	{
		ImGui::Text("Frame Time: %.2lfms", s_DeltaTime * 1000.0);
	}
	ImGui::End();
}

i32 main(i32 argc, char* argv[])
{	
	Engine::Get().Init();

	WindowHandle Window = DisplayManager::Get().CreateWindow("Window", 1280, 720);
	RenderContextHandle RenderContext = DisplayManager::Get().GetWindowRenderContext(Window);
	
	WindowHandle Window2 = DisplayManager::Get().CreateWindow("Window", 1280, 720);
	RenderContextHandle RenderContext2 = DisplayManager::Get().GetWindowRenderContext(Window2);
	
	Clock Clock;
	Clock.Start();
	
	GraphicsManager& GraphicsManager = GraphicsManager::Get();
	
	bool bShouldQuit = false;
	while (!bShouldQuit)
	{
		s_DeltaTime = Clock.GetElapsedSeconds();
		Clock.Reset(true);
		
		DisplayManager::Get().PollEvents(bShouldQuit);
		
		GraphicsManager.BeginFrame(RenderContext);
		{
			Draw(RenderContext);
			{
				GraphicsManager.BeginFrameImGui();
				DrawGUI();
				GraphicsManager.EndFrameImGui();			
			}
		}	
		GraphicsManager.EndFrame(RenderContext);

		GraphicsManager.BeginFrame(RenderContext2);
		{
			Draw(RenderContext2);
			{
				GraphicsManager.BeginFrameImGui();
				DrawGUI();
				GraphicsManager.EndFrameImGui();			
			}
		}	
		GraphicsManager.EndFrame(RenderContext2);
	}
	
	Engine::Get().Shutdown();
	return 0;
}