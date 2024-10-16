#pragma once
#ifndef SCRIPT_TO_ENGINE_H
#define SCRIPT_TO_ENGINE_H

#include "BehaviourInterface.h"

class Script_to_Engine : public engine_interface
{
	virtual void AddComponent(Entity entity, ComponentType type) {}
};

#endif // !SCRIPT_TO_ENGINE_H