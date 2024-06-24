#pragma once

#include "Asserts.hpp"
#include "Defines.hpp"

#include <cstring>

namespace Locus
{
	template <class T>
	class TArray
	{
		public:
			TArray(arch count) :
			m_Capacity(count), 
			m_Count(count)
			{
				m_Data = (T*)(malloc(m_Capacity * sizeof(T)));
			}
			
			TArray() = default;
			
			~TArray() 
			{
				free(m_Data);
			};
			
			arch Length() const
			{
				return m_Count;
			}
			
			arch Max() const
			{
				return m_Capacity;
			}
			
			T* Data() const
			{
				return m_Data;
			}
			
			bool Empty() const
			{
				return (m_Count == 0);
			}
			
			const T& GetElement(arch index) const
			{
				return m_Data[index];
			}
			
			T& operator[] (arch index) const
			{
				return m_Data[index];
			}
			
			void Push(T value)
			{
				m_Count ++;
				if (m_Count >= m_Capacity)
				{
					arch newCapacity = (arch)((m_Capacity + 1) * 1.5f);
					Resize(newCapacity);
					m_Capacity = newCapacity;
				}
				m_Data[m_Count - 1] = value;
			}
			
			T Pop()
			{
				LAssert(!Empty());
				m_Count --;
				return m_Data[m_Count];
			}
			
			void Clear()
			{
				m_Count = 0;
				memset(m_Data, 0, sizeof(T) * m_Capacity);
			}
			
			void Resize(arch capacity)
			{
				m_Capacity = capacity;
				if (m_Data != nullptr)
				{
					auto temp = (T*)realloc(m_Data, m_Capacity * sizeof(T));
					LAssert(temp != NULL);
					m_Data = temp;
				}
				else
				{
					m_Data = (T*)malloc(m_Capacity * sizeof(T));
				}
			}
			
			void Reserve(arch capacity)
			{
				Resize(capacity);
				m_Count = capacity;
			}
		
		private:
			arch m_Capacity = 0;
			arch m_Count = 0;
			T* m_Data = nullptr;
	};
}
