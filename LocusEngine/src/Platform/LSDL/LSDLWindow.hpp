#pragma once

#include "Core/Window.hpp"

struct SDL_Window;

namespace Locus
{	
	class LSDLWindow : public Window
	{
	public:
			LSDLWindow(const WindowConfig& Config);
			~LSDLWindow();
			
			void* GetNativeHandle() const override;
			bool CreateVulkanSurface(VkInstance Instance, VkSurfaceKHR& OutSurface) const override;
			void GetFramebufferSize(u32& Width, u32& Height) const override;
			
	protected:
			SDL_Window* WindowHandle;
	};
};