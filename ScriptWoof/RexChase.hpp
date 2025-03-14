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
    bool inPlay = false;
    bool gravitySet = false;
	Entity EntityNearestNode = NULL;
    Entity PlayerNearestNode = NULL;
    Entity playerEntity = INVALID_ENT;

    glm::vec3 rexPosition = glm::vec3(0.0f);
    glm::vec3 rexRotation = glm::vec3(0.0f);

    glm::vec3 previousPlayerPosition = glm::vec3(0.0f);  // New: Store player's last position
    float movementThreshold = 0.1f;  // New: Define a movement threshold to prevent unnecessary updates

    bool sawPlayer{ false };

    enum State
    {
        TELEPORTING,
        MOVING,
		ATTACKING,
        SEARCHING,
		IDLE
    };

	State state = IDLE;

    virtual void Init(Entity entity) override
    {
        //std::cout << "[Rex] Initializing Pathfinding Component..." << std::endl;
        UNUSED(entity);
        pathInitialized = false;
        followingPath = false;
        isMovingRex = false;
        currentPathIndex = 0;
        state = IDLE;
        inPlay = false;

        playerEntity = INVALID_ENT;
		// FOR TESTING PURPOSES + line 96
        //state = MOVING;
    }

    virtual void Update(Entity entity) override
    {
        // Check if Rex's physics body exists before setting gravity
        if (!gravitySet && m_Engine.HavePhysicsBody(entity))
        {
            m_Engine.getPhysicsSystem().SetEntityGravityFactor(entity, 0.0f);
            //std::cout << "[Physics] Gravity factor for Rex set to 0!" << std::endl;
            gravitySet = true;  // Mark as set so we don't repeat it
        }

        if (!m_Engine.IsGamePaused()) {
            glm::vec3 currentPos = m_Engine.GetPosition(entity);
            glm::vec3 velocity(0.0f); 

            // **Ground Check Implementation (Continuous Falling)**
            float maxGroundCheckDistance = 1.50f;
            glm::vec3 downward = glm::vec3(0.0f, -1.0f, 0.0f);
            Entity groundEntity = m_Engine.getPhysicsSystem().Raycast(currentPos, downward, maxGroundCheckDistance, entity);

            if (m_Engine.GetTRtimer() <= 0.0f && state == IDLE)
            {
                state = TELEPORTING;
            }
            else if (m_Engine.GetTRtimer() > 0.0f)
            {
				// COMMENTED OUT FOR TESTING PURPOSES
                return;
            }

            if (playerEntity == INVALID_ENT)
            {
                playerEntity = m_Engine.GetPlayerEntity();
            }

            Entity PlayerNodeCheck = m_Engine.GetNearestNode(m_Engine.GetPlayerEntity());
            EntityNearestNode = m_Engine.GetNearestNode(entity);
            //std::cout << "Player Node : " << PlayerNodeCheck << " , " << "Rex Node : " << EntityNearestNode << std::endl;
            //std::cout << state << std::endl;


            if (state != TELEPORTING) {
                
                // If player is in front
                if (CheckifPlayerInFront(entity)) {
                    state = ATTACKING;
                    std::cout << "Player is in front of Rex" << std::endl;
                }
                // if player nearest node is the same as rex nearest node, stop and look around
                else if (EntityNearestNode == PlayerNodeCheck) {
                    state = ATTACKING;
                    PlayerNearestNode = PlayerNodeCheck;
                    std::cout << "Player is at the same node as Rex" << std::endl;
                }
                else if (PlayerNearestNode != PlayerNodeCheck) {
                    PlayerNearestNode = PlayerNodeCheck;
                    pathInitialized = false;
                    std::cout << "Player moved to a new node" << std::endl;
                    state = MOVING;
                }
                else {
                    /*
                    if (state != MOVING) {
                        pathInitialized = false;
                        state = MOVING;
                        std::cout << "Player is not in front of Rex, find him" << std::endl;
                    }
                    */
                    //std::cout << "Carry On" << std::endl;
                }
                
            }
            

            switch (state) 
            {
                
                case TELEPORTING:
                {
                    //Teleport Rex into the scene
                    // **Disable physics before teleporting**
                    m_Engine.DisablePhysics(entity);

                    // **Teleport the start**
                    m_Engine.SetPosition(entity, glm::vec3(20.f, 1.5f, 3.f));

                    // **Update physics transform**
                    m_Engine.UpdatePhysicsTransform(entity);

                    // **Re-enable physics**
                    m_Engine.EnablePhysics(entity);
                    gravitySet = false;  // Ensure it's reapplied in next frame

                    if (m_Engine.GetPosition(entity) != glm::vec3(20.f, 1.5f, 3.f)) {
						std::cout << "Failed to teleport Rex" << std::endl;
                        break;
                    }
                    inPlay = true;
                    state = MOVING;
                    PlayerNearestNode = PlayerNodeCheck;

                    m_Engine.SetDialogue(28);
                    break;
                }

                case MOVING:
                {
                    if (!pathInitialized)
                    {
                        if (m_Engine.HavePathfindingComponent(entity))
                        {
                            // Set the start and goal nodes
                            m_Engine.SetStartNode(entity, EntityNearestNode);
                            PlayerNearestNode = PlayerNodeCheck;
                            m_Engine.SetGoalNode(entity, PlayerNearestNode);

                            // Force a rebuild of the path
                            m_Engine.SetBuilt(entity, false); // Mark as needing update
                            m_Engine.ForcePathfindingUpdate();  // **This ensures path is recalculated immediately**
                            path = m_Engine.GetPath(entity);  // Fetch new path

                            std::cout << "[RexChase] ---------------------------------" << std::endl;
                            std::cout << "[RexChase] Start Node: " << m_Engine.GetStartNode(entity)
                                << ", End Node: " << m_Engine.GetGoalNode(entity) << std::endl;

                            // Debugging: Print the full new path
                            std::cout << "[RexChase] New Full Path: ";
                            for (size_t i = 0; i < path.size(); ++i) {
                                std::cout << "(" << path[i].x << ", " << path[i].y << ", " << path[i].z << ")";
                                if (i < path.size() - 1) std::cout << " -> ";
                            }
                            std::cout << std::endl << "---------------------------------" << std::endl;

                            if (!path.empty())
                            {
                                currentPathIndex = 0;
                                followingPath = true;
                                pathInitialized = true;  // Ensure new path is recognized

                                // DEBUG: Print full path in one line
                                std::cout << "[RexChase] Full Path: ";
                                for (size_t i = 0; i < path.size(); ++i)
                                {
                                    std::cout << "(" << path[i].x << ", " << path[i].y << ", " << path[i].z << ")";
                                    if (i < path.size() - 1) std::cout << " -> ";
                                }
                                std::cout << std::endl;

                                // DEBUG: Print detailed waypoint list
                                for (size_t i = 0; i < path.size(); ++i)
                                {
                                    std::cout << "[RexChase] Waypoint " << i + 1 << ": ("
                                        << path[i].x << ", " << path[i].y << ", " << path[i].z << ")" << std::endl;
                                }
                                std::cout << "---------------------------------\n" << std::endl;
                            }
                            else
                            {
                                followingPath = false;
                                isMovingRex = false;
                                pathInitialized = false;

                                // DEBUG: Path failed
                                std::cout << "[RexChase] ERROR: No valid path! Recalculating...\n" << std::endl;
                                return;
                            }
                        }
                        pathInitialized = true;
                    }

                    if (followingPath && currentPathIndex < path.size())
                    {
                        glm::vec3 targetPos = path[currentPathIndex];

                        // Lock Y-axis movement to keep Rex on the ground
                        targetPos.y = currentPos.y;
                        glm::vec3 direction = glm::normalize(targetPos - currentPos);

                        if (glm::length(direction) > 0.0001f)
                        {
                            velocity = direction * speed;
                        }
                        else
                        {
                            velocity = glm::vec3(0.0f);
                        }

                        float distance = glm::length(targetPos - currentPos);

                        // DEBUG: Print movement towards next waypoint
                        std::cout << "[RexChase] Moving towards waypoint " << currentPathIndex + 1 << " at ("
                            << targetPos.x << ", " << targetPos.y << ", " << targetPos.z << ")" << " | Position: "<< currentPos.x << ", " << currentPos.y << ", " << currentPos.z << std::endl;

                        if (distance <= pathThreshold)
                        {
                            // DEBUG: Reached waypoint
                            std::cout << "[RexChase] Reached waypoint " << currentPathIndex + 1 << std::endl;
                            currentPathIndex++;

                            if (currentPathIndex >= path.size())
                            {
                                followingPath = false;
                                currentPathIndex = 0;
                                velocity = glm::vec3(0.0f);
                                pathInitialized = false;
                                m_Engine.SetBuilt(entity, false);

                                // DEBUG: Path completed
                                std::cout << "[RexChase] Path completed. Searching for a new path...\n" << std::endl;
                                return;
                            }
                        }
                        isMovingRex = true;
                    }
                    break;
                }
            

                
                case ATTACKING:
                {
                    // Directly attack the player
                    glm::vec3 playerPos = m_Engine.GetPosition(m_Engine.GetPlayerEntity());
                    glm::vec3 direction = glm::normalize(playerPos - currentPos);

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
                    isMovingRex = true;

                    if (!sawPlayer) {
                        m_Engine.SetDialogue(8);
                        sawPlayer = true;
                    }
                    break;
                }
                /*
                case SEARCHING:
                {

                    // if player nearest node is the same as rex nearest node, stop and look around
                    velocity = glm::vec3(0.0f);
                    isMovingRex = false;

                    //rotate rex to look around
                    float tempyaw = rexRotation.y;
                    tempyaw += 0.01f;
                    m_Engine.SetRotation(entity, glm::vec3(0.0f, tempyaw, 0.0f));
                    
                    std::cout << "Player is at the same node as Rex" << std::endl;
                    break;
                }*/
                case IDLE:
                {
                    //velocity = glm::vec3(0.0f);
                    isMovingRex = false;
                    sawPlayer = false;
                    break;
                }
            }


            // Apply velocity correctly
            if (isMovingRex)
            {
                if (groundEntity == INVALID_ENT)
                {
                    // No ground detected, continue moving downward
                    velocity.y -= 100.0f * static_cast<float>(m_Engine.GetDeltaTime());  // Simulated gravity effect
                    //std::cout << "[Rex] Falling... Current Y: " << currentPos.y << std::endl;
                }
                else
                {
                    // Ground detected, adjust position to the ground height
                    float raycastFraction = m_Engine.getPhysicsSystem().RaycastFraction(currentPos, downward, maxGroundCheckDistance, entity);
                    glm::vec3 groundPosition = currentPos + downward * raycastFraction * maxGroundCheckDistance;

                    currentPos.y = groundPosition.y + 0.1f;
                    velocity.y = 0.0f;  // Stop falling

                    //std::cout << "[Rex] Landed on ground at Y: " << groundPosition.y << std::endl;
                }

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
                m_Engine.SetVelocity(entity, glm::vec3(0.0f));
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
        float horizontalFOVAngle = 20.0f; // Customize how wide the spread is
        float verticalFOVAngle = 10.0f;   // Customize how far up/down the rays spread
        int horizontalRays = 7; // Number of horizontal rays
        int verticalRays = 7;   // Number of vertical rays
        //glm::vec3 rayOffset = glm::vec3(0.0f, 0.0f, 0.0f);

        // Original eye offset (Local Space)
        glm::vec3 localEyeOffset = glm::vec3(1.2f, 0.8f, 0.0f);

        // **Convert to World Space**
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), yaw, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::vec3 rotatedOffset = glm::vec3(rotationMatrix * glm::vec4(localEyeOffset, 1.0f));

        std::vector<Entity> detectedObjects = m_Engine.getPhysicsSystem().ConeRaycast(
            entity, forwardDirection, maxRayDistance,
            horizontalRays, verticalRays,
            horizontalFOVAngle, verticalFOVAngle,
            //rayOffset
            rotatedOffset
        );

        if (!detectedObjects.empty()) {
            //std::cout << "[Rex] Cone Raycast Detected Entities:\n";
            for (Entity e : detectedObjects) {
                //std::cout << "   - Entity ID: " << e << "\n";
                if (e == playerEntity) {
                    //std::cout << "[Rex] Player detected in FOV.\n";
                    //std::cout << playerEntity << std::endl;
                    return true;
                }
            }
        }
        //std::cout << "[Rex] No objects detected in FOV.\n";
        return false;
    }
};





//HISTORY

//case MOVING:
                //{
                //    /***************************************************************************************************
                //    * 1. Pathfind by setting the start to nearest node and goal to player's nearest node
                //    * 2. Get the path
                //    * 3. Follow the path
                //    * 4. If player is in front, stop and attack
                //    * 5. If player is not in front, continue following the path
                //    * 6. If player is not in front and path is empty, find the path again
                //    * **************************************************************************************************/
                //    // Ensure path is properly initialized after resetting the scene
                //    if (!pathInitialized)
                //    {
                //        if (m_Engine.HavePathfindingComponent(entity))
                //        {
                //            // Set the start and goal nodes for pathfinding
                //            m_Engine.SetStartNode(entity, EntityNearestNode);
                //            PlayerNearestNode = PlayerNodeCheck;
                //            m_Engine.SetGoalNode(entity, PlayerNearestNode);
                //            path = m_Engine.GetPath(entity);

                //            // print start and end node
                //            std::cout << "Start node : " << m_Engine.GetStartNode(entity) << " , " << "End node : " << m_Engine.GetGoalNode(entity) << std::endl;

                //            if (!path.empty())
                //            {
                //                currentPathIndex = 0;
                //                followingPath = true; // <-- This was missing before
                //                std::cout << "[Pathfinding] Path successfully retrieved! Entity " << entity << " will start moving." << std::endl;
                //            }
                //            else
                //            {
                //                followingPath = false;
                //                isMovingRex = false;
                //                pathInitialized = false;
                //                
                //                return;
                //            }
                //        }
                //        else
                //        {
                //            //std::cout << "[Pathfinding] ERROR: No pathfinding component found!" << std::endl;
                //        }

                //        pathInitialized = true; // Set after we have attempted to load the path
                //    }

                //    // Fix: Ensure followingPath is set to true if the path is valid
                //    if (path.size() > 0 && !followingPath)
                //    {
                //        //std::cout << "[Pathfinding] WARNING: Path exists but followingPath is FALSE! Fixing..." << std::endl;
                //        followingPath = true;
                //    }

                //    // Move towards the next waypoint
                //    if (followingPath && currentPathIndex < path.size())
                //    {
                //        glm::vec3 targetPos = path[currentPathIndex];

                //        // Lock Y-axis movement to keep the entity on the ground
                //        targetPos.y = currentPos.y;

                //        glm::vec3 direction = glm::normalize(targetPos - currentPos);

                //        //// Debugging direction calculation
                //        //std::cout << "[Pathfinding] Calculated direction vector: ("
                //        //    << direction.x << ", " << direction.y << ", " << direction.z << ")" << std::endl;

                //        //// *** ADD RAYCAST CHECK HERE ***
                //        //float rayDistance = 2.0f; // Check 2 meters ahead
                //        //Entity hitEntity = m_Engine.getPhysicsSystem().Raycast(currentPos, direction, rayDistance);

                //        //if (hitEntity != invalid_entity)
                //        //{
                //        //    std::cout << "[Rex] Object detected in front! Entity ID: " << hitEntity << std::endl;
                //        //    velocity = glm::vec3(0.0f); // Stop movement if an obstacle is in front
                //        //}

                //        // Ensure no division by zero and check if movement is happening
                //        if (glm::length(direction) > 0.0001f)
                //        {
                //            velocity = direction * speed;
                //        }
                //        else
                //        {
                //            //std::cout << "[Pathfinding] WARNING: Direction vector too small, setting velocity to zero." << std::endl;
                //            velocity = glm::vec3(0.0f);
                //        }

                //        float distance = glm::length(targetPos - currentPos);
                //        //std::cout << "[Pathfinding] Moving towards waypoint " << currentPathIndex + 1 << " at (" << targetPos.x << ", " << targetPos.y << ", " << targetPos.z << ")" << std::endl;
                //        //std::cout << "[Pathfinding] Distance to next waypoint: " << distance << std::endl;

                //        // Check if the entity has reached the waypoint
                //        if (distance <= pathThreshold)
                //        {
                //            //std::cout << "[Pathfinding] Reached waypoint " << currentPathIndex + 1 << std::endl;
                //            currentPathIndex++;

                //            if (currentPathIndex >= path.size())
                //            {

                //                followingPath = false;                // Commented out to allow for continuous path following
                //                currentPathIndex = 0;
                //                velocity = glm::vec3(0.0f);
                //                //std::cout << "[Pathfinding] Entity " << entity << " has reached the final destination!" << std::endl;
                //                pathInitialized = false;
                //                m_Engine.SetBuilt(entity, false);

                //                return;

                //            }
                //        }
                //        isMovingRex = true;
                //    }
                //    else if (!followingPath)
                //    {
                //        //std::cout << "[Pathfinding] No path to follow or already at the destination." << std::endl;
                //        //std::cout << "[Pathfinding] Debugging: pathInitialized = " << pathInitialized
                //        //    << ", followingPath = " << followingPath << ", path size = " << path.size() << std::endl;
                //    }


                //    /****************************************************************************************************
                //    * 1. Check if the player has moved to a new node
                //    * 2. If the player has moved to a new node, reinitialize the path
                //    * 3. If the player has not moved to a new node, continue following the path
                //    ****************************************************************************************************/

                //    break;
                //}