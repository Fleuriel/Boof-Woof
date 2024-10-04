#include "../Core/Logic/Behaviour.h"
#include "../Core/ECS/Coordinator.hpp"
#include "../Core/Input/Input.h"

namespace Player {
	void Start(Entity entity) {
		std::cout << "Player Behaviour Started" << std::endl;
	}

	void Update(Entity entity) {
		if (g_Input.GetKeyState(GLFW_KEY_W) >= 1) {
			g_Coordinator.GetComponent<TransformComponent>(entity).SetPosition(g_Coordinator.GetComponent<TransformComponent>(entity).GetPosition() + glm::vec3(0.0f, 0.0f, -0.1f));
		}
		if (g_Input.GetKeyState(GLFW_KEY_S) >= 1) {
			g_Coordinator.GetComponent<TransformComponent>(entity).SetPosition(g_Coordinator.GetComponent<TransformComponent>(entity).GetPosition() + glm::vec3(0.0f, 0.0f, 0.1f));
		}
		if (g_Input.GetKeyState(GLFW_KEY_A) >= 1) {
			g_Coordinator.GetComponent<TransformComponent>(entity).SetPosition(g_Coordinator.GetComponent<TransformComponent>(entity).GetPosition() + glm::vec3(-0.1f, 0.0f, 0.0f));
		}
		if (g_Input.GetKeyState(GLFW_KEY_D) >= 1) {
			g_Coordinator.GetComponent<TransformComponent>(entity).SetPosition(g_Coordinator.GetComponent<TransformComponent>(entity).GetPosition() + glm::vec3(0.1f, 0.0f, 0.0f));
		}
	}

	void Destroy(Entity entity) {
		std::cout << "Player Behaviour Destroyed" << std::endl;
	}

}