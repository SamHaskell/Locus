#include "Window.hpp"

#include "Platform/LSDL/LSDLWindow.hpp"

namespace Locus
{
	Window* Window::Create(const WindowConfig& Config)
	{
		return new LSDLWindow(Config);
	}
}