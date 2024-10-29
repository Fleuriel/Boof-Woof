#pragma once
#ifndef SCRIPT_TO_ENGINE_H
#define SCRIPT_TO_ENGINE_H

#include "../ECS/Coordinator.hpp"
#include "BehaviourInterface.h"
#include "../Input/Input.h"

class Script_to_Engine : public engine_interface
{
public:
	/*
	virtual void AddComponent(Entity entity, ComponentType type) {
		(void)entity, type;
		// Add component to entity
		//g_Coordinator.AddComponent(entity, type);
	}
	*/

	virtual void Movement(Entity entity) override {
		// If W, A, S, D is pressed, move entity
		if (g_Input.GetKeyState(GLFW_KEY_W) >= 1) {
			g_Coordinator.GetComponent<TransformComponent>(entity).SetPosition(g_Coordinator.GetComponent<TransformComponent>(entity).GetPosition() + glm::vec3(0.0f, 0.0f, -0.1f));
			std::cout << "W is Pressed" << std::endl;
		}
		if (g_Input.GetKeyState(GLFW_KEY_S) >= 1) {
			g_Coordinator.GetComponent<TransformComponent>(entity).SetPosition(g_Coordinator.GetComponent<TransformComponent>(entity).GetPosition() + glm::vec3(0.0f, 0.0f, 0.1f));
			std::cout << "S is Pressed" << std::endl;
		}
		if (g_Input.GetKeyState(GLFW_KEY_A) >= 1) {
			g_Coordinator.GetComponent<TransformComponent>(entity).SetPosition(g_Coordinator.GetComponent<TransformComponent>(entity).GetPosition() + glm::vec3(-0.1f, 0.0f, 0.0f));
			std::cout << "A is Pressed" << std::endl;
		}
		if (g_Input.GetKeyState(GLFW_KEY_D) >= 1) {
			g_Coordinator.GetComponent<TransformComponent>(entity).SetPosition(g_Coordinator.GetComponent<TransformComponent>(entity).GetPosition() + glm::vec3(0.1f, 0.0f, 0.0f));
			std::cout << "D is Pressed" << std::endl;
		}

		
	}
};

#endif // !SCRIPT_TO_ENGINE_H