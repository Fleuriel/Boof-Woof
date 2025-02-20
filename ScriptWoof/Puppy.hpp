#pragma once
#include <iostream>
#define UNUSED(x) (void)(x) // Macro to suppress warnings
#include <vector>

struct Puppy final : public Behaviour
{
    using Behaviour::Behaviour;

    std::vector<glm::vec3> path;
    int currentPathIndex = 0;
    bool followingPath = false;
    bool pathInitialized = false;
    float speed = 5.0f;
    float pathThreshold = 0.2f;
    bool isMovingPuppy = false;
    bool collected = false; // Track if the player collected the puppy
    Entity playerEntity = MAX_ENTITIES; // Store player entity

    virtual void Init(Entity entity) override
    {
        UNUSED(entity);
        pathInitialized = false;
        followingPath = false;
        isMovingPuppy = true;
        currentPathIndex = 0;

        // Get the player entity from the engine
        playerEntity = m_Engine.GetPlayerEntity();
        if (playerEntity != MAX_ENTITIES)
        {
            std::cout << "[Puppy] Found Player Entity: " << playerEntity << std::endl;
        }
        else
        {
            std::cerr << "[Puppy] ERROR: Player entity not found!" << std::endl;
        }
    }

    virtual void Update(Entity entity) override
    {
        if (!collected)
        {
            // Check if the puppy is colliding with the player
            if (m_Engine.IsColliding(entity) && std::strcmp(m_Engine.GetCollidingEntityName(entity), "Player") == 0)
            {
                collected = true; // Player has collected the puppy
                std::cout << "[Puppy] Player collected the puppy!" << std::endl;
                pathInitialized = false; // Force re-initialization
            }
            return; // If not collected, don't move
        }

        glm::vec3 currentPos = m_Engine.GetPosition(entity);
        glm::vec3 velocity(0.0f);

        // Ensure path updates dynamically when following the player
        if (collected && playerEntity != MAX_ENTITIES)
        {
            Entity playerNode = m_Engine.GetStartNode(playerEntity); // Get player node
            m_Engine.SetGoalNode(entity, playerNode); // Set the goal as the player

            if (!pathInitialized || m_Engine.GetGoalNode(entity) != playerNode)
            {
                path = m_Engine.GetPath(entity);
                std::cout << "[Puppy] Updated path to follow player. Path length: " << path.size() << std::endl;

                if (!path.empty())
                {
                    currentPathIndex = std::max(0, (int)path.size() - 3); // Stay a few steps behind
                    followingPath = true;
                }
                else
                {
                    std::cerr << "[Puppy] ERROR: No path to follow!" << std::endl;
                }

                pathInitialized = true;
            }
        }

        // Move along the path
        if (followingPath && currentPathIndex < path.size())
        {
            glm::vec3 targetPos = path[currentPathIndex];
            targetPos.y = currentPos.y; // Keep movement on the same Y-plane

            glm::vec3 direction = glm::normalize(targetPos - currentPos);
            if (glm::length(direction) > 0.0001f)
            {
                velocity = direction * speed;
            }

            float distance = glm::length(targetPos - currentPos);
            if (distance <= pathThreshold)
            {
                currentPathIndex++;
                if (currentPathIndex >= path.size())
                {
                    followingPath = false;
                    velocity = glm::vec3(0.0f);
                    std::cout << "[Puppy] Reached final destination!" << std::endl;
                }
            }

            isMovingPuppy = true;
        }

        // Apply velocity
        if (isMovingPuppy)
        {
            float maxAllowedSpeed = 10.0f;
            if (glm::length(velocity) > maxAllowedSpeed)
            {
                velocity = glm::normalize(velocity) * maxAllowedSpeed;
            }

            m_Engine.SetVelocity(entity, velocity);
            std::cout << "[Puppy] Moving with velocity: (" << velocity.x << ", " << velocity.y << ", " << velocity.z << ")" << std::endl;
        }
    }

    virtual const char* getBehaviourName() override
    {
        return "Puppy";
    }
};
