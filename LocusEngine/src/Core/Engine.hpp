#pragma once

#include "Base/Base.hpp"
#include "Core/DisplayManager.hpp"

namespace Locus
{
	class Engine : public Singleton<Engine>
	{
	public:
		void Init();
		void Shutdown();
	
	private:
		DisplayManager* m_DisplayManager;
		
	
	};
}