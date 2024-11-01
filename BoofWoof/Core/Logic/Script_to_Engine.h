#pragma once
#ifndef SCRIPT_TO_ENGINE_H
#define SCRIPT_TO_ENGINE_H

#include "../ECS/Coordinator.hpp"
#include "BehaviourInterface.h"
#include "../Input/Input.h"

class Script_to_Engine : public engine_interface, public input_interface
{
public:


	virtual bool isButtonPressed(std::uint32_t Key) override
	{
		return g_Input.GetKeyState(Key);
	}

	virtual input_interface& getInputSystem() override
	{
		return *this;
	}
	
	virtual glm::vec3 GetPosition(Entity entity) override
	{
		return g_Coordinator.GetComponent<TransformComponent>(entity).GetPosition();
	}

	virtual void SetPosition(Entity entity, glm::vec3 position) override
	{
		g_Coordinator.GetComponent<TransformComponent>(entity).SetPosition(position);
	}

};

#endif // !SCRIPT_TO_ENGINE_H