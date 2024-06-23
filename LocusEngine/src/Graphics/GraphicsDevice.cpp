#include "GraphicsDevice.hpp"

#include "Platform/LVK/LVKGraphicsDevice.hpp"

namespace Locus
{
	GraphicsDevice* GraphicsDevice::Create(const WindowHandle Window)
	{
		return new LVKGraphicsDevice(Window);
	}
}