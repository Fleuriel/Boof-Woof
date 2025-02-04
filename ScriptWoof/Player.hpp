#include <iostream>
#define UNREFERENCED_PARAMETER(P)          (P)


struct Player final : public Behaviour
{
	using Behaviour::Behaviour;
	glm::vec3 velocity;
	glm::vec3 previousPosition; // Track the previous position
	float speed;
	bool isMoving;
	bool isJumping;

	virtual void Init(Entity entity) override
	{
		//std::cout << "Player Init" << std::endl;
		previousPosition = m_Engine.GetPosition(entity); // Initialize with the starting position
		isJumping = false;
		isMoving = false;
	}

	virtual void Update(Entity entity) override
	{
		UNREFERENCED_PARAMETER(entity);
		velocity = glm::vec3(0.0f, 0.0f, 0.0f);
		isMoving = false;

		//double deltaTime = m_Engine.GetDeltaTime(); // Get delta time
		
		//std::cout << "[DEBUG] Delta Time: " << deltaTime << std::endl;

		// Debug: Starting state
		//std::cout << "[DEBUG] Start of Update: Entity = " << entity
		//	<< ", isMoving = " << std::boolalpha << isMoving << std::endl;

		// Debug: Starting state
		//std::cout << "[DEBUG] Start of Update: isMoving = " << std::boolalpha << isMoving << std::endl;

		// Get grounded state from the CollisionComponent
		bool isGrounded = m_Engine.HaveCollisionComponent(entity) ? m_Engine.IsGrounded(entity) : false;

		//std::cout << "[DEBUG] isGrounded = " << std::boolalpha << isGrounded << std::endl;

		if (m_Engine.getInputSystem().isActionPressed("Sprint"))
		{
			speed = 5.0f;
		}
		else {
			speed = 3.0f;
		}

		// Preserve horizontal velocity when jumping
		glm::vec3 currentVelocity = m_Engine.GetVelocity(entity);

		// Stop horizontal movement if grounded
		if (isGrounded)
		{
			if (currentVelocity.x != 0.0f || currentVelocity.z != 0.0f)
			{
				//std::cout << "[DEBUG] Player landed, stopping horizontal movement. Previous velocity: ("
				//	<< currentVelocity.x << ", " << currentVelocity.y << ", " << currentVelocity.z << ")" << std::endl;
			}
			// Set horizontal velocity to zero
			currentVelocity.x = 0.0f;
			currentVelocity.z = 0.0f;

			// Apply the stopped velocity
			m_Engine.SetVelocity(entity, currentVelocity);

			// Debug: Velocity after stopping
			//std::cout << "[DEBUG] Velocity after landing: ("
			//	<< currentVelocity.x << ", " << currentVelocity.y << ", " << currentVelocity.z << ")" << std::endl;
		}

		// Allow movement only if the player is grounded
		if (isGrounded)
		{
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

		}
		if (isMoving)
		{
			m_Engine.getAudioSystem().PlaySoundById("Corgi/Dog_Footsteps_Walk/Dog_Footstep_Walk_01.wav");
		}


		// Debug: After processing input
		//std::cout << "[DEBUG] After Input Processing: isMoving = " << std::boolalpha << isMoving << std::endl;

		// Jump logic
		if (m_Engine.getInputSystem().isActionPressed("Jump") && isGrounded)
		{
			float gravity = 9.81f;
			float jumpHeight = 2.0f;
			float jumpVelocity = sqrt(2 * gravity * jumpHeight);

			velocity.y = jumpVelocity;
			m_Engine.SetGrounded(entity, false);

			//std::cout << "[DEBUG] Player jumped. Jump velocity: " << jumpVelocity << std::endl;
			isJumping = true;
			isMoving = true;
		}

		// Check if the entity has a collision component and physics body before setting velocity
		if (m_Engine.HaveCollisionComponent(entity) && m_Engine.HavePhysicsBody(entity))
		{
			if (isMoving)
			{
				if (m_Engine.HaveCameraComponent(entity))
				{
					//std::cout << "has camera" << std::endl;

					// Apply player input velocity
					m_Engine.SetVelocity(entity, velocity);
				}

				//std::cout << "[DEBUG] Applying velocity: ("
				//	<< velocity.x << ", " << velocity.y << ", " << velocity.z << ")" << std::endl;

				m_Engine.SetVelocity(entity, velocity);
			}
			//else
			//{
			//	std::cout << "inside not isMoving" << std::endl;

			//	// Preserve gravity by only setting horizontal velocity to zero
			//	glm::vec3 currentVelocity = m_Engine.GetVelocity(entity);
			//	glm::vec3 stopVelocity(0.0f, currentVelocity.y, 0.0f); // Preserve Y-axis velocity (gravity)
			//	m_Engine.SetVelocity(entity, stopVelocity);
			//}
			/*
			if (m_Engine.GetPosition(entity).y == 0.0f)
			{
				isJumping = false;
			}
			*/
		}

		// Check position change to determine movement
		glm::vec3 currentPosition = m_Engine.GetPosition(entity);
		if (currentPosition != previousPosition)
		{
			isMoving = true;
		}
		else 
		{
			isMoving = false;
		}
		// Update the previous position
		previousPosition = currentPosition;

		// Debug: End of update
		//std::cout << "[DEBUG] End of Update: isMoving = " << std::boolalpha << isMoving << std::endl;

		// Debug output for velocity
		//std::cout << "Player Velocity: (" << velocity.x << ", " << velocity.y << ", " << velocity.z << ")" << std::endl;

		//std::cout << "Player Update" << std::endl;
		//std::cout << "It works" << std::endl;
		// Comment for fun


		if (m_Engine.getInputSystem().isActionPressed("Bark"))
		{
			m_Engine.getAudioSystem().PlaySoundById("Corgi/CorgiBark1.wav");
		}
	}

	virtual const char* getBehaviourName() override
	{
		return "Player";
	}
};