#pragma once
#include <iostream>
#define UNUSED(x) (void)(x)  // Define macro to suppress warnings
#include <vector>

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
	bool returningtoStart = false;
    
    // Create a state machine
    enum class State
    {
        PATROL,
		CHASE
	};

	State state = State::PATROL;

    virtual void Init(Entity entity) override
    {
        //std::cout << "[Rex] Initializing Pathfinding Component..." << std::endl;
        UNUSED(entity);
        pathInitialized = false;
        followingPath = false;
        isMovingRex = false;
        currentPathIndex = 0;
    }

    virtual void Update(Entity entity) override
    {
        glm::vec3 currentPos = m_Engine.GetPosition(entity);
        //std::cout << "[Pathfinding] Entity " << entity << " is currently at Position: ("
        //    << currentPos.x << ", " << currentPos.y << ", " << currentPos.z << ")" << std::endl;

        glm::vec3 velocity(0.0f);

        // Ensure path is properly initialized after resetting the scene
        if (!pathInitialized)
        {
            //std::cout << "[Pathfinding] Checking if entity " << entity << " has a pathfinding component..." << std::endl;

            if (m_Engine.HavePathfindingComponent(entity))
            {
                path = m_Engine.GetPath(entity);
                std::cout << "[Pathfinding] Retrieved path of length " << path.size() << std::endl;
                // print start and end node
                std::cout << "Start node : " << m_Engine.GetStartNode(entity) << " , " << "End node : " << m_Engine.GetGoalNode(entity);

                if (!path.empty())
                {
                    currentPathIndex = 0;
                    followingPath = true; // <-- This was missing before
                    std::cout << "[Pathfinding] Path successfully retrieved! Entity " << entity << " will start moving." << std::endl;
                }
                else
                {
                    //std::cout << "[Pathfinding] ERROR: Path is empty after scene reload!" << std::endl;
                }
            }
            else
            {
                //std::cout << "[Pathfinding] ERROR: No pathfinding component found!" << std::endl;
            }

            pathInitialized = true; // Set after we have attempted to load the path
        }

        // Fix: Ensure followingPath is set to true if the path is valid
        if (path.size() > 0 && !followingPath)
        {
            //std::cout << "[Pathfinding] WARNING: Path exists but followingPath is FALSE! Fixing..." << std::endl;
            followingPath = true;
        }

        // Move towards the next waypoint
        if (followingPath && currentPathIndex < path.size())
        {
            glm::vec3 targetPos = path[currentPathIndex];

            // Lock Y-axis movement to keep the entity on the ground
            targetPos.y = currentPos.y;

            glm::vec3 direction = glm::normalize(targetPos - currentPos);

            //// Debugging direction calculation
            //std::cout << "[Pathfinding] Calculated direction vector: ("
            //    << direction.x << ", " << direction.y << ", " << direction.z << ")" << std::endl;

            // Ensure no division by zero and check if movement is happening
            if (glm::length(direction) > 0.0001f)
            {
                velocity = direction * speed;
            }
            else
            {
                //std::cout << "[Pathfinding] WARNING: Direction vector too small, setting velocity to zero." << std::endl;
                velocity = glm::vec3(0.0f);
            }

            float distance = glm::length(targetPos - currentPos);
            //std::cout << "[Pathfinding] Moving towards waypoint " << currentPathIndex + 1 << " at (" << targetPos.x << ", " << targetPos.y << ", " << targetPos.z << ")" << std::endl;
            //std::cout << "[Pathfinding] Distance to next waypoint: " << distance << std::endl;

            // Check if the entity has reached the waypoint
            if (distance <= pathThreshold)
            {
                //std::cout << "[Pathfinding] Reached waypoint " << currentPathIndex + 1 << std::endl;
                currentPathIndex++;

                if (currentPathIndex >= path.size())
                {
                    
					followingPath = false;                // Commented out to allow for continuous path following
                    currentPathIndex = 0;
                    velocity = glm::vec3(0.0f);
                    //std::cout << "[Pathfinding] Entity " << entity << " has reached the final destination!" << std::endl;
                    if (state == State::PATROL)
                    {
						//returningtoStart  = !returningtoStart;
                        //if (returningtoStart) {
                            Entity temp = m_Engine.GetStartNode(entity);
                            m_Engine.SetStartNode(entity, m_Engine.GetGoalNode(entity));
                            m_Engine.SetGoalNode(entity, temp);
							pathInitialized = false;
							m_Engine.SetBuilt(entity, false);
                            //std::cout << "Looping Path" << std::endl;
                        //}
                    }
                }
            }
            isMovingRex = true;
        }
        else if (!followingPath)
        {
            //std::cout << "[Pathfinding] No path to follow or already at the destination." << std::endl;
            //std::cout << "[Pathfinding] Debugging: pathInitialized = " << pathInitialized
            //    << ", followingPath = " << followingPath << ", path size = " << path.size() << std::endl;
        }

        // Apply velocity correctly
        if (isMovingRex)
        {
            // Clamp velocity to avoid breaking the physics engine
            float maxAllowedSpeed = 10.0f;
            if (glm::length(velocity) > maxAllowedSpeed)
            {
                velocity = glm::normalize(velocity) * maxAllowedSpeed;
            }

            //std::cout << "[Pathfinding] Applying velocity to Entity " << entity
            //    << " (" << velocity.x << ", " << velocity.y << ", " << velocity.z << ")" << std::endl;

            m_Engine.SetVelocity(entity, velocity);

            //// Print new position after applying velocity
            //glm::vec3 newPos = m_Engine.GetPosition(entity);
            //std::cout << "[Pathfinding] Entity " << entity << " new position after velocity applied: ("
            //    << newPos.x << ", " << newPos.y << ", " << newPos.z << ")" << std::endl;
        }
        else
        {
            //std::cout << "[Pathfinding] Entity " << entity << " is not moving." << std::endl;
        }
    }

    virtual const char* getBehaviourName() override
    {
        return "Rex";
    }
};
