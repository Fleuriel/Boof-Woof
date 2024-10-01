#pragma once

#include "ECS/Coordinator.hpp"

class BehaviourComponent
{
public:
	int getBehaviourIndex() { return BehaviourIndex; }

	void setBehaviourIndex(int index) { BehaviourIndex = index; }

	void SetComponentEntityID(Entity& entity) { m_EntityID = entity; }
private:
	Entity m_EntityID{};
	int BehaviourIndex{};
};