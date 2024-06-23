#pragma once

#include "Base/Asserts.hpp"
namespace Locus
{
	template <typename T> class Singleton
	{
	public:
		Singleton(const Singleton<T>&) = delete;
		Singleton& operator=(const Singleton<T>&) = delete;
	
		Singleton()
		{
			LAssertMsg(m_Instance == nullptr, "You must not instantiate any class derived from Singleton<T> more than once.");
			m_Instance = static_cast<T*>(this);
		}
		
		~Singleton()
		{
			LAssertMsg(m_Instance != nullptr, "Calling destructor on singleton when there is no instance.");
			m_Instance = nullptr;
		}
		
		static T& Get()
		{
			LAssertMsg(m_Instance, "Please ensure the singleton has been instantiated before attempting to get a reference.");
			return (*m_Instance);
		}
		
		// For use in checking if the singleton has an instance.
		static const T* GetPtr()
		{
			return m_Instance;
		}
	
	protected:
		static T* m_Instance;
	};
	
	template <typename T>
	T* Singleton<T>::m_Instance;
}