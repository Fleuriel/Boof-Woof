#pragma once

#ifndef ENGINE_CORE_H
#define ENGINE_CORE_H

#include "../Core/ECS/Coordinator.hpp"
#include <iostream>
#include "../Utilities/Components/GraphicsComponent.hpp"
#include "../Utilities/Components/TransformComponent.hpp"
#include "../Utilities/Components/BehaviourComponent.hpp"
#include "../Utilities/Components/MetaData.hpp"

#include "Windows/WindowManager.h"
#include "AssetManager/AssetManager.h"

#include "Graphics/GraphicsSystem.h"
#include "Graphics/FontSystem.h"
#include "Logic/LogicSystem.h"
#include "Physics/PhysicsSystem.h"


#include "Serialization/Serialization.h"
#include "Input/Input.h"

#include "Timer/Timer.hpp"
#include "SceneManager/SceneManager.h"
#include "Audio/AudioSystem.h"

#include "fmod.hpp"

class EngineCore 
{
public:
	void OnInit();
	void OnUpdate();
	void OnShutdown();

public:
	double m_ElapsedDT{}, m_AccumulatedTime{}, m_StartTime{}, m_EndTime{}, m_DeltaTime{};
	double m_GraphicsDT{}, m_LogicDT{};
	int m_CurrNumSteps{};
	const double m_FixedDT = 1.0 / 60.0; // user defined 60fps
};

extern EngineCore* g_Core;

#endif