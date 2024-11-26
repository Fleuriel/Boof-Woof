#include <iostream>


struct Player final : public Behaviour
{
	using Behaviour::Behaviour;
	glm::vec3 velocity;
	float speed;
	bool isMoving;
	bool isJumping;

	virtual void Init(Entity entity) override
	{
		std::cout << "Player Init" << std::endl;
		
		isJumping = false;
	}

	virtual void Update(Entity entity) override
	{
		velocity = glm::vec3(0.0f, 0.0f, 0.0f);
		isMoving = false;

		if (m_Engine.getInputSystem().isActionPressed("Sprint"))
		{
			speed = 5.0f;
		}
		else {
			speed = 3.0f;
		}
		
		if (m_Engine.HaveCameraComponent(entity)) {

			if (m_Engine.getInputSystem().isActionPressed("MoveForward"))
			{
				//std::cout << "movingW" << std::endl;
				// Get Camera Direction
				//float yaw = m_Engine.GetCameraYaw();

				velocity.x += m_Engine.GetCameraDirection(entity).x * speed;
				velocity.z += m_Engine.GetCameraDirection(entity).z * speed;
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
				velocity += rotation * glm::vec3(m_Engine.GetCameraDirection(entity).x, 0.f, m_Engine.GetCameraDirection(entity).y) * speed;
				isMoving = true;
			}

			if (m_Engine.getInputSystem().isActionPressed("MoveBackward"))
			{
				//std::cout << "movingS" << std::endl;
				velocity.x += -m_Engine.GetCameraDirection(entity).x * speed;
				velocity.z += -m_Engine.GetCameraDirection(entity).z * speed;
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

				velocity += rotation * glm::vec3(m_Engine.GetCameraDirection(entity).x, 0.f, m_Engine.GetCameraDirection(entity).y) * speed;
				isMoving = true;
			}
		}
		else {
			if (m_Engine.getInputSystem().isActionPressed("MoveForward"))
			{
				velocity.z -= speed;
				isMoving = true;
			}

			if (m_Engine.getInputSystem().isActionPressed("MoveLeft"))
			{
				velocity.x -= speed;
				isMoving = true;
			}

			if (m_Engine.getInputSystem().isActionPressed("MoveBackward"))
			{
				velocity.z += speed;
				isMoving = true;
			}

			if (m_Engine.getInputSystem().isActionPressed("MoveRight"))
			{
				velocity.x += speed;
				isMoving = true;
			}
		}

		
		if (m_Engine.getInputSystem().isActionPressed("Jump")) {
			float gravity = 9.81f;      // Use your engine's gravity value
			float jumpHeight = 0.25f;    // Desired jump height in units
			float jumpVelocity = sqrt(speed * gravity * jumpHeight);
			velocity.y += jumpVelocity; // Add upward velocity for the jump

			std::cout << "Player jumped with velocity: " << jumpVelocity << std::endl;
			isMoving = true;
			isJumping = true;
		}
		

		if (m_Engine.getInputSystem().isActionPressed("Bark"))
		{
			std::cout << "Bark" << std::endl;

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
				if (m_Engine.HaveCameraComponent(entity))
					std::cout << "has camera" << std::endl;
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
			/*
			if (m_Engine.GetPosition(entity).y == 0.0f)
			{
				isJumping = false;
			}
			*/
		}

		// Debug output for velocity
		//std::cout << "Player Velocity: (" << velocity.x << ", " << velocity.y << ", " << velocity.z << ")" << std::endl;

		//std::cout << "Player Update" << std::endl;
		//std::cout << "It works" << std::endl;
		// Comment for fun
	}

	virtual const char* getBehaviourName() override
	{
		return "Player";
	}
};
