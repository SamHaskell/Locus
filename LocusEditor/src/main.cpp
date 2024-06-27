#include "Locus.hpp"
#include "Platform/Platform.hpp"


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
		if (ImGui::Button("Make Window!"))
		{
			WindowHandle Handle = DisplayManager::Get().CreateWindow("Aghh", 800, 600);
		}
	}
	ImGui::End();
}

i32 main(i32 argc, char* argv[])
{	
	Engine::Get().Init();

	WindowHandle Window = DisplayManager::Get().CreateWindow("Window", 1280, 720);
	RenderContextHandle RenderContext = DisplayManager::Get().GetWindowRenderContext(Window);
	
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
	}
	
	Engine::Get().Shutdown();
	return 0;
}