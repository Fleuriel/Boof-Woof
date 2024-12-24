#pragma once

#ifndef ENGINE_CORE_H
#define ENGINE_CORE_H

#ifdef APIENTRY
#undef APIENTRY
#endif

#include "../Core/ECS/Coordinator.hpp"
#include "../Utilities/Components/GraphicsComponent.hpp"
#include "../Utilities/Components/AnimationComponent.h"
#include "../Utilities/Components/TransformComponent.hpp"
#include "../Utilities/Components/MaterialComponent.hpp"
#include "../Utilities/Components/HierarchyComponent.hpp"


#include "../Utilities/Components/BehaviourComponent.hpp"
#include "../Utilities/Components/CollisionComponent.hpp"
#include "../Utilities/Components/MetaData.hpp"
#include "../Utilities/Components/CameraComponent.hpp"
#include "../Utilities/Components/ParticleComponent.hpp"
#include "../Utilities/Components/LightComponent.hpp"

#include "Windows/WindowManager.h"
#include "AssetManager/AssetManager.h"

#include "Graphics/GraphicsSystem.h"
#include "Graphics/FontSystem.h"
#include "Logic/LogicSystem.h"
#include "Physics/PhysicsSystem.h"
#include "Physics/CollisionSystem.h"
#include "Graphics/TransformSystem.hpp"
#include "Pathfinding/PathfindingSystem.h"


#include "Serialization/Serialization.h"
#include "Input/Input.h"

#include "SceneManager/SceneManager.h"
#include "Audio/AudioSystem.h"

class EngineCore 
{
public:
	void OnInit();
	void OnUpdate();
	void OnShutdown();

public:
	double m_ElapsedDT{}, m_AccumulatedTime{}, m_StartTime{}, m_EndTime{}, m_DeltaTime{};
	double m_GraphicsDT{}, m_LogicDT{}, m_PhysicsDT{};
	int m_CurrNumSteps{};
	const double m_FixedDT = 1.0 / 60.0; // user defined 60fps

	double GetCurrentTime() {
		// Get the current time as a double value
		auto now = std::chrono::high_resolution_clock::now();
		return std::chrono::duration<double>(now.time_since_epoch()).count();
	}
};

class Timer 
{
public:
	Timer()
		: mStartTime(std::chrono::steady_clock::now()) {}

	double GetElapsedTime() const {
		auto endTime = std::chrono::steady_clock::now();
		double mElapsedTime = std::chrono::duration<double, std::milli>(endTime - mStartTime).count();
		return mElapsedTime;
	}

private:
	std::chrono::steady_clock::time_point mStartTime;
};

extern EngineCore* g_Core;

#endif