#pragma once

#include "Base/Base.hpp"
#include "vulkan/vulkan_core.h"

namespace Locus
{
	struct WindowConfig
	{
		const char* Title = "Locus";
		u32 Width = 0;
		u32 Height = 0;
	};
	
	class Window 
	{
	public:
		static Window* Create(const WindowConfig& Config);
		virtual ~Window() = default;
		
		virtual void* GetNativeHandle() const = 0;
		virtual bool CreateVulkanSurface(VkInstance Instance, VkSurfaceKHR& OutSurface) const = 0;
		virtual void GetFramebufferSize(u32& Width, u32& Height) const = 0;
	};
}