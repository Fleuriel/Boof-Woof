#include <iostream>


struct Player final : public Behaviour
{
	using Behaviour::Behaviour;

	virtual void Update(Entity entity) override
	{
		glm::vec3 velocity(0.0f);
		bool isMoving = false;

		if (m_Engine.getInputSystem().isActionPressed("MoveForward"))
		{
			//std::cout << "movingW" << std::endl;
			// Get Camera Direction
			//float yaw = m_Engine.GetCameraYaw();

			velocity += m_Engine.GetCameraDirection(entity) * 2.f;
			
			std::cout << velocity.x << " " << velocity.z << std::endl;
			isMoving = true;

		}

		if (m_Engine.getInputSystem().isActionPressed("MoveLeft"))
		{
			// Rotate the velocity 90 degrees to the left
			glm::mat3 rotation = glm::mat3(
				0.0f, 0.0f, -1.0f,
				0.0f, 1.0f, 0.0f,
				1.0f, 0.0f, 0.0f
			);

			velocity += rotation * m_Engine.GetCameraDirection(entity) * 2.f;
			isMoving = true;
		}

		if (m_Engine.getInputSystem().isActionPressed("MoveBackward"))
		{
			//std::cout << "movingS" << std::endl;
			velocity += -m_Engine.GetCameraDirection(entity) * 2.f;
			isMoving = true;
		}

		if (m_Engine.getInputSystem().isActionPressed("MoveRight"))
		{
			//std::cout << "movingD" << std::endl;
			
			// Rotate the velocity 90 degrees to the right
			glm::mat3 rotation = glm::mat3(
				0.0f, 0.0f, 1.0f,
				0.0f, 1.0f, 0.0f,
				-1.0f, 0.0f, 0.0f
			);

			velocity += rotation * m_Engine.GetCameraDirection(entity) * 2.f;
			isMoving = true;
		}

		/*
		if (m_Engine.getInputSystem().isActionPressed("Jump"))
		{
			//std::cout << "jumping" << std::endl;
			velocity.y += 2.f;
			isMoving = true;
		}
		*/

		//// Check if the entity has a collision component and physics body before setting velocity
		//if (m_Engine.HaveCollisionComponent(entity) && m_Engine.HavePhysicsBody(entity))
		//{
		//	m_Engine.SetVelocity(entity, velocity);
		//}

		// Check if the entity has a collision component and physics body before setting velocity
		if (m_Engine.HaveCollisionComponent(entity) && m_Engine.HavePhysicsBody(entity))
		{
			if (isMoving)
			{
				//velocity *= m_Engine.GetRotation(entity);
				// Apply player input velocity
				m_Engine.SetVelocity(entity, velocity);
			}
			else
			{
				// Preserve gravity by only setting horizontal velocity to zero
				glm::vec3 currentVelocity = m_Engine.GetVelocity(entity);
				glm::vec3 stopVelocity(0.0f, currentVelocity.y, 0.0f); // Preserve Y-axis velocity (gravity)
				m_Engine.SetVelocity(entity, stopVelocity);
			}
		}

		// Debug output for velocity
		//std::cout << "Player Velocity: (" << velocity.x << ", " << velocity.y << ", " << velocity.z << ")" << std::endl;

		//std::cout << "Player Update" << std::endl;
		//std::cout << "It works" << std::endl;
	}

	virtual const char* getBehaviourName() override
	{
		return "Player";
	}
};
