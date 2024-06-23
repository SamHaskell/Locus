#include "Engine.hpp"

#include "Core/DisplayManager.hpp"
#include "Platform/LSDL/LSDLDisplayManager.hpp"

namespace Locus
{
	static Engine gEngine;
	
	void Engine::Init()
	{
		m_DisplayManager = new LSDLDisplayManager();
	}
	
	void Engine::Shutdown()
	{
		delete m_DisplayManager;
	}
}