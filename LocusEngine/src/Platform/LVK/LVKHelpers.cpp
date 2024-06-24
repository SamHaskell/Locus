#include "LVKHelpers.hpp"

#include "Base/Base.hpp"
#include "Core/DisplayManager.hpp"
#include "Math/Numerics.hpp"
#include <set>
#include <vulkan/vulkan_core.h>

using namespace Locus;

bool Locus::LVK::CreateInstance(VkInstance& OutInstance, const TArray<const char*>& RequiredExtensions, const TArray<const char*>& ValidationLayers, const VkAllocationCallbacks *Allocator)
{
	VkApplicationInfo AppInfo = {};
	AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	AppInfo.pApplicationName = "Locus Engine";
	AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	AppInfo.pEngineName = "Locus Engine";
	AppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	AppInfo.apiVersion = VK_API_VERSION_1_3;

	VkInstanceCreateInfo InstanceCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &AppInfo,
		.enabledLayerCount = static_cast<u32>(ValidationLayers.Length()),
		.ppEnabledLayerNames = ValidationLayers.Data(),
		.enabledExtensionCount = static_cast<u32>(RequiredExtensions.Length()),
		.ppEnabledExtensionNames = RequiredExtensions.Data(),
	};

#if LOCUS_PLATFORM_MACOS
	InstanceCreateInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

	VkResult Result = vkCreateInstance(&InstanceCreateInfo, Allocator, &OutInstance);
	VK_CHECK_RESULT(Result);
	return (Result == VK_SUCCESS);
}

void Locus::LVK::DestroyInstance(VkInstance& Instance)
{
	vkDestroyInstance(Instance, nullptr);
}

TArray<VkExtensionProperties> Locus::LVK::GetInstanceExtensionProperties()
{
	u32 ExtensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionCount, nullptr);
	TArray<VkExtensionProperties> ExtensionProperties(ExtensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionCount, ExtensionProperties.Data());
	return ExtensionProperties;
}

TArray<VkLayerProperties> Locus::LVK::GetInstanceLayerProperties()
{
	u32 LayerCount = 0;
	vkEnumerateInstanceLayerProperties(&LayerCount, nullptr);
	TArray<VkLayerProperties> LayerProperties(LayerCount);
	vkEnumerateInstanceLayerProperties(&LayerCount, LayerProperties.Data());
	return LayerProperties;
}

bool Locus::LVK::ChoosePhysicalDevice(VkInstance Instance, VkSurfaceKHR Surface, VkPhysicalDevice& OutPhysicalDevice, VkPhysicalDeviceFeatures& RequiredDeviceFeatures, const TArray<VkPhysicalDeviceType>& AllowedDeviceTypes, const TArray<const char*>& RequiredDeviceExtensions)
{
	VK_CHECK_HANDLE(Instance);
	VK_CHECK_HANDLE(Surface);
	
	u32 PhysicalDeviceCount = 0;
	vkEnumeratePhysicalDevices(Instance, &PhysicalDeviceCount, nullptr);
	if (PhysicalDeviceCount <= 0)
	{
		LLOG(Vulkan, Error, "No physical devices found!");
		return false;
	}
	
	TArray<VkPhysicalDevice> PhysicalDevices(PhysicalDeviceCount);
	vkEnumeratePhysicalDevices(Instance, &PhysicalDeviceCount, PhysicalDevices.Data());
	
	for (i32 i = 0; i < PhysicalDevices.Length(); i++)
	{
		VkPhysicalDevice Device = PhysicalDevices[i];
		if (LVK::CheckPhysicalDeviceSuitability(Device, Surface, RequiredDeviceFeatures, AllowedDeviceTypes, RequiredDeviceExtensions))
		{
			OutPhysicalDevice = Device;
			return true;
		}
	}
	
	LLOG(Vulkan, Error, "No physical devices found that are suitable!");
	return false;
}

bool Locus::LVK::CheckPhysicalDeviceFeatures(VkPhysicalDeviceFeatures& RequiredFeatures, VkPhysicalDeviceFeatures& PhysicalDeviceFeatures)
{
	if (RequiredFeatures.robustBufferAccess && !PhysicalDeviceFeatures.robustBufferAccess) return false;
	if (RequiredFeatures.fullDrawIndexUint32 && !PhysicalDeviceFeatures.fullDrawIndexUint32) return false;
	if (RequiredFeatures.imageCubeArray && !PhysicalDeviceFeatures.imageCubeArray) return false;
	if (RequiredFeatures.independentBlend && !PhysicalDeviceFeatures.independentBlend) return false;
	if (RequiredFeatures.geometryShader && !PhysicalDeviceFeatures.geometryShader) return false;
	if (RequiredFeatures.tessellationShader && !PhysicalDeviceFeatures.tessellationShader) return false;
	if (RequiredFeatures.sampleRateShading && !PhysicalDeviceFeatures.sampleRateShading) return false;
	if (RequiredFeatures.dualSrcBlend && !PhysicalDeviceFeatures.dualSrcBlend) return false;
	if (RequiredFeatures.logicOp && !PhysicalDeviceFeatures.logicOp) return false;
	if (RequiredFeatures.multiDrawIndirect && !PhysicalDeviceFeatures.multiDrawIndirect) return false;
	if (RequiredFeatures.drawIndirectFirstInstance && !PhysicalDeviceFeatures.drawIndirectFirstInstance) return false;
	if (RequiredFeatures.depthClamp && !PhysicalDeviceFeatures.depthClamp) return false;
	if (RequiredFeatures.depthBiasClamp && !PhysicalDeviceFeatures.depthBiasClamp) return false;
	if (RequiredFeatures.fillModeNonSolid && !PhysicalDeviceFeatures.fillModeNonSolid) return false;
	if (RequiredFeatures.depthBounds && !PhysicalDeviceFeatures.depthBounds) return false;
	if (RequiredFeatures.wideLines && !PhysicalDeviceFeatures.wideLines) return false;
	if (RequiredFeatures.largePoints && !PhysicalDeviceFeatures.largePoints) return false;
	if (RequiredFeatures.alphaToOne && !PhysicalDeviceFeatures.alphaToOne) return false;
	if (RequiredFeatures.multiViewport && !PhysicalDeviceFeatures.multiViewport) return false;
	if (RequiredFeatures.samplerAnisotropy && !PhysicalDeviceFeatures.samplerAnisotropy) return false;
	if (RequiredFeatures.textureCompressionETC2 && !PhysicalDeviceFeatures.textureCompressionETC2) return false;
	if (RequiredFeatures.textureCompressionASTC_LDR && !PhysicalDeviceFeatures.textureCompressionASTC_LDR) return false;
	if (RequiredFeatures.textureCompressionBC && !PhysicalDeviceFeatures.textureCompressionBC) return false;
	if (RequiredFeatures.occlusionQueryPrecise && !PhysicalDeviceFeatures.occlusionQueryPrecise) return false;
	if (RequiredFeatures.pipelineStatisticsQuery && !PhysicalDeviceFeatures.pipelineStatisticsQuery) return false;
	if (RequiredFeatures.vertexPipelineStoresAndAtomics && !PhysicalDeviceFeatures.vertexPipelineStoresAndAtomics) return false;
	if (RequiredFeatures.fragmentStoresAndAtomics && !PhysicalDeviceFeatures.fragmentStoresAndAtomics) return false;
	if (RequiredFeatures.shaderTessellationAndGeometryPointSize && !PhysicalDeviceFeatures.shaderTessellationAndGeometryPointSize) return false;
	if (RequiredFeatures.shaderImageGatherExtended && !PhysicalDeviceFeatures.shaderImageGatherExtended) return false;
	if (RequiredFeatures.shaderStorageImageExtendedFormats && !PhysicalDeviceFeatures.shaderStorageImageExtendedFormats) return false;
	if (RequiredFeatures.shaderStorageImageMultisample && !PhysicalDeviceFeatures.shaderStorageImageMultisample) return false;
	if (RequiredFeatures.shaderStorageImageReadWithoutFormat && !PhysicalDeviceFeatures.shaderStorageImageReadWithoutFormat) return false;
	if (RequiredFeatures.shaderStorageImageWriteWithoutFormat && !PhysicalDeviceFeatures.shaderStorageImageWriteWithoutFormat) return false;
	if (RequiredFeatures.shaderUniformBufferArrayDynamicIndexing && !PhysicalDeviceFeatures.shaderUniformBufferArrayDynamicIndexing) return false;
	if (RequiredFeatures.shaderSampledImageArrayDynamicIndexing && !PhysicalDeviceFeatures.shaderSampledImageArrayDynamicIndexing) return false;
	if (RequiredFeatures.shaderStorageBufferArrayDynamicIndexing && !PhysicalDeviceFeatures.shaderStorageBufferArrayDynamicIndexing) return false;
	if (RequiredFeatures.shaderStorageImageArrayDynamicIndexing && !PhysicalDeviceFeatures.shaderStorageImageArrayDynamicIndexing) return false;
	if (RequiredFeatures.shaderClipDistance && !PhysicalDeviceFeatures.shaderClipDistance) return false;
	if (RequiredFeatures.shaderCullDistance && !PhysicalDeviceFeatures.shaderCullDistance) return false;
	if (RequiredFeatures.shaderFloat64 && !PhysicalDeviceFeatures.shaderFloat64) return false;
	if (RequiredFeatures.shaderInt64 && !PhysicalDeviceFeatures.shaderInt64) return false;
	if (RequiredFeatures.shaderInt16 && !PhysicalDeviceFeatures.shaderInt16) return false;
	if (RequiredFeatures.shaderResourceResidency && !PhysicalDeviceFeatures.shaderResourceResidency) return false;
	if (RequiredFeatures.shaderResourceMinLod && !PhysicalDeviceFeatures.shaderResourceMinLod) return false;
	if (RequiredFeatures.sparseBinding && !PhysicalDeviceFeatures.sparseBinding) return false;
	if (RequiredFeatures.sparseResidencyBuffer && !PhysicalDeviceFeatures.sparseResidencyBuffer) return false;
	if (RequiredFeatures.sparseResidencyImage2D && !PhysicalDeviceFeatures.sparseResidencyImage2D) return false;
	if (RequiredFeatures.sparseResidencyImage3D && !PhysicalDeviceFeatures.sparseResidencyImage3D) return false;
	if (RequiredFeatures.sparseResidency2Samples && !PhysicalDeviceFeatures.sparseResidency2Samples) return false;
	if (RequiredFeatures.sparseResidency4Samples && !PhysicalDeviceFeatures.sparseResidency4Samples) return false;
	if (RequiredFeatures.sparseResidency8Samples && !PhysicalDeviceFeatures.sparseResidency8Samples) return false;
	if (RequiredFeatures.sparseResidency16Samples && !PhysicalDeviceFeatures.sparseResidency16Samples) return false;
	if (RequiredFeatures.sparseResidencyAliased && !PhysicalDeviceFeatures.sparseResidencyAliased) return false;
	if (RequiredFeatures.variableMultisampleRate && !PhysicalDeviceFeatures.variableMultisampleRate) return false;
	if (RequiredFeatures.inheritedQueries && !PhysicalDeviceFeatures.inheritedQueries) return false;
	return true;
}

bool Locus::LVK::CheckPhysicalDeviceSuitability(VkPhysicalDevice PhysicalDevice, VkSurfaceKHR Surface, VkPhysicalDeviceFeatures& RequiredFeatures, const TArray<VkPhysicalDeviceType>& AllowedDeviceTypes, const TArray<const char*>& RequiredDeviceExtensions)
{
	VkPhysicalDeviceFeatures PhysicalDeviceFeatures;
	VkPhysicalDeviceProperties PhysicalDeviceProperties;
	vkGetPhysicalDeviceFeatures(PhysicalDevice, &PhysicalDeviceFeatures);
	vkGetPhysicalDeviceProperties(PhysicalDevice, &PhysicalDeviceProperties);
	
	bool bFeatureComplete = CheckPhysicalDeviceFeatures(RequiredFeatures, PhysicalDeviceFeatures);
	bool bDeviceTypePermitted = false;
	for (i32 i = 0; i < AllowedDeviceTypes.Length(); i++)
	{
		const auto& Type = AllowedDeviceTypes.GetElement(i);
		if (Type == PhysicalDeviceProperties.deviceType)
		{
			bDeviceTypePermitted = true;
		}
	}
	
	LVKQueueFamilyIndices QueueFamilyIndices = LVK::FindPhysicalDeviceQueueFamilies(PhysicalDevice, Surface);
	bool bQueueTypesPresent = QueueFamilyIndices.IsComplete();
	
	bool bExtensionsAvailable = true;
	
	u32 DeviceExtensionCount;
	vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &DeviceExtensionCount, nullptr);
	TArray<VkExtensionProperties> AvailableDeviceExtensions(DeviceExtensionCount);
	vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &DeviceExtensionCount, AvailableDeviceExtensions.Data());
	
	for (i32 i = 0; i < RequiredDeviceExtensions.Length(); i++)
	{
		const auto RequiredExtension = RequiredDeviceExtensions[i];
		
		bool bFound = false;
		for (i32 j = 0; j < AvailableDeviceExtensions.Length(); j++)
		{
			const auto& AvailableExtension = AvailableDeviceExtensions.GetElement(j);
			if (strcmp(AvailableExtension.extensionName, RequiredExtension) == 0)
			{
				bFound = true;
				break;
			}
		}
		
		if (!bFound)
		{
			LLOG(Vulkan, Info, "Extensions Unavailable.");
			bExtensionsAvailable = false;
			break;
		}
	}
	
	bool bSwapchainAdequate = false;
	if (bExtensionsAvailable)
	{
		LVKSwapchainSupportDetails SwapchainSupportDetails = QuerySwapchainSupport(Surface, PhysicalDevice);
		bSwapchainAdequate = SwapchainSupportDetails.IsValid();
	}
	
	if (!bSwapchainAdequate)
	{
		LLOG(Vulkan, Info, "Swapchain inadequate.");
	}

	return bFeatureComplete && bDeviceTypePermitted && bQueueTypesPresent && bExtensionsAvailable && bSwapchainAdequate;
}

LVKQueueFamilyIndices Locus::LVK::FindPhysicalDeviceQueueFamilies(VkPhysicalDevice PhysicalDevice, VkSurfaceKHR Surface)
{
	LVKQueueFamilyIndices QueueFamilyIndices;
	
	u32 QueueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, nullptr);
	TArray<VkQueueFamilyProperties> QueueFamilyProperties(QueueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, QueueFamilyProperties.Data());
	
	for (i32 i = 0; i < QueueFamilyProperties.Length(); i++)
	{
		const auto& QueueFamily = QueueFamilyProperties.GetElement(i);
		if ((QueueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) && !QueueFamilyIndices.GraphicsFamilyPresent)
		{
			QueueFamilyIndices.GraphicsFamilyPresent = true;
			QueueFamilyIndices.GraphicsFamilyIndex = i;
		}
	}
	
	VkBool32 PresentSupport = false;
	vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, QueueFamilyIndices.PresentFamilyIndex, Surface, &PresentSupport);
	if (PresentSupport)
	{
		QueueFamilyIndices.PresentFamilyPresent = true;
	}
	
	return QueueFamilyIndices;
}

LVKSwapchainSupportDetails Locus::LVK::QuerySwapchainSupport(VkSurfaceKHR Surface, VkPhysicalDevice PhysicalDevice)
{
	LVKSwapchainSupportDetails SwapchainSupportDetails;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice, Surface, &SwapchainSupportDetails.Capabilities);
	
	u32 FormatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &FormatCount, nullptr);
	if (FormatCount != 0)
	{
		SwapchainSupportDetails.Formats.Reserve(FormatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &FormatCount, SwapchainSupportDetails.Formats.Data());
	}
	
	u32 PresentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, &PresentModeCount, nullptr);
	if (PresentModeCount != 0)
	{
		SwapchainSupportDetails.PresentModes.Reserve(PresentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, &PresentModeCount, SwapchainSupportDetails.PresentModes.Data());
	}
	
	return SwapchainSupportDetails;
}

bool Locus::LVK::CreateLogicalDevice(VkPhysicalDevice PhysicalDevice, VkSurfaceKHR Surface, VkDevice& OutDevice, LVKQueueFamilyIndices& QueueFamilyIndices, VkPhysicalDeviceFeatures& RequiredFeatures, const TArray<const char*>& RequiredDeviceExtensions, const TArray<const char*>& ValidationLayers, const VkAllocationCallbacks *Allocator)
{
	VK_CHECK_HANDLE(PhysicalDevice);
	VK_CHECK_HANDLE(Surface);
	
	QueueFamilyIndices = LVK::FindPhysicalDeviceQueueFamilies(PhysicalDevice, Surface);
	
	std::set<u32> UniqueQueueFamilyIndices = {
		QueueFamilyIndices.GraphicsFamilyIndex,
		QueueFamilyIndices.PresentFamilyIndex
	};
	
	f32 QueuePriorities[1] = {1.0f};
	TArray<VkDeviceQueueCreateInfo> QueueCreateInfos;
	for (u32 QueueFamilyIndex : UniqueQueueFamilyIndices)
	{
		QueueCreateInfos.Push({
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueFamilyIndex = QueueFamilyIndices.GraphicsFamilyIndex,
			.queueCount = 1,
			.pQueuePriorities = &QueuePriorities[0]
		});
	}
	
	VkDeviceCreateInfo DeviceCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pNext = nullptr,
		.queueCreateInfoCount = (u32)QueueCreateInfos.Length(),
		.pQueueCreateInfos = QueueCreateInfos.Data(),
		.enabledLayerCount = (u32)ValidationLayers.Length(),
		.ppEnabledLayerNames = ValidationLayers.Data(),
		.enabledExtensionCount = (u32)RequiredDeviceExtensions.Length(),
		.ppEnabledExtensionNames = RequiredDeviceExtensions.Data(),
		.pEnabledFeatures = &RequiredFeatures,
	};
	
	VkResult Result = vkCreateDevice(PhysicalDevice, &DeviceCreateInfo, Allocator, &OutDevice);
	VK_CHECK_RESULT(Result);
	
	return (Result == VK_SUCCESS);
}

void Locus::LVK::DestroyDevice(VkDevice& Device, const VkAllocationCallbacks *Allocator)
{
	vkDestroyDevice(Device, Allocator);
}

bool Locus::LVK::CreateSurface(const WindowHandle Window, VkInstance Instance, VkSurfaceKHR &OutSurface, const VkAllocationCallbacks *Allocator)
{
	VK_CHECK_HANDLE(Instance);
	return DisplayManager::Get().CreateVulkanSurface(Window, Instance, OutSurface);
}

void Locus::LVK::DestroySurface(VkInstance Instance, VkSurfaceKHR& Surface, const VkAllocationCallbacks *Allocator)
{
	vkDestroySurfaceKHR(Instance, Surface, Allocator);
}

VkSurfaceFormatKHR Locus::LVK::ChooseSwapchainSurfaceFormat(const TArray<VkSurfaceFormatKHR>& AvailableFormats)
{
	for (i32 i = 0; i < AvailableFormats.Length(); i++)
	{
		const auto& AvailableFormat = AvailableFormats.GetElement(i);
		if (AvailableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && AvailableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return AvailableFormat;
		}
	}
	
	return AvailableFormats.GetElement(0);
}

VkPresentModeKHR Locus::LVK::ChooseSwapchainPresentMode(const TArray<VkPresentModeKHR>& AvailablePresentModes)
{
	for (i32 i = 0; i < AvailablePresentModes.Length(); i++)
	{
		const auto& AvailablePresentMode = AvailablePresentModes.GetElement(i);
		if (AvailablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return VK_PRESENT_MODE_MAILBOX_KHR;
		}
	}
	
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Locus::LVK::ChooseSwapchainExtent(const WindowHandle Window, const VkSurfaceCapabilitiesKHR& Capabilities)
{
	if (Capabilities.currentExtent.width != UINT32_MAX)
	{
		return Capabilities.currentExtent;
	} 
	else 
	{
		u32 Width, Height;
		DisplayManager::Get().GetWindowFramebufferSize(Window, Width, Height);
		
		VkExtent2D Actual = {
			.width = Width,
			.height = Height
		};
		
		Actual.width = Math::Clamp(Actual.width, Capabilities.minImageExtent.width, Capabilities.maxImageExtent.width);
		Actual.width = Math::Clamp(Actual.height, Capabilities.minImageExtent.height, Capabilities.maxImageExtent.height);
	
		return Actual;
	}
	
}

VkRenderingAttachmentInfo Locus::LVK::RenderingAttachmentInfo(VkImageView View, VkClearValue* Clear, VkImageLayout Layout)
{
	return {
		.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
		.pNext = nullptr,
		.imageView = View,
		.imageLayout = Layout,
		.loadOp = Clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.clearValue = Clear ? *Clear : VkClearValue{}
	};
}

VkShaderModule Locus::LVK::CreateShaderModule(VkDevice Device, const VkAllocationCallbacks* Allocator, const u8* Code, arch CodeSize)
{
	VkShaderModuleCreateInfo ShaderModuleCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = CodeSize,
		.pCode = reinterpret_cast<const u32*>(Code)
	};
	
	VkShaderModule ShaderModule;
	LCheck(vkCreateShaderModule(Device, &ShaderModuleCreateInfo, Allocator, &ShaderModule) == VK_SUCCESS);
	return ShaderModule;
}