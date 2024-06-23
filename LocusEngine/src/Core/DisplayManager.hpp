#pragma once

#include "Base/Base.hpp"
#include <vulkan/vulkan_core.h>

namespace Locus
{
	using WindowHandle = HandleType;
	constexpr u32 WINDOW_COUNT_MAX = 255; // Be real, if you are making more then you need to reevaluate some life decisions
	
	class DisplayManager : public Object, public Singleton<DisplayManager>
	{
	public:
		virtual WindowHandle CreateWindow(const char* Title, u32 Width, u32 Height) = 0;
		virtual void DestroyWindow(WindowHandle Window) = 0;
		
		virtual void* GetNativeWindowHandle(WindowHandle Window) = 0;
		virtual void GetWindowFramebufferSize(WindowHandle Window, u32& Width, u32& Height) = 0;
		
		virtual bool CreateVulkanSurface(WindowHandle Window, VkInstance Instance, VkSurfaceKHR& OutSurface) const = 0;
		
		virtual void PollEvents(bool& bShouldQuit) = 0;
	};
}