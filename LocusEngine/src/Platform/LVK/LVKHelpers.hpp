#pragma once

#include "LVKTypes.hpp"

#include "Core/Window.hpp"

using namespace Locus;

namespace Locus::LVK
{
	bool CreateInstance(VkInstance& OutInstance, const TArray<const char*>& RequiredExtensions, const TArray<const char*>& ValidationLayers, const VkAllocationCallbacks *Allocator = nullptr);
	void DestroyInstance(VkInstance& Instance);
		
	TArray<VkExtensionProperties> GetInstanceExtensionProperties();
	TArray<VkLayerProperties> GetInstanceLayerProperties();

	bool ChoosePhysicalDevice(VkInstance Instance, VkSurfaceKHR Surface, VkPhysicalDevice& OutPhysicalDevice, VkPhysicalDeviceFeatures& RequiredDeviceFeatures, const TArray<VkPhysicalDeviceType>& AllowedDeviceTypes, const TArray<const char*>& RequiredDeviceExtensions);
	
	bool CheckPhysicalDeviceFeatures(VkPhysicalDeviceFeatures& RequiredFeatures, VkPhysicalDeviceFeatures& PhysicalDeviceFeatures);
	bool CheckPhysicalDeviceSuitability(VkPhysicalDevice PhysicalDevice, VkSurfaceKHR Surface, VkPhysicalDeviceFeatures& RequiredFeatures, const TArray<VkPhysicalDeviceType>& AllowedDeviceTypes, const TArray<const char*>& RequiredDeviceExtensions);

	LVKQueueFamilyIndices FindPhysicalDeviceQueueFamilies(VkPhysicalDevice PhysicalDevice, VkSurfaceKHR Surface);
	LVKSwapchainSupportDetails QuerySwapchainSupport(VkSurfaceKHR Surface, VkPhysicalDevice PhysicalDevice);

	bool CreateLogicalDevice(VkPhysicalDevice PhysicalDevice, VkSurfaceKHR Surface, VkDevice& OutDevice, LVKQueueFamilyIndices& QueueFamilyIndices, VkPhysicalDeviceFeatures& RequiredFeatures, const TArray<const char*>& RequiredDeviceExtensions, const TArray<const char*>& ValidationLayers, const VkAllocationCallbacks *Allocator = nullptr);
	void DestroyDevice(VkDevice& Device, const VkAllocationCallbacks *Allocator = nullptr);
	
	bool CreateSurface(const Window* Window, VkInstance Instance, VkSurfaceKHR& OutSurface, const VkAllocationCallbacks* Allocator = nullptr);
	void DestroySurface(VkInstance sInstance, VkSurfaceKHR& sSurface, const VkAllocationCallbacks *Allocator = nullptr);
	
	VkSurfaceFormatKHR ChooseSwapchainSurfaceFormat(const TArray<VkSurfaceFormatKHR>& AvailableFormats);
	VkPresentModeKHR ChooseSwapchainPresentMode(const TArray<VkPresentModeKHR>& AvailablePresentModes);
	VkExtent2D ChooseSwapchainExtent(const Window* Window, const VkSurfaceCapabilitiesKHR& Capabilities);
	
	VkRenderingAttachmentInfo RenderingAttachmentInfo(VkImageView View, VkClearValue* Clear, VkImageLayout Layout);
}