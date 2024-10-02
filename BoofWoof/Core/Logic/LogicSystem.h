#pragma once

#ifndef LOGIC_SYSTEM_H
#define LOGIC_SYSTEM_H

class Behaviour;

#include "../ECS/System.hpp"
#include "Behaviour.h"
#include <map>

class LogicSystem : public System
{
public:
	void Init();
	void Update();

	void AddBehaviour(Behaviour* behaviour);
private:
	// Keep track of all the behaviours
	 std::map<const char*, Behaviour*> mBehaviours;
};

#endif  // LOGIC_SYSTEM_H