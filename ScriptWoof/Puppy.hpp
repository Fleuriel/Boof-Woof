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

        if (collected[entity] && playerEntity != INVALID_ENTITY) 
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
					
				}
            }
            if (distance <= pathThreshold)
            {
                velocity = glm::vec3(0.0f);
                //std::cout << "[Puppy] Reached player!" << std::endl;
            }

            
            if (distance >= 10) {
				// Teleport the puppy to the player
				glm::vec3 temppos = playerPos;
                temppos.y += 2;
                //m_Engine.SetPosition(entity, playerPos);

                std::cout << "[DEBUG] Before SetPosition: " << currentPos.x << ", " << currentPos.y << ", " << currentPos.z << std::endl;

                // **Disable physics before teleporting**
                m_Engine.DisablePhysics(entity);

                // **Teleport the puppy**
                m_Engine.SetPosition(entity, glm::vec3(30.0f, 0.7f, -10.0f));

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

        // Apply velocity
        if (isMovingPuppy)
        {
			m_Engine.SetRotationYawFromVelocity(entity, velocity);

			// Clamp the speed to a maximum value
            float maxAllowedSpeed = 10.0f;
            if (glm::length(velocity) > maxAllowedSpeed)
            {
                velocity = glm::normalize(velocity) * maxAllowedSpeed;
            }

            m_Engine.SetVelocity(entity, velocity);
            //std::cout << "[Puppy] Moving with velocity: (" << velocity.x << ", " << velocity.y << ", " << velocity.z << ")" << std::endl;
        }
    }


    virtual const char* getBehaviourName() override
    {
        return "Puppy";
    }
};
