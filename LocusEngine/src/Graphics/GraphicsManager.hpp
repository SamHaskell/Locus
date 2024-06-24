#pragma once

#include "Base/Base.hpp"
#include "Base/Handles.hpp"
#include "Core/DisplayManager.hpp"

#include <functional>

namespace Locus
{
	using RenderContextHandle = HandleType;
	
	class GraphicsManager : public Object, public Singleton<GraphicsManager>
	{
	public:
		virtual	RenderContextHandle CreateRenderContext(const WindowHandle Window) = 0;
		virtual void TestDraw() = 0;
	};
};