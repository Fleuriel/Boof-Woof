#pragma once

#ifndef LOGIC_SYSTEM_H
#define LOGIC_SYSTEM_H

struct Behaviour;

#include "../ECS/System.hpp"
#include "Behaviour.h"
#include <map>

class LogicSystem : public System
{
public:
	void Init();
	void Update();
	void Shutdown();

	void AddBehaviour(Behaviour* behaviour);
private:
	// Keep track of all the behaviours
	 std::map<std::string, Behaviour*> mBehaviours;
};

extern Entity g_Player;
#endif  // LOGIC_SYSTEM_H