#pragma once
#include "../ECS/pch.hpp"
#include "../ECS/Coordinator.hpp"
#include "LogicSystem.h"


typedef void(*InitBehaviour)(Entity*);
typedef void(*UpdateBehaviour)(Entity*);
typedef void(*DestroyBehaviour)(Entity*);

class Behaviour
{
public:
	Behaviour(const InitBehaviour & init, const UpdateBehaviour & update, const DestroyBehaviour & destroy)
		: Init(init), Update(update), Destroy(destroy) {
		//g_Coordinator.GetSystem<LogicSystem>();
	}

	int getBehaviourIndex() { return BehaviourIndex; }

private:
	int BehaviourIndex{};
	InitBehaviour Init;
	UpdateBehaviour Update;
	DestroyBehaviour Destroy;
};