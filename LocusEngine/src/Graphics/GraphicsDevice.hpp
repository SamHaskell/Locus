#pragma once

#include "Base/Base.hpp"
#include "Core/DisplayManager.hpp"

namespace Locus
{
	constexpr u32 FRAMES_IN_FLIGHT = 2;
	
	class Window;
	class Swapchain;
	
	class GraphicsDevice : public Object
	{
	public:
		static GraphicsDevice* Create(const WindowHandle Window);
		virtual ~GraphicsDevice() = default;
		
		virtual void TestDraw() = 0;
	};
}