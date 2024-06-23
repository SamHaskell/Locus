#pragma once

#include "Base/Asserts.hpp"
#include "Defines.hpp"

using HandleType = u32;

// A handle is composed of an index (I) and generation (G) [GGIIIIII] ...
// The index part is used to point into some datastore
// The generation part is used to keep track of entity lifetimes to allow for slot reuse

constexpr uint32_t GenerationBits { 10 };
constexpr uint32_t IndexBits { (sizeof(HandleType) * 8) - GenerationBits };
static_assert((GenerationBits + IndexBits) == (sizeof(HandleType) * 8));

constexpr HandleType HandleMask { HandleType{static_cast<HandleType>(-1)} };
constexpr HandleType IndexMask { (HandleType{1} << IndexBits) - 1 };
constexpr HandleType GenerationMask { ~IndexMask };

constexpr HandleType HANDLE_INVALID { HandleMask };
static_assert((GenerationMask^IndexMask) == HandleMask);
	
namespace Handles
{
	inline HandleType Create(HandleType Index)
	{
		LAssert(Index < (1 << IndexBits));
		return Index;
	}
	
	inline bool IsValid(HandleType Handle)
	{
		return Handle != HandleMask;
	}
	
	inline HandleType GetIndex(HandleType Handle)
	{
		return Handle & IndexMask;
	}
	
	inline HandleType GetGeneration(HandleType Handle)
	{
		return (Handle & GenerationMask) >> IndexBits;
	}
	
	inline HandleType Regenerate(HandleType Handle)
	{
		const HandleType Generation {GetGeneration(Handle) + 1};
		LAssert(Generation < (1 << GenerationBits));
		return GetIndex(Handle) | (Generation << IndexBits);
	}
}