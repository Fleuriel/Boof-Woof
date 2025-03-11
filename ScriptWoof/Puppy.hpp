#pragma once
#include <iostream>
#define UNUSED(x) (void)(x) // Macro to suppress warnings
#include <vector>
#include <map>

#define MAX_ENTITIES 5000
struct Puppy final : public Behaviour
{
    using Behaviour::Behaviour;

    std::vector<glm::vec3> path;
    int currentPathIndex = 0;
    bool followingPath = false;
    bool pathInitialized = false;
    float speed = 5.0f;
    float pathThreshold = 5.f;
    bool isMovingPuppy = false;
    //bool collected = false; // Track if the player collected the puppy
    Entity playerEntity = 5000; // Store player entity
    bool puppyGrounded = false; // Add this as a member variable

	std::map<Entity, bool> collected;

    virtual void Init(Entity entity) override
    {
        collected.clear();
        UNUSED(entity);
        pathInitialized = false;
        followingPath = false;
        isMovingPuppy = true;
        currentPathIndex = 0;
		collected.insert(std::pair<Entity, bool>(entity, false));

        // Get the player entity from the engine
        playerEntity = m_Engine.GetPlayerEntity();
        if (playerEntity != 5000)
        {
            std::cout << "[Puppy " << entity << "] Found Player Entity : " << playerEntity << std::endl;
        }
        else
        {
            std::cerr << "[Puppy" << entity << "] ERROR: Player entity not found!" << std::endl;
        }
    }

    virtual void Update(Entity entity) override
    {

		if (collected.find(entity) == collected.end())
		{
			collected.insert(std::pair<Entity, bool>(entity, false));
		}


  //      std::cout << "Puppy "<< entity;
		//collected[entity] ? std::cout << " collected\n" : std::cout << " not collected\n";

        if (!collected[entity])
        {
            // Check if the puppy is colliding with the player
            if (m_Engine.IsColliding(entity) && std::strcmp(m_Engine.GetCollidingEntityName(entity), "Player") == 0)
            {
                collected[entity] = true; // Player has collected the puppy
                std::cout << "[Puppy] Player collected the puppy!" << std::endl;
                pathInitialized = false; // Force re-initialization
            }
            else {
                return; // If not collected, don't move
            }
        }

        glm::vec3 currentPos = m_Engine.GetPosition(entity);
        glm::vec3 velocity(0.0f);
        float deltaTime = static_cast<float>(m_Engine.GetDeltaTime()); // Get the frame time

        if (collected[entity] && playerEntity != INVALID_ENT) 
        {

            // Get player position for movement
            glm::vec3 playerPos = m_Engine.GetPosition(playerEntity);
			float playerY = playerPos.y;

			// Move towards the player x and z position
			playerPos.y = currentPos.y; // Keep movement on the same Y-plane
            glm::vec3 direction = glm::normalize(playerPos - currentPos);

            if (glm::length(direction) > 0.0001f)
            {
                velocity = direction * speed;
            }

            float distance = glm::length(playerPos - currentPos);
            //if player y position is greater than 5, move puppy up
            if (playerY - currentPos.y > 1) {
                if (distance < 2) {
                    velocity = glm::vec3(0.0f);
                    std::cout << "[Puppy] Reached player!" << std::endl;
				}
				else {
					// Make the puppy jump towards the player
                    float gravity = 9.81f;
                    float jumpHeight = 1.5f;
                    float jumpVelocity = 1.2f * sqrt(2 * gravity * jumpHeight);

                    velocity.y = jumpVelocity;
                    puppyGrounded = false; // Puppy is now airborne
				}
            }
            if (distance <= pathThreshold)
            {
                velocity = glm::vec3(0.0f);
                //std::cout << "[Puppy] Reached player!" << std::endl;
            }

            
            if (distance >= 10) {
				// Teleport the puppy to the player behind
				glm::vec3 playerDirection = glm::normalize(currentPos - playerPos);
				glm::vec3 temppos = playerPos + playerDirection * 2.0f;
				temppos.y = playerPos.y;
				
                std::cout << "[DEBUG] Before SetPosition: " << currentPos.x << ", " << currentPos.y << ", " << currentPos.z << std::endl;

                // **Disable physics before teleporting**
                m_Engine.DisablePhysics(entity);

                // **Teleport the puppy**
                m_Engine.SetPosition(entity, temppos);

                // **Update physics transform**
                m_Engine.UpdatePhysicsTransform(entity);

                // **Re-enable physics**
                m_Engine.EnablePhysics(entity);

                std::cout << "[DEBUG] After SetPosition: " << m_Engine.GetPosition(entity).x
                    << ", " << m_Engine.GetPosition(entity).y
                    << ", " << m_Engine.GetPosition(entity).z << std::endl;
            }
		

            isMovingPuppy = true;
        }

        // **Gravity Fix**
        float gravityForce = 100.0f;  // Increased gravity
        float terminalVelocity = -100.0f; // Allow faster falling
        float velocityThreshold = 0.05f; // Small movement threshold
        glm::vec3 playerPos = m_Engine.GetPosition(playerEntity);
        float playerY = playerPos.y;

        if (!puppyGrounded)
        {
            velocity.y -= gravityForce * deltaTime;
            velocity.y = glm::max(velocity.y, terminalVelocity);

            // **Ground Detection (Ensures it actually lands)**
            static float groundTimer = 0.0f;
            if (glm::abs(velocity.y) < velocityThreshold)
            {
                groundTimer += deltaTime;

                // Only mark as grounded if very close to the ground
                if (groundTimer > 0.2f && currentPos.y <= playerY + 0.1f)
                {
                    puppyGrounded = true;
                    velocity.y = 0.0f;
                }
            }
            else
            {
                groundTimer = 0.0f;
            }
        }

        // Apply velocity
        if (isMovingPuppy)
        {
            m_Engine.SetRotationYawFromVelocity(entity, velocity);

            float maxAllowedSpeed = 10.0f;
            if (glm::length(glm::vec2(velocity.x, velocity.z)) > maxAllowedSpeed)
            {
                glm::vec3 horizontalVelocity = glm::normalize(glm::vec3(velocity.x, 0, velocity.z)) * maxAllowedSpeed;
                velocity.x = horizontalVelocity.x;
                velocity.z = horizontalVelocity.z;
            }

            m_Engine.SetVelocity(entity, velocity);
        }
    }


    virtual const char* getBehaviourName() override
    {
        return "Puppy";
    }
};
