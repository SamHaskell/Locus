#include "Platform.hpp"

#include "Base/Asserts.hpp"
#include "Base/Defines.hpp"

#include <chrono>
#include <thread>
#include <fstream>

namespace Locus
{
	namespace Platform
	{		
		void SleepThisThread(u32 Milliseconds)
		{
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