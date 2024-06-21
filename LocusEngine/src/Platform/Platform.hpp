#pragma once

#include "Base/Base.hpp"

namespace Locus
{
	namespace Platform
	{
		void Init();
		void Shutdown();
		
		void SleepThisThread(u32 Milliseconds);
	};
};