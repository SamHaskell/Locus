#include "Engine.hpp"

#include "Core/DisplayManager.hpp"
#include "Platform/LSDL/LSDLDisplayManager.hpp"
#include "Platform/LVK/LVKGraphicsManager.hpp"

namespace Locus
{
	static Engine gEngine;
	
	void Engine::Init()
	{
		m_DisplayManager = new LSDLDisplayManager();
		m_GraphicsManager = new LVKGraphicsManager();
	}
	
	void Engine::Shutdown()
	{
		delete m_GraphicsManager;
		delete m_DisplayManager;
	}
}