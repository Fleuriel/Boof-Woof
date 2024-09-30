#pragma once

#ifndef ENGINE_CORE_H
#define ENGINE_CORE_H

#include "../Core/ECS/Coordinator.hpp"
#include <iostream>
#include "../Utilities/Components/GraphicsComponent.hpp"
#include "../Utilities/Components/TransformComponent.hpp"
#include "Windows/WindowManager.h"
#include "Graphics/GraphicsSystem.h"
#include "AssetManager/AssetManager.h"
#include "Serialization/Serialization.h"
#include "Input/Input.h"
#include "../Utilities/Components/MetaData.hpp"
#include "Timer/Timer.hpp"

#include <GLFW/glfw3.h>
#include "fmod.hpp"

class EngineCore 
{
public:
	void OnInit();
	void OnUpdate();
	void OnShutdown();

public:
	double m_ElapsedDT{}, m_AccumulatedTime{}, m_StartTime{}, m_EndTime{}, m_DeltaTime{};
	double m_GraphicsDT{};
	int m_CurrNumSteps{};
	const double m_FixedDT = 1.0 / 60.0; // user defined 60fps
};

extern EngineCore* g_Core;

#endif