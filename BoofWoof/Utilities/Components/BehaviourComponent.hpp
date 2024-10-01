#pragma once

#include "ECS/Coordinator.hpp"

class BehaviourComponent
{
public:
	int getBehaviourIndex() { return BehaviourIndex; }
private:
	Entity m_EntityID{};
	int BehaviourIndex{};
};