#pragma once

#include "LVKCommon.hpp"

namespace Locus 
{
	struct LVKConfig
	{
		LVKConfig() = default;
		
		TArray<const char*> RequiredExtensions;
		TArray<const char*> RequiredDeviceExtensions;
		TArray<const char*> ValidationLayers;
		VkPhysicalDeviceFeatures RequiredDeviceFeatures;
		TArray<VkPhysicalDeviceType> AllowedDeviceTypes;
	};

	struct LVKQueueFamilyIndices
	{
		bool GraphicsFamilyPresent = false;
		u32 GraphicsFamilyIndex = 0;
		
		bool PresentFamilyPresent = false;
		u32 PresentFamilyIndex = 0;
		
		inline bool IsComplete();
	};
	
	bool LVKQueueFamilyIndices::IsComplete() 
	{
		return GraphicsFamilyPresent && PresentFamilyPresent;
	}
	
	struct LVKSwapchainSupportDetails
	{
		VkSurfaceCapabilitiesKHR Capabilities;
		TArray<VkSurfaceFormatKHR> Formats;
		TArray<VkPresentModeKHR> PresentModes;
		
		inline bool IsValid();
	};
	
	bool LVKSwapchainSupportDetails::IsValid()
	{
		return !Formats.Empty() && !PresentModes.Empty();
	}
	
	struct LVKShaderStage
	{
		VkShaderModule Module = VK_NULL_HANDLE;
		VkShaderStageFlags StageFlags = 0;
		const char* EntryPoint = "main";
	};
}