#include <iostream>
#define UNREFERENCED_PARAMETER(P)          (P)
#include <random>
#include <unordered_set>

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
	bool wasGrounded = true; // Track the previous grounded state
	bool inRopeBreaker{ false }, touchingToy{ false }, cooldownActive{ false }, justplaypls{ false };
	double stunlockTimer = 2.0;	// 2.0 seconds
	double cooldownTimer = 0.0;

	float footstepTimer = 0.0f;
	const float footstepInterval = 0.25f;  // Interval between footstep sounds (adjustable)

	bool hasJumped = false;  // Track if the player has jumped


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

	std::vector<std::string> jumpSounds = {
		"Corgi/Jump_001 1.wav",
		"Corgi/Jump_002 1.wav",
		"Corgi/Jump_003 1.wav",
		"Corgi/Jump_004 1.wav",
		"Corgi/Jump_005 1.wav"
	};

	std::vector<std::string> landingSounds = {
		"Corgi/JumpLand_01.wav",
		"Corgi/JumpLand_02.wav",
		"Corgi/JumpLand_03.wav"
	};

	// Function to get a random sound from a vector
	std::string GetRandomSound(const std::vector<std::string>& soundList) {
		static std::random_device rd;
		static std::mt19937 gen(rd()); // Mersenne Twister PRNG
		std::uniform_int_distribution<std::size_t> dis(0, soundList.size() - 1);
		return soundList[dis(gen)];
	}

	virtual void Init(Entity entity) override
	{
		previousPosition = m_Engine.GetPosition(entity); // Initialize with the starting position
		isJumping = false;
		isMoving = false;
		isGrounded = true;
	}

	virtual void Update(Entity entity) override
	{
		if (!m_Engine.IsGamePaused())
		{
			velocity = glm::vec3(0.0f, 0.0f, 0.0f);
			isMoving = false;

			// Get grounded state
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

			// If not exhausted, run. Else, stop running, walk.
			if (m_Engine.getInputSystem().isActionPressed("Sprint") && !m_Engine.GetExhausted())
			{
				speed = 5.0f;
			}
			else {
				speed = 3.0f;
			}

			// Preserve horizontal velocity when grounded
			glm::vec3 currentVelocity = m_Engine.GetVelocity(entity);
			if (isGrounded)
			{
				currentVelocity.x = 0.0f;
				currentVelocity.z = 0.0f;
				m_Engine.SetVelocity(entity, currentVelocity);
			}

			// If the player jumps, mark them as having jumped
			if (m_Engine.getInputSystem().isActionPressed("Jump") && isGrounded)
			static const std::unordered_set<std::string> ropeEntities = { "Rope1", "Rope2" };
			static const std::unordered_set<std::string> toyEntities = { "Bone", "TennisBall" };

			if (m_Engine.IsColliding(entity))
			{
				const char* collidingEntityName = m_Engine.GetCollidingEntityName(entity);
				std::string entityName(collidingEntityName); // Convert C-string to std::string for easy lookup

				if (ropeEntities.count(entityName))
				{
					inRopeBreaker = true;
				}
				else if (toyEntities.count(entityName) && !cooldownActive)
				{
					touchingToy = true;

					if (!justplaypls) 
					{
						m_Engine.getAudioSystem().PlaySoundByFile("ToyTouch.wav", false, "SFX");
						double currTimer = m_Engine.GetTimerTiming();
						double newTimer = currTimer - 10.0;
						m_Engine.SetTimerTiming(newTimer);
						justplaypls = true;
					}
				}
			}
			else
			{
				inRopeBreaker = false;
			}

			if (touchingToy) 
			{
				m_Engine.SetTouched(true);

				if (stunlockTimer > 0.0)
				{
					stunlockTimer -= m_Engine.GetDeltaTime();
				}
				else 
				{
					// Stunlock is over, now enter cooldown phase
					m_Engine.SetTouched(false);
					m_Engine.SetCollidingEntityName(entity);

					cooldownTimer = 0.0;
					cooldownActive = true;
					touchingToy = false;
				}
			}

			if (cooldownActive)
			{
				cooldownTimer += m_Engine.GetDeltaTime();

				if (cooldownTimer >= 2.0)
				{
					stunlockTimer = 2.0;  //  Reset stunlock timer for next interaction
					cooldownTimer = 0.0;  //  Reset cooldown timer
					justplaypls = false;
					cooldownActive = false;  //  Allow player to be stunned again
				}
			}

			// Allow movement only if the player is grounded & not in rope breaker or touching toy
			if (isGrounded && !inRopeBreaker && !touchingToy)
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
				footstepTimer -= static_cast<float>(m_Engine.GetDeltaTime());

				if (footstepTimer <= 0.0f) // 
				{
					static std::random_device rd; // Seed
					static std::mt19937 gen(rd()); // Mersenne Twister PRNG
					std::uniform_int_distribution<std::size_t> dis(0, footstepSounds.size() - 1);

					// Get a random sound ID
					std::string randomSound = footstepSounds[dis(gen)];

					m_Engine.getAudioSystem().PlaySoundByFile(randomSound.c_str(), false, "SFX");

					footstepTimer = footstepInterval;
				}
			}
			else
			{
				footstepTimer = 0.0f; // Reset timer when not moving
			}



			// Debug: After processing input
			//std::cout << "[DEBUG] After Input Processing: isMoving = " << std::boolalpha << isMoving << std::endl;

			// Jump logic - don't allow jump when in rope breaker or touching toys
			if (m_Engine.getInputSystem().isActionPressed("Jump") && isGrounded && !inRopeBreaker && !touchingToy)
			{
				float gravity = 9.81f;
				float jumpHeight = 1.5f;
				float jumpVelocity = 1.2f * sqrt(2 * gravity * jumpHeight);

				velocity.y = jumpVelocity;
				m_Engine.SetGrounded(entity, false);

				// Play a random jump sound
				m_Engine.getAudioSystem().PlaySoundByFile(GetRandomSound(jumpSounds).c_str(), false, "SFX");

				isJumping = true;
				isMoving = true;
				hasJumped = true;  // Mark that the player has jumped
			}

			// If the player lands and has jumped before, play the landing sound
			if (isGrounded && !wasGrounded && hasJumped)
			{
				// Play a random landing sound
				m_Engine.getAudioSystem().PlaySoundByFile(GetRandomSound(landingSounds).c_str(), false, "SFX");

				hasJumped = false;  // Reset jump tracking after landing
			}

			wasGrounded = isGrounded;  // Update previous grounded state


			// Movement Logic
			if (isGrounded)
			{
				if (m_Engine.getInputSystem().isActionPressed("MoveForward"))
				{
					velocity.x += m_Engine.GetCameraDirection(entity).x * 1.f;
					velocity.z += m_Engine.GetCameraDirection(entity).z * 1.f;
					isMoving = true;
				}

				if (m_Engine.getInputSystem().isActionPressed("MoveLeft"))
				{
					velocity += glm::cross(m_Engine.GetCameraDirection(entity), glm::vec3(0.0f, -1.0f, 0.0f)) * 1.f;
					isMoving = true;
				}

				if (m_Engine.getInputSystem().isActionPressed("MoveBackward"))
				{
					velocity.x += -m_Engine.GetCameraDirection(entity).x * 1.f;
					velocity.z += -m_Engine.GetCameraDirection(entity).z * 1.f;
					isMoving = true;
				}

				if (m_Engine.getInputSystem().isActionPressed("MoveRight"))
				{
					velocity += glm::cross(m_Engine.GetCameraDirection(entity), glm::vec3(0.0f, 1.0f, 0.0f)) * 1.f;
					isMoving = true;
				}

				velocity *= speed;
			}

			// Footstep sound logic
			if (isMoving)
			{
				footstepTimer -= static_cast<float>(m_Engine.GetDeltaTime());

				if (footstepTimer <= 0.0f)
				{
					m_Engine.getAudioSystem().PlaySoundByFile(GetRandomSound(footstepSounds).c_str(), false, "SFX");
					footstepTimer = footstepInterval;
				}
			}
			else
			{
				footstepTimer = 0.0f; // Reset timer when not moving
			}

			// Apply velocity if the entity has a physics body
			if (m_Engine.HaveCollisionComponent(entity) && m_Engine.HavePhysicsBody(entity))
			{
				m_Engine.SetVelocity(entity, velocity);
			}

			// Check position change to determine movement
			glm::vec3 currentPosition = m_Engine.GetPosition(entity);
			isMoving = (currentPosition != previousPosition);
			previousPosition = currentPosition;
		}
	}

	virtual const char* getBehaviourName() override
	{
		return "Player";
	}
};
