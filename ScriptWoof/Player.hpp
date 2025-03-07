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
	bool inRopeBreaker{ false }, touchingToy{ false }, cooldownActive{ false }, justplaypls{ false };
	double stunlockTimer = 2.0;	// 2.0 seconds
	double cooldownTimer = 0.0;
	bool jumpSoundPlayed = false;  // Add this as a new class member variable

	glm::vec3 PlayerPosition = glm::vec3(0.0f);
	glm::vec3 PlayerRotation = glm::vec3(0.0f);



	float footstepTimer = 0.0f;
	const float footstepInterval = 0.25f;  // Interval between footstep sounds (adjustable)

	bool hasJumped = false;  // Track if the player has jumped
	bool wasGrounded = true; // Track the previous grounded state
	float fallTime = 0.0f;  // ? New variable to track how long the player is in the air
	bool falling = false;  // ? New flag to track if the player was falling
	bool jumpInitiated = false;  // ? New flag to track intentional jumps
	std::string surfaceType = ""; // Tracks the surface the player is on




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


		std::vector<std::string> CarpetfootstepSounds = {
	"Corgi/DogCarpetFootsteps1.wav",
	"Corgi/DogCarpetFootsteps2.wav",
	"Corgi/DogCarpetFootsteps3.wav",
	"Corgi/DogCarpetFootsteps4.wav",
	"Corgi/DogCarpetFootsteps5.wav",
		};

			std::vector<std::string> WoodfootstepSounds = {
	"Corgi/DogWoodFootsteps1.wav",
	"Corgi/DogWoodFootsteps2.wav",
	"Corgi/DogWoodFootsteps3.wav",
	"Corgi/DogWoodFootsteps4.wav",
	"Corgi/DogWoodFootsteps5.wav",
	"Corgi/DogWoodFootsteps6.wav",
			};

	
	std::vector<std::string> jumpSounds = {
		"Corgi/Jump_001 1.wav",
		"Corgi/Jump_002 1.wav",
		"Corgi/Jump_003 1.wav",
		"Corgi/Jump_004 1.wav",
		"Corgi/Jump_005 1.wav"
	};

	std::vector<std::string> landingSounds = {
		"Corgi/JumpLand_011.wav",
		"Corgi/JumpLand_021.wav",
		"Corgi/JumpLand_031.wav"
	};

	std::vector<std::string> rubberSqueakSounds = {
	"RubberSqueak1.wav",
	"RubberSqueak2.wav",
	"RubberSqueak3.wav"
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
		//std::cout << "Player Init" << std::endl;
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

			// If not exhausted, run. Else, stop running, walk.
			if (m_Engine.getInputSystem().isActionPressed("Sprint") && !m_Engine.GetExhausted())
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
						m_Engine.getAudioSystem().PlaySoundByFile(GetRandomSound(rubberSqueakSounds).c_str(), false, "SFX");
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

			// Allow movement only if the player is grounded & not in rope breaker or touching toy or stunned
			if (isGrounded && !inRopeBreaker && !touchingToy && !m_Engine.GetStunned())
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



			if (m_Engine.IsColliding(entity))
			{
				const char* collidingEntityName = m_Engine.GetCollidingEntityName(entity);

				if (std::strcmp(collidingEntityName, "Floor") == 0)
				{
					surfaceType = "Floor";
				}
				else if (std::strcmp(collidingEntityName, "Carpet") == 0)
				{
					surfaceType = "Carpet";
				}
				else if (std::strcmp(collidingEntityName, "WoodFloor") == 0)
				{
					surfaceType = "WoodFloor";
				}
				else if (std::strcmp(collidingEntityName, "WoodSteps") == 0)
				{
					surfaceType = "WoodSteps";
				}
				else if (std::strcmp(collidingEntityName, "FloorCastle") == 0)
				{
					surfaceType = "FloorCastle";
				}
			}

			// Footstep sound logic
			if (isMoving && !surfaceType.empty())  // Ensure a valid surfaceType before playing sound
			{
				footstepTimer -= static_cast<float>(m_Engine.GetDeltaTime());

				if (footstepTimer <= 0.0f)
				{
					std::string footstepSound;

					if (surfaceType == "Floor")
					{
						footstepSound = GetRandomSound(footstepSounds);
					}
					else if (surfaceType == "Carpet")
					{
						footstepSound = GetRandomSound(CarpetfootstepSounds);
					}
					else if (surfaceType == "WoodFloor")
					{
						footstepSound = GetRandomSound(WoodfootstepSounds);
					}
					else if (surfaceType == "WoodSteps")
					{
						footstepSound = GetRandomSound(WoodfootstepSounds);
					}
					else if (surfaceType == "FloorCastle")
					{
						footstepSound = GetRandomSound(footstepSounds);
					}

					if (!footstepSound.empty()) // Prevent playing an empty sound
					{
						m_Engine.getAudioSystem().PlaySoundByFile(footstepSound.c_str(), false, "SFX");
					}

					footstepTimer = footstepInterval;
				}
			}
			else
			{
				footstepTimer = 0.0f; // Reset when not moving
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

				if (!jumpSoundPlayed) {  // ? Ensure jump sound plays only once per jump

					
						m_Engine.getAudioSystem().PlaySoundByFile(GetRandomSound(jumpSounds).c_str(), false, "SFX");
						jumpSoundPlayed = true;  // Mark that the jump sound has played
					}


				//std::cout << "[DEBUG] Player jumped. Jump velocity: " << jumpVelocity << std::endl;
				//std::cout << "[DEBUG] isGrounded: " << isGrounded << std::endl;
				isJumping = true;
				isMoving = true;
				hasJumped = true;  // Mark that the player has jumped
				jumpInitiated = true;  // ? Mark that the player has jumped intentionally


			}

			// Detect if the player starts falling after jumping
			if (wasGrounded && !isGrounded) {
				falling = true;
				fallTime = 0.0f;
			}

			// If the player is falling, increase fall time
			if (falling) {
				fallTime += static_cast<float>(m_Engine.GetDeltaTime());
			}

			// Detect if the player lands **only if they previously jumped**
			if (falling && isGrounded && fallTime > 0.2f && jumpInitiated) {
				m_Engine.getAudioSystem().PlaySoundByFile(GetRandomSound(landingSounds).c_str(), false, "SFX");

				hasJumped = false;
				jumpSoundPlayed = false;
				falling = false;
				jumpInitiated = false;  // ? Reset jump tracking after landing
				std::cout << "[DEBUG] Player landed after intentional jump!" << std::endl;
			}

			// Update grounded state at the end
			wasGrounded = isGrounded;



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

	void CheckForObjectsInFront(Entity rexEntity)
	{
		if (!m_Engine.HaveTransformComponent(rexEntity)) {
			return; // Ensure the entity has a TransformComponent
		}

		PlayerPosition = m_Engine.GetPosition(rexEntity);
		PlayerRotation = m_Engine.GetRotation(rexEntity); // Get yaw rotation

		// **Compute forward direction from Rex's yaw rotation**
		//float yaw = PlayerRotation.y;
		glm::vec3 forwardDirection = glm::vec3(0.0f, -1.0f, 0.0f); // ? Change to downward

		float maxRayDistance = 10.0f;
		float fovAngle = 30.0f; // 30-degree cone
		int horizontalRays = 5; // Number of horizontal rays
		int verticalRays = 3;   // Number of vertical rays

		std::vector<Entity> detectedObjects = m_Engine.getPhysicsSystem().ConeRaycast(
			rexEntity, forwardDirection, maxRayDistance, horizontalRays, verticalRays, fovAngle
		);

		//if (!detectedObjects.empty()) {
		//    std::cout << "[Rex] Cone Raycast Detected Entities:\n";
		//    for (Entity e : detectedObjects) {
		//        std::cout << "   - Entity ID: " << e << "\n";
		//    }
		//}
		//else {
		//    std::cout << "[Rex] No objects detected in FOV.\n";
		//}
	}

	void SingleRayCheck(Entity rexEntity, glm::vec3 currentPos) {
		PlayerRotation = m_Engine.GetRotation(rexEntity); // Get yaw rotation

		// **Compute forward direction from Rex's yaw rotation**
		float yaw = PlayerRotation.y;
		glm::vec3 forwardDirection = glm::vec3(glm::cos(yaw), 0.0f, -glm::sin(yaw));

		float maxDistance = 10.0f; // Distance to check

		// Just shoot a single ray in front
		Entity hitEntity = m_Engine.getPhysicsSystem().Raycast(currentPos, forwardDirection, maxDistance, rexEntity);

		if (hitEntity != -1) {
			std::cout << "[Rex] Single Ray Test: Object detected in front! Entity ID: " << hitEntity << std::endl;
		}
		else {
			std::cout << "[Rex] Single Ray Test: No objects detected in front." << std::endl;
		}
	}



};