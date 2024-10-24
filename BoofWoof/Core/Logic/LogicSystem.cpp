#include "LogicSystem.h"
#include "../ECS/Coordinator.hpp"
#include "../Utilities/Components/BehaviourComponent.hpp"
#include "../../../Scripts/Compile.cpp"
#include "Script_to_Engine.h"
#include <wtypes.h>

void LogicSystem::Init()
{
	std::cout << "Logic System Initialized" << std::endl;

	HINSTANCE hGetProcIDDLL = LoadLibrary(L"..\\Scripts\\Scripts.dll");
	
	if (hGetProcIDDLL == NULL) {
		std::cerr << "Could not load the dynamic library" << std::endl;
	}
	if (hGetProcIDDLL) {
		auto pGetScripts = (GetScripts_cpp_t)GetProcAddress(hGetProcIDDLL, "GetScripts");
		if (!pGetScripts) 
			std::cout << "Its not working" << std::endl;
		else
		// Get the scripts from the script engine
		// Add scripts from script engine to logic system
		AddBehaviours(pGetScripts(new Script_to_Engine()));
	}
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
		else
		{
			//print all the behaviours
			for (auto const& behaviour : mBehaviours)
			{
				std::cout << behaviour.first << std::endl;
			}
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
	
	// Clear the map to remove all entries
	//mBehaviours.clear();
}

void LogicSystem::AddBehaviours(std::vector<std::unique_ptr<Behaviour_i>>* B_Vec)
{
	for (int i = 0; B_Vec->begin() + i != B_Vec->end(); i++)
	{
		std::cout << "Behaviour Added : " << B_Vec->begin()[i]->getBehaviourName() << std::endl;
		mBehaviours[B_Vec->begin()[i]->getBehaviourName()] = std::move(B_Vec->begin()[i]);
		
	}
	
}