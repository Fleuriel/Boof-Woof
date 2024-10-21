#pragma once
#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H


#include "../ECS/pch.hpp"
#include "../ECS/Coordinator.hpp"
#include "LogicSystem.h"
#include "BehaviourInterface.h"

struct Behaviour : public Behaviour_i
{
	Behaviour(engine_interface& Engine) : m_Engine(Engine) {}
	virtual void Init(Entity entity) override {}
	virtual void Update(Entity entity) override {}
	virtual void Destroy(Entity entity) override {}
	engine_interface&	m_Engine;
};


#endif  // BEHAVIOUR_H