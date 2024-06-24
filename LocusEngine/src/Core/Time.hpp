#pragma once

#include "Base/Base.hpp"

#include <chrono>

namespace Locus
{
	class Clock
	{
	public:
		Clock();
		~Clock() = default;

		void Start();
		void Stop();
		void Reset(bool bStart = true);

		f64 GetElapsedSeconds() const;
		f64 GetElapsedMilliseconds() const;
	
	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime;	
		std::chrono::duration<f64> m_Elapsed; // Milliseconds
		bool m_Paused;
	};
}