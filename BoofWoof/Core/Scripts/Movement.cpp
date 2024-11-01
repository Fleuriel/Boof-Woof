#include "pch.h"
#include "../Core/Logic/Behaviour.h"
#include "../Core/ECS/Coordinator.hpp"
#include "../Core/Input/Input.h"
#include "../Utilities/Components/CollisionComponent.hpp"

#pragma warning(push)
#pragma warning(disable: 6385 6386)
#include <Jolt/Physics/Body/Body.h>


namespace Movement
{
	inline void Start(Entity entity) 
	{
		(void)entity;
		std::cout << "Movement Behaviour Started" << std::endl;
	}

	//inline void Update(Entity entity) {
	//	
	//	if (g_Input.GetKeyState(GLFW_KEY_W) >= 1) {
	//		g_Coordinator.GetComponent<TransformComponent>(entity).SetPosition(g_Coordinator.GetComponent<TransformComponent>(entity).GetPosition() + glm::vec3(0.0f, 0.0f, -0.1f));
	//	}
	//	if (g_Input.GetKeyState(GLFW_KEY_S) >= 1) {
	//		g_Coordinator.GetComponent<TransformComponent>(entity).SetPosition(g_Coordinator.GetComponent<TransformComponent>(entity).GetPosition() + glm::vec3(0.0f, 0.0f, 0.1f));
	//	}
	//	if (g_Input.GetKeyState(GLFW_KEY_A) >= 1) {
	//		g_Coordinator.GetComponent<TransformComponent>(entity).SetPosition(g_Coordinator.GetComponent<TransformComponent>(entity).GetPosition() + glm::vec3(-0.1f, 0.0f, 0.0f));
	//	}
	//	if (g_Input.GetKeyState(GLFW_KEY_D) >= 1) {
	//		g_Coordinator.GetComponent<TransformComponent>(entity).SetPosition(g_Coordinator.GetComponent<TransformComponent>(entity).GetPosition() + glm::vec3(0.1f, 0.0f, 0.0f));
	//	}
	//	
	//}

	inline void Update(Entity entity) {
		// Check if the entity has the necessary components
		if (!g_Coordinator.HaveComponent<TransformComponent>(entity)) {
			std::cerr << "Entity " << entity << " does not have a TransformComponent." << std::endl;
			return;
		}

		if (!g_Coordinator.HaveComponent<CollisionComponent>(entity)) {
			std::cerr << "Entity " << entity << " does not have a CollisionComponent." << std::endl;
			return;
		}

		auto& transform = g_Coordinator.GetComponent<TransformComponent>(entity);
		auto& collisionComponent = g_Coordinator.GetComponent<CollisionComponent>(entity);

		glm::vec3 velocity(0.0f);

		if (g_Input.GetKeyState(GLFW_KEY_W) >= 1) {
			velocity.z -= 1.f;
		}
		if (g_Input.GetKeyState(GLFW_KEY_S) >= 1) {
			velocity.z += 1.f;
		}
		if (g_Input.GetKeyState(GLFW_KEY_A) >= 1) {
			velocity.x -= 1.f;
		}
		if (g_Input.GetKeyState(GLFW_KEY_D) >= 1) {
			velocity.x += 1.f;
		}

		// Update the physics body's velocity
		if (collisionComponent.GetPhysicsBody()) {
			collisionComponent.GetPhysicsBody()->SetLinearVelocity(JPH::Vec3(velocity.x, velocity.y, velocity.z));
		}

		// Optionally, update the transform to match the position set by the physics system.
	}

	inline void Destroy(Entity entity) 
	{
		(void)entity;
		std::cout << "Test Behaviour Destroyed" << std::endl;
	}

}

#pragma warning(pop)
