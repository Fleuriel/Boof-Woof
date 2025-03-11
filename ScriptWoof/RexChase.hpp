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
    bool inPlay = false;
    Entity PlayerNearestNode = NULL;
    Entity playerEntity = INVALID_ENT;

    glm::vec3 rexPosition = glm::vec3(0.0f);
    glm::vec3 rexRotation = glm::vec3(0.0f);

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
    }

    virtual void Update(Entity entity) override
    {
        if (m_Engine.GetTRtimer() <= 0.0f)
        {
           chase = true;
           state = TELEPORTING;
        }
        else {
			chase = false;
            return;
        }
        Entity PlayerNodeCheck = m_Engine.GetNearestNode(m_Engine.GetPlayerEntity());

        // If player is in front
        if (CheckifPlayerInFront(entity)) {
            state = ATTACKING;
        }
        // if player nearest node is the same as rex nearest node, stop and look around
        else if (m_Engine.GetNearestNode(entity) == PlayerNodeCheck) {
            state = SEARCHING;
			PlayerNearestNode = PlayerNodeCheck;
		}
        else if (PlayerNearestNode != PlayerNodeCheck) {
            PlayerNearestNode = PlayerNodeCheck;
            pathInitialized = false;
            std::cout << "Player moved to a new node" << std::endl;
			state = MOVING;
        }
        else {
            if (state != MOVING) {
				pathInitialized = false;
                state = MOVING;
            }
        }
       
        glm::vec3 currentPos = m_Engine.GetPosition(entity);
        glm::vec3 velocity(0.0f);

        switch (state) {

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
            inPlay = true;
            state = MOVING;
            break;
        }


        case MOVING:
        {
            /***************************************************************************************************
            * 1. Pathfind by setting the start to nearest node and goal to player's nearest node
            * 2. Get the path
            * 3. Follow the path
            * 4. If player is in front, stop and attack
            * 5. If player is not in front, continue following the path
            * 6. If player is not in front and path is empty, find the path again
            * **************************************************************************************************/
            // Ensure path is properly initialized after resetting the scene
            if (!pathInitialized)
            {
                if (m_Engine.HavePathfindingComponent(entity))
                {
                    // Set the start and goal nodes for pathfinding
                    m_Engine.SetStartNode(entity, m_Engine.GetNearestNode(entity));
                    PlayerNearestNode = PlayerNodeCheck;
                    m_Engine.SetGoalNode(entity, PlayerNearestNode);
                    path = m_Engine.GetPath(entity);

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
                        pathInitialized = false;
                        return;
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
                        return;

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


            /****************************************************************************************************
            * 1. Check if the player has moved to a new node
            * 2. If the player has moved to a new node, reinitialize the path
            * 3. If the player has not moved to a new node, continue following the path
            ****************************************************************************************************/

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
            break;
        }
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
        }
        case IDLE:
        {
            velocity = glm::vec3(0.0f);
            isMovingRex = false;
            break;
        }
        }

			



     //   if (chase)
     //   {
     //       if (!inPlay)
     //       {
     //           //Teleport Rex into the scene
     //           // **Disable physics before teleporting**
     //           m_Engine.DisablePhysics(entity);

     //           // **Teleport the start**
     //           m_Engine.SetPosition(entity, glm::vec3(20.f, 1.5f, 3.f));

     //           // **Update physics transform**
     //           m_Engine.UpdatePhysicsTransform(entity);

     //           // **Re-enable physics**
     //           m_Engine.EnablePhysics(entity);
     //           inPlay = true;
     //       }

     //       glm::vec3 currentPos = m_Engine.GetPosition(entity);
     //       glm::vec3 velocity(0.0f);

     //       

     //       // If player is in front
     //       if (CheckifPlayerInFront(entity)) {
     //           // Directly attack the player
     //           glm::vec3 playerPos = m_Engine.GetPosition(m_Engine.GetPlayerEntity());
     //           glm::vec3 direction = glm::normalize(playerPos - currentPos);

     //           // Ensure no division by zero and check if movement is happening
     //           if (glm::length(direction) > 0.0001f)
     //           {
     //               velocity = direction * speed;
     //           }
     //           else
     //           {
     //               //std::cout << "[Pathfinding] WARNING: Direction vector too small, setting velocity to zero." << std::endl;
     //               velocity = glm::vec3(0.0f);
     //           }
     //           isMovingRex = true;
     //       }
     //       else {

     //           // if player nearest node is the same as rex nearest node, stop and look around
     //           if (m_Engine.GetNearestNode(entity) == m_Engine.GetNearestNode(m_Engine.GetPlayerEntity())) {
     //               velocity = glm::vec3(0.0f);
     //               isMovingRex = false;

     //               //rotate rex to look around
     //               float yaw = rexRotation.y;
     //               yaw += 0.01f;
     //               m_Engine.SetRotation(entity, glm::vec3(0.0f, yaw, 0.0f));
					//m_Engine.SetVelocity(entity, velocity);
     //               std::cout << "Player is at the same node as Rex" << std::endl;
     //               return;
     //           }

     //       /***************************************************************************************************
     //       * 1. Pathfind by setting the start to nearest node and goal to player's nearest node
     //       * 2. Get the path
     //       * 3. Follow the path
     //       * 4. If player is in front, stop and attack
     //       * 5. If player is not in front, continue following the path
     //       * 6. If player is not in front and path is empty, find the path again
     //       * **************************************************************************************************/
     //       // Ensure path is properly initialized after resetting the scene
     //           if (!pathInitialized)
     //           {
     //               if (m_Engine.HavePathfindingComponent(entity))
     //               {
     //                   // Set the start and goal nodes for pathfinding
     //                   m_Engine.SetStartNode(entity, m_Engine.GetNearestNode(entity));
     //                   PlayerNearestNode = m_Engine.GetNearestNode(m_Engine.GetPlayerEntity());
     //                   m_Engine.SetGoalNode(entity, PlayerNearestNode);
     //                   path = m_Engine.GetPath(entity);

     //                   // print start and end node
     //                   std::cout << "Start node : " << m_Engine.GetStartNode(entity) << " , " << "End node : " << m_Engine.GetGoalNode(entity) << std::endl;

     //                   if (!path.empty())
     //                   {
     //                       currentPathIndex = 0;
     //                       followingPath = true; // <-- This was missing before
     //                       std::cout << "[Pathfinding] Path successfully retrieved! Entity " << entity << " will start moving." << std::endl;
     //                   }
     //                   else
     //                   {
     //                       followingPath = false;
     //                       isMovingRex = false;
     //                       pathInitialized = false;
     //                       return;
     //                   }
     //               }
     //               else
     //               {
     //                   //std::cout << "[Pathfinding] ERROR: No pathfinding component found!" << std::endl;
     //               }

     //               pathInitialized = true; // Set after we have attempted to load the path
     //           }

     //           // Fix: Ensure followingPath is set to true if the path is valid
     //           if (path.size() > 0 && !followingPath)
     //           {
     //               //std::cout << "[Pathfinding] WARNING: Path exists but followingPath is FALSE! Fixing..." << std::endl;
     //               followingPath = true;
     //           }

     //           // Move towards the next waypoint
     //           if (followingPath && currentPathIndex < path.size())
     //           {
     //               glm::vec3 targetPos = path[currentPathIndex];

     //               // Lock Y-axis movement to keep the entity on the ground
     //               targetPos.y = currentPos.y;

     //               glm::vec3 direction = glm::normalize(targetPos - currentPos);

     //               //// Debugging direction calculation
     //               //std::cout << "[Pathfinding] Calculated direction vector: ("
     //               //    << direction.x << ", " << direction.y << ", " << direction.z << ")" << std::endl;

     //               //// *** ADD RAYCAST CHECK HERE ***
     //               //float rayDistance = 2.0f; // Check 2 meters ahead
     //               //Entity hitEntity = m_Engine.getPhysicsSystem().Raycast(currentPos, direction, rayDistance);

     //               //if (hitEntity != invalid_entity)
     //               //{
     //               //    std::cout << "[Rex] Object detected in front! Entity ID: " << hitEntity << std::endl;
     //               //    velocity = glm::vec3(0.0f); // Stop movement if an obstacle is in front
     //               //}

     //               // Ensure no division by zero and check if movement is happening
     //               if (glm::length(direction) > 0.0001f)
     //               {
     //                   velocity = direction * speed;
     //               }
     //               else
     //               {
     //                   //std::cout << "[Pathfinding] WARNING: Direction vector too small, setting velocity to zero." << std::endl;
     //                   velocity = glm::vec3(0.0f);
     //               }

     //               float distance = glm::length(targetPos - currentPos);
     //               //std::cout << "[Pathfinding] Moving towards waypoint " << currentPathIndex + 1 << " at (" << targetPos.x << ", " << targetPos.y << ", " << targetPos.z << ")" << std::endl;
     //               //std::cout << "[Pathfinding] Distance to next waypoint: " << distance << std::endl;

     //               // Check if the entity has reached the waypoint
     //               if (distance <= pathThreshold)
     //               {
     //                   //std::cout << "[Pathfinding] Reached waypoint " << currentPathIndex + 1 << std::endl;
     //                   currentPathIndex++;

     //                   if (currentPathIndex >= path.size())
     //                   {

     //                       followingPath = false;                // Commented out to allow for continuous path following
     //                       currentPathIndex = 0;
     //                       velocity = glm::vec3(0.0f);
     //                       //std::cout << "[Pathfinding] Entity " << entity << " has reached the final destination!" << std::endl;
     //                       pathInitialized = false;
     //                       m_Engine.SetBuilt(entity, false);
     //                       return;

     //                   }
     //               }
     //               isMovingRex = true;
     //           }
     //           else if (!followingPath)
     //           {
     //               //std::cout << "[Pathfinding] No path to follow or already at the destination." << std::endl;
     //               //std::cout << "[Pathfinding] Debugging: pathInitialized = " << pathInitialized
     //               //    << ", followingPath = " << followingPath << ", path size = " << path.size() << std::endl;
     //           }


     //           /****************************************************************************************************
     //           * 1. Check if the player has moved to a new node
     //           * 2. If the player has moved to a new node, reinitialize the path
     //           * 3. If the player has not moved to a new node, continue following the path
     //           ****************************************************************************************************/
     //           if (PlayerNearestNode != m_Engine.GetNearestNode(m_Engine.GetPlayerEntity())) {
     //               PlayerNearestNode = m_Engine.GetNearestNode(m_Engine.GetPlayerEntity());
     //               pathInitialized = false;
     //               std::cout << "Player moved to a new node" << std::endl;
     //           }
     //       }

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
				m_Engine.SetVelocity(entity, glm::vec3(0.0f));
            }
        }
    //}

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
                //std::cout << "   - Entity ID: " << e << "\n";
                if (e == playerEntity) {
                    std::cout << "[Rex] Player detected in FOV.\n";
                    std::cout << playerEntity << std::endl;
                    return true;
                }
            }
        }
        //std::cout << "[Rex] No objects detected in FOV.\n";
        return false;
    }
};
