#include <iostream>
#define UNREFERENCED_PARAMETER(P)          (P)
#include <random>

struct Player final : public Behaviour
{
	using Behaviour::Behaviour;
	glm::vec3 velocity;
	glm::vec3 previousPosition; // Track the previous position
	float speed;
	bool isMoving;
	bool isJumping;
	bool isGrounded;
	std::vector<glm::vec3> path;
	int currentPathIndex;
	bool followingPath;
	float pathThreshold; // Distance threshold for reaching a waypoint
	bool pathInitialized = false;
	bool inRopeBreaker{ false };


	std::vector<std::string> footstepSounds = {
	"Corgi/Dog_Footsteps_Walk/Dog_Footstep_Walk_01.wav",
	"Corgi/Dog_Footsteps_Walk/Dog_Footstep_Walk_02.wav",
	"Corgi/Dog_Footsteps_Walk/Dog_Footstep_Walk_03.wav",
	"Corgi/Dog_Footsteps_Walk/Dog_Footstep_Walk_04.wav",
	"Corgi/Dog_Footsteps_Walk/Dog_Footstep_Walk_05.wav",
	"Corgi/Dog_Footsteps_Walk/Dog_Footstep_Walk_06.wav",
	"Corgi/Dog_Footsteps_Walk/Dog_Footstep_Walk_07.wav",
	"Corgi/Dog_Footsteps_Walk/Dog_Footstep_Walk_08.wav",
	"Corgi/Dog_Footsteps_Walk/Dog_Footstep_Walk_09.wav",
	"Corgi/Dog_Footsteps_Walk/Dog_Footstep_Walk_10.wav",
	"Corgi/Dog_Footsteps_Walk/Dog_Footstep_Walk_11.wav",
	"Corgi/Dog_Footsteps_Walk/Dog_Footstep_Walk_12.wav",
	"Corgi/Dog_Footsteps_Walk/Dog_Footstep_Walk_13.wav",
	"Corgi/Dog_Footsteps_Walk/Dog_Footstep_Walk_14.wav"
	};

	virtual void Init(Entity entity) override
	{
		std::cout << "Player Init" << std::endl;
		previousPosition = m_Engine.GetPosition(entity); // Initialize with the starting position
		isJumping = false;
		isMoving = false;
		isGrounded = true;
		//std::vector<glm::vec3> path;
		//pathInitialized = false;
	}

	virtual void Update(Entity entity) override
	{
		if (!m_Engine.IsGamePaused())
		{
			//UNREFERENCED_PARAMETER(entity);
			velocity = glm::vec3(0.0f, 0.0f, 0.0f);
			isMoving = false;

			//// Debug for movement
			//glm::vec3 currentPos = m_Engine.GetPosition(entity);
			//std::cout << "[Pathfinding] Entity " << entity << " is currently at Position: ("
			//	<< currentPos.x << ", " << currentPos.y << ", " << currentPos.z << ")" << std::endl;

			//double deltaTime = m_Engine.GetDeltaTime(); // Get delta time
		
			//std::cout << "[DEBUG] Delta Time: " << deltaTime << std::endl;

			// Debug: Starting state
			//std::cout << "[DEBUG] Start of Update: Entity = " << entity
			//	<< ", isMoving = " << std::boolalpha << isMoving << std::endl;

			// Debug: Starting state
			//std::cout << "[DEBUG] Start of Update: isMoving = " << std::boolalpha << isMoving << std::endl;

			// Get grounded state from the CollisionComponent
			isGrounded = m_Engine.IsGrounded(entity);

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

			if (m_Engine.IsColliding(entity))
			{
				const char* collidingEntityName = m_Engine.GetCollidingEntityName(entity);
				if (std::strcmp(collidingEntityName, "Rope1") == 0)
				{
					inRopeBreaker = true;
				}
				else if (std::strcmp(collidingEntityName, "Rope2") == 0)
				{
					inRopeBreaker = true;
				}
			}
			else
			{
				inRopeBreaker = false;
			}


			// Allow movement only if the player is grounded
			if (isGrounded && !inRopeBreaker)
			{

				if (m_Engine.HaveCameraComponent(entity)) 
				{

					if (m_Engine.getInputSystem().isActionPressed("MoveForward"))
					{
						//std::cout << "movingW" << std::endl;
						// Get Camera Direction
						//float yaw = m_Engine.GetCameraYaw();

						velocity.x += m_Engine.GetCameraDirection(entity).x * 1.f;
						velocity.z += m_Engine.GetCameraDirection(entity).z * 1.f;
						isMoving = true;

					}

					if (m_Engine.getInputSystem().isActionPressed("MoveLeft"))
					{
						// Rotate the velocity 90 degrees to the left
						/*
						glm::mat3 rotation = glm::mat3(
							0.0f, 0.0f, -1.0f,
							0.0f, 1.0f, 0.0f,
							1.0f, 0.0f, 0.0f
						);
						velocity += rotation * glm::vec3(m_Engine.GetCameraDirection(entity).x, 0.f, m_Engine.GetCameraDirection(entity).y) * speed;
						*/

						velocity += glm::cross(m_Engine.GetCameraDirection(entity), glm::vec3(0.0f, -1.0f, 0.0f)) * 1.f;
						isMoving = true;
					}

					if (m_Engine.getInputSystem().isActionPressed("MoveBackward"))
					{
						//std::cout << "movingS" << std::endl;
						velocity.x += -m_Engine.GetCameraDirection(entity).x * 1.f;
						velocity.z += -m_Engine.GetCameraDirection(entity).z * 1.f;
						isMoving = true;
					}

					if (m_Engine.getInputSystem().isActionPressed("MoveRight"))
					{
						//std::cout << "movingD" << std::endl;

						// Rotate the velocity 90 degrees to the right
						/*
						glm::mat3 rotation = glm::mat3(
							0.0f, 0.0f, 1.0f,
							0.0f, 1.0f, 0.0f,
							-1.0f, 0.0f, 0.0f
						);

						velocity += rotation * glm::vec3(m_Engine.GetCameraDirection(entity).x, 0.f, m_Engine.GetCameraDirection(entity).y) * speed;
						isMoving = true;
						*/


						velocity += glm::cross(m_Engine.GetCameraDirection(entity), glm::vec3(0.0f, 1.0f, 0.0f)) * 1.f;
						isMoving = true;
					}
				}

				else 
				{
					if (m_Engine.getInputSystem().isActionPressed("MoveForward"))
					{
						velocity.z -= 1;
						isMoving = true;
					}

					if (m_Engine.getInputSystem().isActionPressed("MoveLeft"))
					{
						velocity.x -= 1;
						isMoving = true;
					}

					if (m_Engine.getInputSystem().isActionPressed("MoveBackward"))
					{
						velocity.z += 1;
						isMoving = true;
					}

					if (m_Engine.getInputSystem().isActionPressed("MoveRight"))
					{
						velocity.x += 1;
						isMoving = true;
					}
				}

				// Normalize the velocity
				velocity *= speed;

				//// Adjust movement for slopes
				//if (isOnSlope)
				//{
				//	velocity = velocity - glm::dot(velocity, surfaceNormal) * surfaceNormal;
				//}
			}



			if (isMoving)
			{
				// Use modern C++ random library to select a random sound
				static std::random_device rd; // Seed
				static std::mt19937 gen(rd()); // Mersenne Twister PRNG
				std::uniform_int_distribution<std::size_t> dis(0, footstepSounds.size() - 1);

				// Get a random sound ID
				std::string randomSound = footstepSounds[dis(gen)];

				// Play the randomly chosen sound
				m_Engine.getAudioSystem().PlaySoundById(randomSound.c_str());
			}


			// Debug: After processing input
			//std::cout << "[DEBUG] After Input Processing: isMoving = " << std::boolalpha << isMoving << std::endl;

			// Jump logic
			if (m_Engine.getInputSystem().isActionPressed("Jump") && isGrounded)
			{
				float gravity = 9.81f;
				float jumpHeight = 1.5f;
				float jumpVelocity = 1.2f * sqrt(2 * gravity * jumpHeight);

				velocity.y = jumpVelocity;
				m_Engine.SetGrounded(entity, false);

				//std::cout << "[DEBUG] Player jumped. Jump velocity: " << jumpVelocity << std::endl;
				//std::cout << "[DEBUG] isGrounded: " << isGrounded << std::endl;
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


			//if (m_Engine.getInputSystem().isActionPressed("Bark"))
			//{
			//	m_Engine.getAudioSystem().PlaySoundById("Corgi/CorgiBark1.wav");
			//}
		}
	}

	virtual const char* getBehaviourName() override
	{
		return "Player";
	}
};