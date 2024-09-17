#pragma once
#include "pch.h"
#include "System.h"
#include "Coordinator.h"

extern Coordinator gCoordinator;

class TestSystem : public System
{
public:
	void Update(float dt)
		{
		/*
			std::cout << "TestSystem Update" << std::endl;
			for(auto const& entity : mEntities)
			{
				auto& testComponent = gCoordinator.GetComponent<TestComponent>(entity);
				std::cout << "Entity: " << entity << " X: " << testComponent.x << " Y: " << testComponent.y << std::endl;
			}
			*/
		}
};
