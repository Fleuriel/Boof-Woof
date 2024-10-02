#pragma once
#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H


#include "../ECS/pch.hpp"
#include "../ECS/Coordinator.hpp"
#include "LogicSystem.h"



typedef void(*InitBehaviour)(Entity);
typedef void(*UpdateBehaviour)(Entity);
typedef void(*DestroyBehaviour)(Entity);

class Behaviour
{
public:
	Behaviour(const char* mBehaviourName,const InitBehaviour& init, const UpdateBehaviour& update, const DestroyBehaviour& destroy);

	void Init(Entity entity);
	void Update(Entity entity);
	void Destroy(Entity entity);

	const char* getBehaviourName() { return mBehaviourName; }

private:
	const char* mBehaviourName;
	InitBehaviour InitFunction;
	UpdateBehaviour UpdateFunction;
	DestroyBehaviour DestroyFunction;
};

#endif  // BEHAVIOUR_H