#pragma once

#include "Base/Base.hpp"

namespace Locus
{
	namespace Platform
	{
		void Init();
		void Shutdown();
		
		void SleepThisThread(u32 Milliseconds);
		
		bool FileGetSize(const char* Path, arch& Size);
		bool FileReadBytes(const char* Path, u8* Data, arch Size);
	};
};