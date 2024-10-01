#pragma once

#include "../ECS/pch.hpp"
#include "../ECS/Coordinator.hpp"
#include "LogicSystem.h"


typedef void(*InitBehaviour)(Entity);
typedef void(*UpdateBehaviour)(Entity);
typedef void(*DestroyBehaviour)(Entity);

class Behaviour
{
public:
	Behaviour(const InitBehaviour & init, const UpdateBehaviour & update, const DestroyBehaviour & destroy)
		: InitFunction(init), UpdateFunction(update), DestroyFunction(destroy) {
		g_Coordinator.GetSystem<LogicSystem>()->AddBehaviour(this);
		
	}

	void Init(Entity entity) { InitFunction(entity); }
	void Update(Entity entity) { UpdateFunction(entity); }
	void Destroy(Entity entity) { DestroyFunction(entity); }

	int getBehaviourIndex() { return BehaviourIndex; }

private:
	int BehaviourIndex{};
	InitBehaviour InitFunction;
	UpdateBehaviour UpdateFunction;
	DestroyBehaviour DestroyFunction;
};