#include "LogicSystem.h"
#include "../ECS/Coordinator.hpp"
#include "../Utilities/Components/BehaviourComponent.hpp"

#include "../Scripts/Movement.cpp"


void LogicSystem::Init()
{
	std::cout << "Logic System Initialized" << std::endl;

	// Add behaviours here
	AddBehaviour(new Behaviour("Movement", Movement::Start, Movement::Update, Movement::Destroy));


	for (auto const& entity : mEntities)
	{
		// Get the logic component of the entity
		BehaviourComponent behaviourComponent = g_Coordinator.GetComponent<BehaviourComponent>(entity);

		// Check if behaviour exists
		if (mBehaviours.find(behaviourComponent.GetBehaviourName()) == mBehaviours.end())
		{
			std::cout << "Behaviour not found" << std::endl;
			continue;
		}
		mBehaviours[behaviourComponent.GetBehaviourName()]->Init(entity);

	}
}

void LogicSystem::Update()
{
	//std::cout << "Logic System Updated" << std::endl;
	for (auto const& entity : mEntities)
	{
		// Get the logic component of the entity
		BehaviourComponent behaviourComponent = g_Coordinator.GetComponent<BehaviourComponent>(entity);
		if (mBehaviours.find(behaviourComponent.GetBehaviourName()) == mBehaviours.end())
		{
			std::cout << "Behaviour not found" << std::endl;
			continue;
		}
		// Find which behaviour the entity has and run
		mBehaviours[behaviourComponent.GetBehaviourName()]->Update(entity);
	}
}

void LogicSystem::AddBehaviour(Behaviour* behaviour)
{
	mBehaviours[behaviour->getBehaviourName()] = behaviour;
	std::cout << "Behaviour Added : " << behaviour->getBehaviourName() <<   std::endl;
}