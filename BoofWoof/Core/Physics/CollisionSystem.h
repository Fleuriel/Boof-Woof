#pragma once
#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

// Temporarily undefine the `free` macro
#ifdef free
#undef free
#endif

// Include JoltPhysics headers directly
#include "Jolt/Jolt.h"
#include "Jolt/Physics/PhysicsSystem.h"

// Other includes
#include "ECS/Coordinator.hpp"
// Remove or comment out the CustomPhysicsSystem.h include
// #include "CustomPhysicsSystem.h"

// Forward declarations to avoid circular dependencies
class CollisionComponent;
class TransformComponent;

class CollisionSystem {
public:
    // Initialize the collision system and any necessary resources
    void InitCollisionSystem(JPH::PhysicsSystem* physicsSystem);

    // Update loop to process collision detection for all entities with a CollisionComponent
    void UpdateLoop(JPH::PhysicsSystem* physicsSystem);

private:
    // Process a single collision between two entities
    void HandleCollision(Entity entityA, Entity entityB);

    // Check for collision between two entities
    bool CheckCollision(const CollisionComponent& collisionA, const CollisionComponent& collisionB);

    JPH::PhysicsSystem* m_PhysicsSystem; // Pointer to the physics system
};

#endif // COLLISION_SYSTEM_HPP

