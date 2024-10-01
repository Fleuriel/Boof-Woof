#include "LogicSystem.h"
#include "../ECS/Coordinator.hpp"
#include "../Utilities/Components/BehaviourComponent.hpp"


void LogicSystem::Init()
{
	std::cout << "Logic System Initialized" << std::endl;
}

void LogicSystem::Update()
{
	std::cout << "Logic System Updated" << std::endl;
	for (auto const& entity : mEntities)
	{
		// Get the logic component of the entity
		BehaviourComponent behaviourComponent = g_Coordinator.GetComponent<BehaviourComponent>(entity);

		// Find which behaviour the entity has and run
		(mBehaviours[behaviourComponent.getBehaviourIndex()])->Update(entity);
	}
}