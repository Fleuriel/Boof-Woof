//#include "CollisionSystem.h"
//// #include "../Utilities/Components/CollisionComponent.hpp"                  // Include in .cpp only to avoid circular dependencies
//#include "Jolt/Physics/Body/Body.h"
//#include "Jolt/Physics/Body/BodyInterface.h"
//#include "Jolt/Geometry/AABox.h"

#include "CollisionSystem.h"
#include "../Utilities/Components/CollisionComponent.hpp" // Include to fully define CollisionComponent
#include "Jolt/Physics/Body/Body.h"
#include "Jolt/Physics/Body/BodyInterface.h"
#include "Jolt/Physics/Body/BodyLockInterface.h"
#include "Jolt/Physics/Body/BodyLock.h"
#include "Jolt/Geometry/AABox.h"

void CollisionSystem::InitCollisionSystem(JPH::PhysicsSystem* physicsSystem) {
    m_PhysicsSystem = physicsSystem;

    // Any initialization specific to collision handling
    std::cout << "Initializing Collision System" << std::endl;
}

void CollisionSystem::UpdateLoop(JPH::PhysicsSystem* physicsSystem) {
    // Loop through all entities and check for collisions
    auto allEntities = g_Coordinator.GetAliveEntitiesSet();

    for (auto& entityA : allEntities) {
        if (g_Coordinator.HaveComponent<CollisionComponent>(entityA)) {
            auto& collisionA = g_Coordinator.GetComponent<CollisionComponent>(entityA);

            for (auto& entityB : allEntities) {
                if (entityA == entityB) continue; // Skip self

                if (g_Coordinator.HaveComponent<CollisionComponent>(entityB)) {
                    auto& collisionB = g_Coordinator.GetComponent<CollisionComponent>(entityB);

                    // Check for collision between entityA and entityB
                    if (CheckCollision(collisionA, collisionB)) {
                        // Handle collision response between entityA and entityB
                        HandleCollision(entityA, entityB);
                    }
                }
            }
        }
    }
}

bool CollisionSystem::CheckCollision(const CollisionComponent& collisionA, const CollisionComponent& collisionB) {
    // Retrieve BodyIDs
    JPH::BodyID bodyIDA = collisionA.GetBodyID();
    JPH::BodyID bodyIDB = collisionB.GetBodyID();

    // Get the BodyLockInterface from the physics system (use const reference)
    const JPH::BodyLockInterface& bodyLockInterface = m_PhysicsSystem->GetBodyLockInterface();

    // Lock the bodies for reading
    JPH::BodyLockRead lockA(bodyLockInterface, bodyIDA);
    JPH::BodyLockRead lockB(bodyLockInterface, bodyIDB);

    if (lockA.Succeeded() && lockB.Succeeded()) {
        const JPH::Body& bodyA = lockA.GetBody();
        const JPH::Body& bodyB = lockB.GetBody();

        // Get the AABBs for both bodies
        JPH::AABox aabbA = bodyA.GetWorldSpaceBounds();
        JPH::AABox aabbB = bodyB.GetWorldSpaceBounds();

        // Check if the two AABBs overlap (basic broad-phase collision check)
        return aabbA.Overlaps(aabbB);
    }
    else {
        // If we couldn't lock the bodies, we can't check for collision
        return false;
    }
}



void CollisionSystem::HandleCollision(Entity entityA, Entity entityB) {
    // Implement collision response between two entities
    std::cout << "Collision detected between Entity " << entityA << " and Entity " << entityB << std::endl;

    // You can apply physics responses, destroy entities, or trigger events here
}
