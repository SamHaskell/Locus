#include "LVKPipeline.hpp"
#include "Platform/LVK/LVKTypes.hpp"
#include <vulkan/vulkan_core.h>

namespace Locus
{
	void LVKPipelineFactory::Clear()
	{
		ShaderStages.Clear();
		
		ColorBlendAttachment = {
			.blendEnable = VK_FALSE,
			.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
		};
		
		VertexInput = { .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
		
		InputAssembly = { 
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			.primitiveRestartEnable = VK_FALSE,
	 	};
		
		Rasterizer = { 
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
	 		.polygonMode = VK_POLYGON_MODE_FILL,
			.cullMode = VK_CULL_MODE_BACK_BIT,
			.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
			.lineWidth = 1.0f,
		};
		
		DepthStencil = { 
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			.depthTestEnable = VK_FALSE,
			.depthWriteEnable = VK_FALSE,
			.depthCompareOp = VK_COMPARE_OP_NEVER,
			.depthBoundsTestEnable = VK_FALSE,
			.stencilTestEnable = VK_FALSE,
			.front = {},
			.back = {},
			.minDepthBounds = 0.0f,
			.maxDepthBounds = 1.0f,
		};
		
		Multisampling = { 
			.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
			.sampleShadingEnable = VK_FALSE,
			.minSampleShading = 1.0f,
			.pSampleMask = nullptr,
			.alphaToCoverageEnable = VK_FALSE,
			.alphaToOneEnable = VK_FALSE
		};
		
		Layout = VK_NULL_HANDLE;
		
		bDynamicViewport = true;
		Viewport = {};
		Scissor = {};
	}
	
	VkPipeline LVKPipelineFactory::Create(VkDevice Device, VkRenderPass RenderPass)
	{
		VkPipelineViewportStateCreateInfo ViewportState = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.pNext = nullptr,
			.viewportCount = 1,
			.pViewports = nullptr,
			.scissorCount = 1,
			.pScissors = nullptr,
		};
		
		VkPipelineDynamicStateCreateInfo DynamicState = { .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
		VkDynamicState State[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		
		if (bDynamicViewport)
		{
			DynamicState.pDynamicStates = State;
			DynamicState.dynamicStateCount = 2;
		}
		else 
		{
			ViewportState.pViewports = &Viewport;
			ViewportState.pScissors = &Scissor;
		}
		
		VkPipelineColorBlendStateCreateInfo ColorBlending = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.pNext = nullptr,
			.logicOpEnable = VK_FALSE,
			.logicOp = VK_LOGIC_OP_COPY,
			.attachmentCount = 1,
			.pAttachments = &ColorBlendAttachment,
		};
		
		VkGraphicsPipelineCreateInfo PipelineInfo = {
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.pNext = nullptr,
			.stageCount = static_cast<u32>(ShaderStages.Length()),
			.pStages = ShaderStages.Data(),
			.pVertexInputState = &VertexInput,
			.pInputAssemblyState = &InputAssembly,
			.pViewportState = &ViewportState,
			.pRasterizationState = &Rasterizer,
			.pMultisampleState = &Multisampling,
			.pDepthStencilState = &DepthStencil,
			.pColorBlendState = &ColorBlending,
			.pDynamicState = &DynamicState,
			.layout = Layout,
			.renderPass = RenderPass,
		};
		
		VkPipeline Pipeline;
		VkResult Result = vkCreateGraphicsPipelines(Device, VK_NULL_HANDLE, 1, &PipelineInfo, nullptr, &Pipeline);
		if (Result != VK_SUCCESS)
		{
			LLOG(Vulkan, Error, "Failed to create pipeline object: %s", string_VkResult(Result));
			return VK_NULL_HANDLE;
		}
		return Pipeline;
	}
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