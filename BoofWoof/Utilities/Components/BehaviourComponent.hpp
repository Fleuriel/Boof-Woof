#pragma once

#include "ECS/Coordinator.hpp"

class BehaviourComponent
{
public:
	BehaviourComponent(): m_EntityID(0), m_BehaviourName("Null") {};
	BehaviourComponent(std::string behaviourname, Entity& entity) 
		: m_BehaviourName(behaviourname), m_EntityID(g_Coordinator.GetEntityId(entity)) {}

	~BehaviourComponent() = default;
	void SetComponentEntityID(Entity& entity) { m_EntityID = entity; }

	std::string GetBehaviourName() { return m_BehaviourName; }
	void SetBehaviourName(std::string behaviourName) { m_BehaviourName = behaviourName; }

private:
	Entity m_EntityID;
	std::string m_BehaviourName;
};