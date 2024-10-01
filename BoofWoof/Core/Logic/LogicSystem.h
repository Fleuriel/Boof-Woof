#pragma once
#include "../ECS/System.hpp"
#include "Behaviour.h"
#include <map>

class LogicSystem : public System
{
public:
	void Init();
	void Update();

	void AddBehaviour(Behaviour * behaviour)
	{
		mBehaviours[behaviour->getBehaviourIndex()] = behaviour;
	}
private:
	// Keep track of all the behaviours
	 std::map<int, Behaviour*> mBehaviours;
};