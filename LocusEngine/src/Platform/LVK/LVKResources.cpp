#include "LVKResources.hpp"
#include "LVKTypes.hpp"

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
	
	VkImageCreateInfo LVKImage::CreateInfo(VkFormat Format, VkImageUsageFlags UsageFlags, VkExtent3D Extent)
	{
		return {
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.pNext = nullptr,
			.imageType = VK_IMAGE_TYPE_2D,
			.format = Format,
			.extent = Extent,
			.mipLevels = 1,
			.arrayLayers = 1,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.tiling = VK_IMAGE_TILING_OPTIMAL,
			.usage = UsageFlags,
		};
	}
	
	VkImageViewCreateInfo LVKImage::ViewCreateInfo(VkFormat Format, VkImage Image, VkImageAspectFlags AspectFlags)
	{
		return {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.pNext = nullptr,
			.image = Image,
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = Format,
			.subresourceRange = {
				.aspectMask = AspectFlags,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1,
			},
		};
	}
	
	VkImageMemoryBarrier LVKImage::MemoryBarrier(VkImage Image, VkImageLayout Initial, VkImageLayout Final, u32 InitialQueue, u32 FinalQueue)
	{
		VkImageAspectFlags AspectMask = (Final == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
		return {
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.pNext = nullptr,
			.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT,
			.dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT,
			.oldLayout = Initial,
			.newLayout = Final,
			.srcQueueFamilyIndex = InitialQueue,
			.dstQueueFamilyIndex = FinalQueue,
			.image = Image,
			.subresourceRange = {
				.aspectMask = AspectMask,
				.baseMipLevel = 0,
				.levelCount = VK_REMAINING_MIP_LEVELS,
				.baseArrayLayer = 0,
				.layerCount = VK_REMAINING_ARRAY_LAYERS
			},
		};
	}
	
	void LVKImage::TransitionLazy(VkCommandBuffer Cmd, const VkImageMemoryBarrier& ImageBarrier)
	{
		vkCmdPipelineBarrier(
			Cmd, 
			VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
			0,
			0, nullptr,
			0, nullptr,
			1, &ImageBarrier
		);	
	}
	
	void LVKImage::Blit(VkCommandBuffer Cmd, VkImage Src, VkImage Dst, VkExtent2D SrcExtent, VkExtent2D DstExtent)
	{
		VkImageBlit BlitRegion = {
			.srcSubresource = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.mipLevel = 0,
				.baseArrayLayer = 0,
				.layerCount = 1
			},
			.srcOffsets = {{0, 0, 0}, {static_cast<i32>(SrcExtent.width), static_cast<i32>(SrcExtent.height), 1}},
			.dstSubresource = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.mipLevel = 0,
				.baseArrayLayer = 0,
				.layerCount = 1
			},
			.dstOffsets = {{0, 0, 0}, {static_cast<i32>(DstExtent.width), static_cast<i32>(DstExtent.height), 1}}
		};
		
		vkCmdBlitImage(
			Cmd,
			Src,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			Dst,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&BlitRegion,
			VK_FILTER_LINEAR
		);
	}
	
	LVKBuffer LVKBuffer::Allocate(arch Size, VkBufferUsageFlags UsageFlags, VmaAllocator Allocator, VmaMemoryUsage MemoryUsage)
	{
		VkBufferCreateInfo BufferInfo = {
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.pNext = nullptr,
			.size = Size,
			.usage = UsageFlags,
		};
		
		VmaAllocationCreateInfo VmaAllocInfo = {
			.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT,
			.usage = MemoryUsage,
		};
		
		LVKBuffer OutBuffer;
		VK_CHECK_RESULT(vmaCreateBuffer(Allocator, &BufferInfo, &VmaAllocInfo, &OutBuffer.Buffer, &OutBuffer.Allocation, &OutBuffer.Info));
		return OutBuffer;
	}
}