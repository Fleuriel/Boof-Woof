#include "../Core/Logic/Behaviour.h"
#include "../Core/ECS/Coordinator.hpp"
#include "../Core/Input/Input.h"


namespace Movement
{
	void Start(Entity entity) {
		std::cout << "Test Behaviour Started" << std::endl;
	}

	void Update(Entity entity) {
		if (g_Input.GetKeyState(GLFW_KEY_W) == GLFW_PRESS | g_Input.GetKeyState(GLFW_REPEAT)) {
			std::cout << "W key pressed" << std::endl;
		}
		if (g_Input.GetKeyState(GLFW_KEY_A) <= 1) {
			std::cout << "A key pressed" << std::endl;
		}
		if (g_Input.GetKeyState(GLFW_KEY_S) <= 1) {
			std::cout << "S key pressed" << std::endl;
		}
		if (g_Input.GetKeyState(GLFW_KEY_D) <= 1) {
			std::cout << "D key pressed" << std::endl;
		}
		
	}

	void Destroy(Entity entity) {
		std::cout << "Test Behaviour Destroyed" << std::endl;
	}

}
