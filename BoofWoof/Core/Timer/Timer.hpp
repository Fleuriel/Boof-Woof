#pragma once

#include <chrono>
#define g_Timer Timer::GetInstance()

class Timer 
{
public:
	static Timer& GetInstance() {
		static Timer instance;
		return instance;
	}

	Timer() : m_StartTime(std::chrono::steady_clock::now()) {}

	double GetElapsedTime() const 
	{
		auto endTime = std::chrono::steady_clock::now();
		double m_ElapsedTime = std::chrono::duration<double, std::milli>(endTime - m_StartTime).count();
		return m_ElapsedTime;
	}

	double GetCurrentTime() 
	{
		auto nowTime = std::chrono::high_resolution_clock::now();
		double m_CurrTime = std::chrono::duration<double>(nowTime.time_since_epoch()).count();
		return m_CurrTime;
	}

private:
	std::chrono::steady_clock::time_point m_StartTime;
};