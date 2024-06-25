#pragma once

#include "Base/Base.hpp"
#include "Base/Handles.hpp"
#include "Core/DisplayManager.hpp"
#include "imgui.h"

namespace Locus
{
	class GraphicsManager : public Object, public Singleton<GraphicsManager>
	{
	public:
		virtual	RenderContextHandle CreateRenderContext(const WindowHandle Window) = 0;
		virtual void DestroyRenderContext(RenderContextHandle RenderContext) = 0;
		
		virtual void BeginFrame(RenderContextHandle RenderContext) = 0;
		virtual void EndFrame(RenderContextHandle RenderContext) = 0;
		
		virtual void BeginFrameImGui() = 0;
		virtual void EndFrameImGui() = 0;
		
		virtual void TestDraw(RenderContextHandle RenderContext) = 0;
		
		inline RenderContextHandle GetActiveRenderContext() { return m_ActiveRenderContext; }
		virtual ImGuiContext* GetImGuiContext(RenderContextHandle RenderContext) = 0;
		
	protected:
		RenderContextHandle m_ActiveRenderContext = HANDLE_INVALID;
	};
};