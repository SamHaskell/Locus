#include "Platform.hpp"

#include "Base/Asserts.hpp"
#include "Base/Defines.hpp"
#include "Platform/LSDL/LSDLPlatform.hpp"
#include <chrono>
#include <thread>

namespace Locus
{
	namespace Platform
	{
		static bool sInitialized = false;
		
		void Init()
		{
			LAssert(!sInitialized);
			LSDL::PlatformInit();
			// TODO: Set up fp's for any platform specific functionality.
			sInitialized = true;
		}
		
		void Shutdown()
		{
			LAssert(sInitialized);
			LSDL::PlatformShutdown();
			// Clean up.
			sInitialized = false;
		}
		
		void SleepThisThread(u32 Milliseconds)
		{
			LAssert(sInitialized);
			std::this_thread::sleep_for(std::chrono::milliseconds(Milliseconds));
		}
	}
};