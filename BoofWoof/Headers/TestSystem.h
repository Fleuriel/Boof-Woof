#pragma once
#include "../Core/ECS/pch.h"
#include "ECS/Coordinator.h"
#include "TestComponent.h"
#include "pch.h"
#include "System.h"
#include "Coordinator.h"

extern Coordinator gCoordinator;

class TestSystem : public System
{
public:
	void Update(float dt);

	std::set<Entity> mEntities;
};
