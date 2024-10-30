#pragma once
#ifndef SCRIPT_TO_ENGINE_H
#define SCRIPT_TO_ENGINE_H

#include "../ECS/Coordinator.hpp"
#include "BehaviourInterface.h"
#include "../Input/Input.h"

class Script_to_Engine : public engine_interface, public input_interface
{
public:
	/*
	virtual void AddComponent(Entity entity, ComponentType type) {
		(void)entity, type;
		// Add component to entity
		//g_Coordinator.AddComponent(entity, type);
	}
	*/
	/*
	template <typename T>
	virtual T GetComponent(Entity entity, T type) {
		g_Coordinator.GetComponent<type>(entity);
		
	}*/
	virtual bool isButtonPressed(std::uint32_t Key)
	{
		return g_Input.GetKeyState(Key);
	}

	virtual input_interface& getInputSystem() override
	{
		return *this;
	}

};

#endif // !SCRIPT_TO_ENGINE_H