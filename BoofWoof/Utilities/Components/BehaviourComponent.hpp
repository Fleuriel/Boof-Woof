#pragma once

#include "ECS/Coordinator.hpp"

class BehaviourComponent
{
public:
	BehaviourComponent() {};
	BehaviourComponent(const char* behaviourname, Entity& entity) 
		: m_BehaviourName(behaviourname), m_EntityID(g_Coordinator.GetEntityId(entity)) {}

	void SetComponentEntityID(Entity& entity) { m_EntityID = entity; }

	const char* GetBehaviourName() { return m_BehaviourName; }
	void SetBehaviourName(const char* behaviourName) { m_BehaviourName = behaviourName; }

private:
	Entity m_EntityID{};
	const char* m_BehaviourName{};
};