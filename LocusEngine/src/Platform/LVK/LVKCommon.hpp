#pragma once

#include "Base/Base.hpp"

#include "vulkan/vulkan_core.h"
#include "vma/vk_mem_alloc.h"
#include "vulkan/vk_enum_string_helper.h"

#define VK_CHECK_RESULT(x)\
	do {\
		VkResult err = (x);\
		LAssertMsg(err == VK_SUCCESS, string_VkResult(err));\
	} while(0)
	
#define VK_CHECK_HANDLE(x)\
	do {\
		auto a = (x);\
		LAssertMsg(a != VK_NULL_HANDLE, "Vulkan Error: Null Handle");\
	} while(0)