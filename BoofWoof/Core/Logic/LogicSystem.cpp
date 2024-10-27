#include "LogicSystem.h"
#include "../ECS/Coordinator.hpp"
#include "../Utilities/Components/BehaviourComponent.hpp"
#include "Scripts/Null.cpp"
#include "Scripts/Player.cpp"
#include "Scripts/Movement.cpp"

void LogicSystem::Init()
{
	std::cout << "Logic System Initialized" << std::endl;

	// Add behaviours here
	AddBehaviour(new Behaviour("Null", Null::Start, Null::Update, Null::Destroy));
	AddBehaviour(new Behaviour("Movement", Movement::Start, Movement::Update, Movement::Destroy));
	AddBehaviour(new Behaviour("Player", Player::Start, Player::Update, Player::Destroy));


	for (auto const& entity : mEntities)
	{
		// Get the logic component of the entity
		BehaviourComponent behaviourComponent = g_Coordinator.GetComponent<BehaviourComponent>(entity);

		// Check if behaviour exists
		if (mBehaviours.find(behaviourComponent.GetBehaviourName()) == mBehaviours.end())
		{
			//std::cout << "Behaviour not found" << std::endl;
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
		if (g_Coordinator.HaveComponent<BehaviourComponent>(entity)) {
			// Get the logic component of the entity
			BehaviourComponent behaviourComponent = g_Coordinator.GetComponent<BehaviourComponent>(entity);
			if (mBehaviours.find(behaviourComponent.GetBehaviourName()) == mBehaviours.end())
			{
				//std::cout << behaviourComponent.GetBehaviourName() << std::endl;
				continue;
			}
			// Find which behaviour the entity has and run
			mBehaviours[behaviourComponent.GetBehaviourName()]->Update(entity);
		}
	}
}

void LogicSystem::Shutdown()
{
	for (auto& behaviourPair : mBehaviours)
	{
		delete behaviourPair.second;  // Free dynamically allocated Behaviour objects
	}

	// Clear the map to remove all entries
	mBehaviours.clear();
}

void LogicSystem::AddBehaviour(Behaviour* behaviour)
{
	mBehaviours[behaviour->getBehaviourName()] = behaviour;
	std::cout << "Behaviour Added : " << behaviour->getBehaviourName() <<   std::endl;
}