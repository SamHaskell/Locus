#pragma once

#include "Base/Base.hpp"
#include "Core/DisplayManager.hpp"

namespace Locus
{
	class GraphicsManager : public Object, public Singleton<GraphicsManager>
	{
	public:
		virtual void Init(const WindowHandle Window) = 0;
		virtual void TestDraw() = 0;
	};
};