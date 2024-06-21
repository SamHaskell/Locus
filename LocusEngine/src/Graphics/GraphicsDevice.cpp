#include "GraphicsDevice.hpp"

#include "Core/Window.hpp"
#include "Platform/LVK/LVKGraphicsDevice.hpp"

namespace Locus
{
	GraphicsDevice* GraphicsDevice::Create(const Window* Window)
	{
		return new LVKGraphicsDevice(Window);
	}
}