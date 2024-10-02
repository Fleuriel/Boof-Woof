#pragma once

#include "ECS/Coordinator.hpp"

class BehaviourComponent
{
public:
	BehaviourComponent() {};
	BehaviourComponent(const char* behaviourname, Entity& entity) 
		: m_EntityID(g_Coordinator.GetEntityId(entity)), m_BehaviourName(behaviourname) {}

	Entity GetComponentEntityID() { return m_EntityID; }

	void SetComponentEntityID(Entity& entity) { m_EntityID = entity; }

	const char* GetBehaviourName() { return m_BehaviourName; }
private:
	Entity m_EntityID{};
	const char* m_BehaviourName{};
};