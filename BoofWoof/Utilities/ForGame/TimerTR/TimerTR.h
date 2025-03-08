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
	double timer = 30.0; // 30 seconds
	bool touched{ false }, scaleUp{ false };

protected:
	std::vector<Entity> storage;
};

extern TimerTR g_TimerTR;