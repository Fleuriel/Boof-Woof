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

	// Exposing Engine functions to scripts
	virtual void ChangePosition(Entity entity, float x, float y, float z) override
	{
		auto& transform = g_Coordinator.GetComponent<TransformComponent>(entity);
		transform.SetPosition(transform.GetPosition() + glm::vec3(x, y, z));
	}

};

#endif // !SCRIPT_TO_ENGINE_H