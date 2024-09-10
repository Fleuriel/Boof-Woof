#pragma once
#include "pch.h"
#include "Coordinator.h"
#include <TestComponent.h>


class TestSystem : public System
{
public:
	void Update(float dt);

	std::set<Entity> mEntities;
};