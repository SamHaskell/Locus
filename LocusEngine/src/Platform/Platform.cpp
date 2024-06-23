#include "Platform.hpp"

#include "Base/Asserts.hpp"
#include "Base/Defines.hpp"

#include <chrono>
#include <thread>
#include <fstream>

#include "Platform/LSDL/LSDLPlatform.hpp"

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
		
		bool FileGetSize(const char* Path, arch& Size)
		{
			std::ifstream File(Path, std::ios::ate | std::ios::binary);
			if (!File.is_open())
			{
				return false;
			}
			
			Size = File.tellg();
			File.close();
			return true;
		}
		
		bool FileReadBytes(const char* Path, u8* Data, arch Size)
		{
			std::ifstream File(Path, std::ios::binary);
			if (!File.is_open())
			{
				return false;
			}
			
			File.read((char*)Data, Size);
			File.close();
			return true;
		}
	}
};