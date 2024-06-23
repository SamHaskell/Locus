#pragma once

#include "LVKCommon.hpp"

namespace Locus
{
	struct LVKPipelineFactory
	{
		LVKPipelineFactory() { Clear(); }
		
		void Clear();
		VkPipeline Create(VkDevice Device, VkRenderPass RenderPass);

		TArray<VkPipelineShaderStageCreateInfo> ShaderStages;
		
		VkPipelineColorBlendAttachmentState ColorBlendAttachment;
		VkPipelineVertexInputStateCreateInfo VertexInput;
		VkPipelineInputAssemblyStateCreateInfo InputAssembly;
		VkPipelineRasterizationStateCreateInfo Rasterizer;
		VkPipelineDepthStencilStateCreateInfo DepthStencil;
		VkPipelineMultisampleStateCreateInfo Multisampling;
		
		VkPipelineLayout Layout;
		
		bool bDynamicViewport = true;
		VkViewport Viewport;
		VkRect2D Scissor;
	};
}

namespace Locus::LVK
{
	VkShaderModule CreateShaderModule(VkDevice Device, const VkAllocationCallbacks* Allocator, const u8* Code, arch CodeSize);
};