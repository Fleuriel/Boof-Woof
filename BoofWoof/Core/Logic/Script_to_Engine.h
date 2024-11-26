#pragma once
#ifndef SCRIPT_TO_ENGINE_H
#define SCRIPT_TO_ENGINE_H

#include "../ECS/Coordinator.hpp"
#include "BehaviourInterface.h"
#include "../Input/Input.h"
#include "../Utilities/Components/CollisionComponent.hpp"

#pragma warning(push)
#pragma warning(disable: 6385 6386)
#include <Jolt/Physics/Body/Body.h>

class Script_to_Engine : public engine_interface, public input_interface
{
public:

	// INPUT INTERFACE
	virtual input_interface& getInputSystem() override
	{
		return *this;
	}

	virtual bool isButtonPressed(std::uint32_t Key) override
	{
		return g_Input.GetKeyState(Key);
	}

	virtual bool isActionPressed(const char * action) override
	{
		return g_Input.IsActionPressed(action);
	}

	// END OF INPUT INTERFACE

	// ENGINE INTERFACE
	
	//Transform Component
	virtual glm::vec3 GetPosition(Entity entity) override
	{
		return g_Coordinator.GetComponent<TransformComponent>(entity).GetPosition();
	}

	virtual void SetPosition(Entity entity, glm::vec3 position) override
	{
		g_Coordinator.GetComponent<TransformComponent>(entity).SetPosition(position);
	}

	virtual bool HaveTransformComponent(Entity entity) override
	{
		return g_Coordinator.HaveComponent<TransformComponent>(entity);
	}


	//Collision Component
	virtual bool HaveCollisionComponent(Entity entity) override
	{
		return g_Coordinator.HaveComponent<CollisionComponent>(entity);
	}

	virtual bool HavePhysicsBody(Entity entity) override
	{
		return g_Coordinator.GetComponent<CollisionComponent>(entity).GetPhysicsBody() != nullptr;
	}

	virtual void SetVelocity(Entity entity, glm::vec3 inputVelocity) override
	{
		if (HaveCollisionComponent(entity) && HavePhysicsBody(entity))
		{
			auto* body = g_Coordinator.GetComponent<CollisionComponent>(entity).GetPhysicsBody();
			if (body->GetMotionType() == JPH::EMotionType::Dynamic)
			{
				JPH::Vec3 currentVelocity = body->GetLinearVelocity();

				// Combine gravity with player input velocity
				JPH::Vec3 newVelocity(inputVelocity.x, inputVelocity.y, inputVelocity.z);

				body->SetLinearVelocity(newVelocity);

				// std::cout << "Set Velocity: (" << newVelocity.GetX() << ", " << newVelocity.GetY() << ", " << newVelocity.GetZ() << ")" << std::endl;
			}
		}
		else
		{
			std::cerr << "Entity " << entity << " does not have a valid CollisionComponent or PhysicsBody. Skipping SetVelocity." << std::endl;
		}
	}

	virtual glm::vec3 GetVelocity(Entity entity) override
	{
		if (HaveCollisionComponent(entity) && HavePhysicsBody(entity))
		{
			auto* body = g_Coordinator.GetComponent<CollisionComponent>(entity).GetPhysicsBody();
			if (body->GetMotionType() == JPH::EMotionType::Dynamic)
			{
				JPH::Vec3 velocity = body->GetLinearVelocity();
				return glm::vec3(velocity.GetX(), velocity.GetY(), velocity.GetZ());
			}
		}
		return glm::vec3(0.0f); // Return zero velocity for static or invalid entities
	}

};

#endif // !SCRIPT_TO_ENGINE_H

#pragma warning(pop)
