#pragma once

#include <EngineCore.h>

class TimerTR
{
public:
	void OnInitialize();
	void OnUpdate(double deltaTime);
	void OnShutdown();
	void Reset();

public:
	Entity m_Timer{}, m_Minus{};
	double timer = 180.0; // 3 minutes in seconds
	double interval = 1.0; // Time interval in seconds
	bool touched{ false };

private:
	double minus10timer = 3.0; // 3 seconds

protected:
	std::vector<Entity> storage;
};

extern TimerTR g_TimerTR;