#pragma once
#ifndef BEHAVIOURINTERFACE_H
#define BEHAVIOURINTERFACE_H

#include "../ECS/pch.hpp"


struct Behaviour_i
{
	virtual ~Behaviour_i() = default;
	virtual void Init(Entity entity) = 0;
	virtual void Update(Entity entity) = 0;
	virtual void Destroy(Entity entity) = 0;
	virtual const char* getBehaviourName() = 0;
};

struct engine_interface
{
	virtual ~engine_interface() = default;
	//virtual void AddComponent(Entity entity, ComponentType type) = 0;
	virtual void Movement(Entity entity) = 0;
};

using GetScripts_cpp_t = std::vector<std::unique_ptr<Behaviour_i>>* (*)(engine_interface* pEI);

#endif // !BEHAVIOURINTERFACE_H