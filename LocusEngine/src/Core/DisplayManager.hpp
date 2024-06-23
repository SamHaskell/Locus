#pragma once

#include "Base/Base.hpp"
#include "Base/Handles.hpp"

namespace Locus
{
	using WindowHandle = HandleType;
	
	class DisplayManager : public Object, public Singleton<DisplayManager>
	{
	public:
		virtual WindowHandle CreateWindow() = 0;
		virtual void DestroyWindow(WindowHandle Window) = 0;
	private:
		
	};
}