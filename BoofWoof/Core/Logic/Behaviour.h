#pragma once
#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H

//#include "../ECS/Coordinator.hpp"
//#include "LogicSystem.h"
#include "BehaviourInterface.h"

struct Behaviour : public Behaviour_i
{
	Behaviour(engine_interface& Engine) : m_Engine(Engine) {}
	virtual void Init(Entity entity) override { (void)entity; }
	virtual void Update(Entity entity) override { (void)entity; }
	virtual void Destroy(Entity entity) override { (void)entity; }
	virtual const char* getBehaviourName() override { return "Not Initialized"; };
	engine_interface&	m_Engine;

};


#endif  // BEHAVIOUR_H