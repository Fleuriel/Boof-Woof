#pragma once
#include <iostream>
#include <vector>
#include <glm/glm.hpp>

struct Rex final : public Behaviour
{
    using Behaviour::Behaviour;

    std::vector<glm::vec3> path;
    int currentPathIndex = 0;
    bool followingPath = false;
    bool pathInitialized = false;
    float speed = 5.0f;
    float pathThreshold = 0.2f;
    bool isMovingRex = false;

    virtual void Init(Entity entity) override
    {
        std::cout << "[Rex] Initializing Pathfinding Component...\n";
        pathInitialized = false;
        followingPath = false;
        currentPathIndex = 0;
    }

    virtual void Update(Entity entity) override
    {
        std::cout << "Rex is updating" << std::endl;

        glm::vec3 currentPos = m_Engine.GetPosition(entity);
        std::cout << "[Pathfinding] Entity " << entity << " is currently at Position: ("
            << currentPos.x << ", " << currentPos.y << ", " << currentPos.z << ")" << std::endl;

        glm::vec3 velocity(0.0f);

        if (!pathInitialized)
        {
            std::cout << "[Pathfinding] Checking if entity " << entity << " has a pathfinding component..." << std::endl;

            if (m_Engine.HavePathfindingComponent(entity))
            {
                path = m_Engine.GetPath(entity);
                currentPathIndex = 0;
                followingPath = !path.empty();

                if (followingPath) {
                    std::cout << "[Pathfinding] Entity " << entity << " initialized with path of length " << path.size() << std::endl;
                }
                else {
                    std::cout << "[Pathfinding] Entity " << entity << " has no valid path." << std::endl;
                }
            }
            else {
                std::cout << "[Pathfinding] Entity " << entity << " does not have a pathfinding component!" << std::endl;
            }
            pathInitialized = true;
        }

        if (followingPath && currentPathIndex < path.size()) {
            glm::vec3 targetPos = path[currentPathIndex];

            // Lock Y-axis movement
            targetPos.y = currentPos.y;

            glm::vec3 direction = glm::normalize(targetPos - currentPos);

            // Ensure no division by zero
            if (glm::length(direction) > 0.0001f) {
                velocity = direction * speed;
            }
            else {
                velocity = glm::vec3(0.0f);
            }

            float distance = glm::length(targetPos - currentPos);
            std::cout << "[Pathfinding] Distance to next waypoint: " << distance << std::endl;

            if (distance <= pathThreshold)
            {
                std::cout << "[Pathfinding] Entity " << entity << " reached waypoint "
                    << currentPathIndex + 1 << std::endl;

                currentPathIndex++;

                if (currentPathIndex >= path.size()) {
                    followingPath = false;
                    velocity = glm::vec3(0.0f);
                    std::cout << "[Pathfinding] Entity " << entity << " has reached the final destination!" << std::endl;
                }
            }
            isMovingRex = true;
        }

        if (isMovingRex) {
            // Clamp velocity to avoid breaking the physics engine
            float maxAllowedSpeed = 10.0f;
            if (glm::length(velocity) > maxAllowedSpeed) {
                velocity = glm::normalize(velocity) * maxAllowedSpeed;
            }

            std::cout << "[Pathfinding] Applying velocity to Entity " << entity
                << " (" << velocity.x << ", " << velocity.y << ", " << velocity.z << ")" << std::endl;

            m_Engine.SetVelocity(entity, velocity);

            glm::vec3 newPos = m_Engine.GetPosition(entity);
            std::cout << "[Pathfinding] Entity " << entity << " new position after velocity applied: ("
                << newPos.x << ", " << newPos.y << ", " << newPos.z << ")" << std::endl;
        }
    }

    virtual const char* getBehaviourName() override
    {
        return "Rex";
    }
};
