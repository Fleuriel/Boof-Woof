#include "../Core/Logic/Behaviour.h"
#include "../Core/ECS/Coordinator.hpp"
#include "../Core/Input/Input.h"


namespace Movement
{
	void Start(Entity entity) {
		std::cout << "Test Behaviour Started" << std::endl;
	}

	void Update(Entity entity) {
		
		if (g_Input.GetKeyState(GLFW_KEY_W) == 1) {
			std::cout << "W key pressed" << std::endl;
		}
		
	}

	void Destroy(Entity entity) {
		std::cout << "Test Behaviour Destroyed" << std::endl;
	}

}
