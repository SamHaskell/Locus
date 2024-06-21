#include "Platform/Platform.hpp"

#include "SDL.h"

namespace Locus
{
	namespace LSDL
	{
		static bool sInitialized = false;
		
		void PlatformInit()
		{
			LAssert(!sInitialized);
			LCheck(SDL_Init(SDL_INIT_EVERYTHING) == 0);
			sInitialized = true;
		}
		
		void PlatformShutdown()
		{
			LAssert(!sInitialized);
			SDL_Quit();
			sInitialized = false;
		}
	}
}