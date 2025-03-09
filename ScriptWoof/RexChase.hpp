#pragma once
#include <iostream>
#define UNUSED(x) (void)(x)  // Define macro to suppress warnings
#include <vector>

struct RexChase final : public Behaviour
{
    using Behaviour::Behaviour;

    std::vector<glm::vec3> path;
    int currentPathIndex = 0;
    bool followingPath = false;
    bool pathInitialized = false;
    float speed = 5.0f;
    float pathThreshold = 0.2f;
    bool isMovingRex = false;
    bool chase = false;
    Entity PlayerNearestNode = NULL;
    Entity playerEntity = INVALID_ENT;

    glm::vec3 rexPosition = glm::vec3(0.0f);
    glm::vec3 rexRotation = glm::vec3(0.0f);

    // Create a state machine
    enum class State
    {
        START,
        CHASE
    };

    State state = State::START;

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
        if (m_Engine.GetTRtimer() <= 0.0f)
        {
           chase = true;
        }
        else {
			chase = false;
        }
		if (state == State::START)
		{
			state = State::CHASE;
            if (!pathInitialized) {
                if (m_Engine.HavePathfindingComponent(entity))
                {
                    path = m_Engine.GetPath(entity);
                    //std::cout << "[Pathfinding] Retrieved path of length " << path.size() << std::endl;
                    // print start and end node
                    std::cout << "Start node : " << m_Engine.GetStartNode(entity) << " , " << "End node : " << m_Engine.GetGoalNode(entity) << std::endl;

                    if (!path.empty())
                    {
                        currentPathIndex = 0;
                        followingPath = true; // <-- This was missing before
                        std::cout << "[Pathfinding] Path successfully retrieved! Entity " << entity << " will start moving." << std::endl;
                    }
                    else
                    {
                        followingPath = false;
                        isMovingRex = false;
                        std::cout << "[Pathfinding] ERROR: No valid path! Entity " << entity << " will NOT start moving." << std::endl;
                        return;
                    }
                }
                else
                {
                    //std::cout << "[Pathfinding] ERROR: No pathfinding component found!" << std::endl;
                }

                pathInitialized = true; // Set after we have attempted to load the path
            }
		}
        else if (state == State::CHASE) {
            /*
            if (PlayerNearestNode == NULL) {
                PlayerNearestNode = m_Engine.GetNearestNode(m_Engine.GetPlayerEntity());
            }
            else if (PlayerNearestNode != m_Engine.GetNearestNode(m_Engine.GetPlayerEntity())) {
                PlayerNearestNode = m_Engine.GetNearestNode(m_Engine.GetPlayerEntity());
                pathInitialized = false;
            }
            */

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
                    m_Engine.SetStartNode(entity, m_Engine.GetNearestNode(entity));
					PlayerNearestNode = m_Engine.GetNearestNode(m_Engine.GetPlayerEntity());
                    m_Engine.SetGoalNode(entity, PlayerNearestNode);
                    path = m_Engine.GetPath(entity);
                    //std::cout << "[Pathfinding] Retrieved path of length " << path.size() << std::endl;
                    // print start and end node
                    std::cout << "Start node : " << m_Engine.GetStartNode(entity) << " , " << "End node : " << m_Engine.GetGoalNode(entity) <<std::endl;

                    if (!path.empty())
                    {
                        currentPathIndex = 0;
                        followingPath = true; // <-- This was missing before
                        std::cout << "[Pathfinding] Path successfully retrieved! Entity " << entity << " will start moving." << std::endl;
                    }
                    else
                    {
                        followingPath = false;
                        isMovingRex = false;
                        std::cout << "[Pathfinding] ERROR: No valid path! Entity " << entity << " will NOT start moving." << std::endl;
                        return;
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

                //// *** ADD RAYCAST CHECK HERE ***
                //float rayDistance = 2.0f; // Check 2 meters ahead
                //Entity hitEntity = m_Engine.getPhysicsSystem().Raycast(currentPos, direction, rayDistance);

                //if (hitEntity != invalid_entity)
                //{
                //    std::cout << "[Rex] Object detected in front! Entity ID: " << hitEntity << std::endl;
                //    velocity = glm::vec3(0.0f); // Stop movement if an obstacle is in front
                //}

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
                        pathInitialized = false;
                        m_Engine.SetBuilt(entity, false);
                        
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

            if (PlayerNearestNode != m_Engine.GetNearestNode(m_Engine.GetPlayerEntity())) {
                PlayerNearestNode = m_Engine.GetNearestNode(m_Engine.GetPlayerEntity());
                pathInitialized = false;
			}
            // Apply velocity correctly
            if (isMovingRex)
            {
                // Rotate Rex to the direction he is moving
                m_Engine.SetRotationYawFromVelocity(entity, velocity);

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
    }

    virtual const char* getBehaviourName() override
    {
        return "RexChase";
    }

    bool CheckifPlayerInFront(Entity entity) {
        if (!m_Engine.HaveTransformComponent(entity)) {
            return false; // Ensure the entity has a TransformComponent
        }

        rexPosition = m_Engine.GetPosition(entity);
        rexRotation = m_Engine.GetRotation(entity); // Get yaw rotation

        // **Compute forward direction from Rex's yaw rotation**
        float yaw = rexRotation.y;
        glm::vec3 forwardDirection = glm::vec3(glm::cos(yaw), 0.0f, -glm::sin(yaw));

        float maxRayDistance = 10.0f;
        //float fovAngle = 30.0f; // 30-degree cone
        float horizontalFOVAngle = 5.0f; // Customize how wide the spread is
        float verticalFOVAngle = 10.0f;   // Customize how far up/down the rays spread
        int horizontalRays = 5; // Number of horizontal rays
        int verticalRays = 5;   // Number of vertical rays
        glm::vec3 rayOffset = glm::vec3(0.0f, 0.0f, 0.0f);

        std::vector<Entity> detectedObjects = m_Engine.getPhysicsSystem().ConeRaycast(
            entity, forwardDirection, maxRayDistance,
            horizontalRays, verticalRays,
            horizontalFOVAngle, verticalFOVAngle,
            rayOffset
        );

        if (!detectedObjects.empty()) {
            //std::cout << "[Rex] Cone Raycast Detected Entities:\n";
            for (Entity e : detectedObjects) {
                std::cout << "   - Entity ID: " << e << "\n";
                if (e == playerEntity) {
                    std::cout << "[Rex] Player detected in FOV.\n";
                    std::cout << playerEntity << std::endl;
                    return true;
                }
            }
        }
        std::cout << "[Rex] No objects detected in FOV.\n";
        return false;
    }
};
