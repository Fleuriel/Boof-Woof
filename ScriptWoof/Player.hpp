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
			velocity.z -= 2.f;
			isMoving = true;
		}

		if (m_Engine.getInputSystem().isActionPressed("MoveLeft"))
		{
			//std::cout << "movingA" << std::endl;
			velocity.x -= 2.f;
			isMoving = true;
		}

		if (m_Engine.getInputSystem().isActionPressed("MoveBackward"))
		{
			//std::cout << "movingS" << std::endl;
			velocity.z += 2.f;
			isMoving = true;
		}

		if (m_Engine.getInputSystem().isActionPressed("MoveRight"))
		{
			//std::cout << "movingD" << std::endl;
			velocity.x += 2.f;
			isMoving = true;
		}

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
		std::cout << "It works" << std::endl;
	}

	virtual const char* getBehaviourName() override
	{
		return "Player";
	}
};
