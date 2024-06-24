#pragma once

#include "Array.hpp"
#include "Asserts.hpp"
#include "Defines.hpp"
#include <cstddef>

namespace Locus
{
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
		
	inline HandleType HandleCreate(HandleType Index)
	{
		LAssert(Index < (1 << IndexBits));
		return Index;
	}
	
	inline bool HandleIsValid(HandleType Handle)
	{
		return Handle != HandleMask;
	}
	
	inline HandleType HandleIndex(HandleType Handle)
	{
		return Handle & IndexMask;
	}
	
	inline HandleType HandleGeneration(HandleType Handle)
	{
		return (Handle & GenerationMask) >> IndexBits;
	}
	
	inline HandleType HandleRegenerate(HandleType Handle)
	{
		const HandleType Generation {HandleGeneration(Handle) + 1};
		LAssert(Generation < (1 << GenerationBits));
		return HandleIndex(Handle) | (Generation << IndexBits);
	}
	
	template<typename T>
	class Pool 
	{
	public:
		Pool(arch Size);
		
		void Clear();
		arch Size() const { return m_Size; }
		arch Count() const { return m_Count; }
		
		HandleType Create(T Value);
		bool Destroy(HandleType Handle);
		
		bool IsValid(HandleType Handle) const;
		bool IsValidAt(arch Index) const;
		
		T& GetMut(HandleType Handle);
		const T& Get(HandleType Handle) const;
		
		const T& GetValueAt(arch Index) const;
		const HandleType GetHandleAt(arch Index) const;
	
	private:
		TArray<T> m_Data;
		TArray<HandleType> m_Handles;
		TArray<HandleType> m_FreeList;
		u32 m_Count {0};
		u32 m_Size {0};
	};
	
	template<typename T>
	Pool<T>::Pool(arch Size)
	{
		m_Data.Reserve(Size);
		m_Handles.Reserve(Size);
		
		m_Size = Size;
	}
	
	template<typename T>
	void Pool<T>::Clear()
	{
		m_Data.Clear();
		m_Handles.Clear();
		m_FreeList.Clear();
		
		m_Count = 0;
	}
	
	template<typename T>
	HandleType Pool<T>::Create(T Value)
	{
		HandleType Handle;
		if (m_FreeList.Length() > 0)
		{
			// Reuse a slot
			Handle = m_FreeList.Pop();
			m_Data[HandleIndex(Handle)] = Value;
			m_Handles[HandleIndex(Handle)] = Handle;
		}
		else
		{
			// Create a new slot, append to the end, increment size
			Handle = HandleCreate(m_Count);
			m_Data[m_Count] = Value;
			m_Handles[m_Count] = Handle;
			m_Count++;
		}
		return Handle;
	}
	
	template<typename T>
	bool Pool<T>::Destroy(HandleType Handle)
	{
		if (IsValid(Handle))
		{
			// Invalidate the handle, push the regenerated handle onto the freelist
			m_FreeList.Push(HandleRegenerate(m_Handles[HandleIndex(Handle)]));
			m_Handles[HandleIndex(Handle)] = HANDLE_INVALID;	
		}
	}
	
	template<typename T>
	bool Pool<T>::IsValid(HandleType Handle) const
	{
		return (HandleIndex(Handle) < m_Size) && (Handle == m_Handles[Handle]);
	}
	
	template<typename T>
	bool Pool<T>::IsValidAt(arch Index) const
	{
		return (Index < m_Size) && (HandleIndex(m_Handles[Index]) == Index);
	}
	
	template<typename T>
	T& Pool<T>::GetMut(HandleType Handle)
	{
		LAssert(IsValid(Handle));
		return m_Data[HandleIndex(Handle)];
	}
	
	template<typename T>
	const T& Pool<T>::Get(HandleType Handle) const
	{
		LAssert(IsValid(Handle));
		return m_Data[HandleIndex(Handle)];
	}
	
	template<typename T>
	const T& Pool<T>::GetValueAt(arch Index) const
	{
		LAssert(Index < m_Size);
		return m_Data[Index];
	}
	
	template<typename T>
	const HandleType Pool<T>::GetHandleAt(arch Index) const
	{
		LAssert(Index < m_Size);
		return m_Handles[Index];
	}
}