#include "Time.hpp"
#include <chrono>

namespace Locus
{
	Clock::Clock()
	{
		Reset(false);
	}

	void Clock::Start()
	{
		if (m_Paused)
		{
			m_StartTime = std::chrono::high_resolution_clock::now();
			m_Paused = false;
		}
	}
	
	void Clock::Stop()
	{
		if (!m_Paused)
		{
			auto Now = std::chrono::high_resolution_clock::now();
			m_Elapsed +=  Now - m_StartTime;
			m_StartTime = Now;
			m_Paused = true;			
		}
	}
	
	void Clock::Reset(bool bStart)
	{
		m_StartTime = std::chrono::high_resolution_clock::now();
		m_Elapsed = std::chrono::duration<f64>(0);
		m_Paused = !bStart;
	}

	f64 Clock::GetElapsedSeconds() const
	{
		if (!m_Paused)
		{
			return std::chrono::duration<f64>(std::chrono::high_resolution_clock::now() - m_StartTime).count() + m_Elapsed.count();
		}
		else
		{
			return m_Elapsed.count();
		}
	}
	
	f64 Clock::GetElapsedMilliseconds() const
	{
		return GetElapsedSeconds() * 1000.0;
	}
}