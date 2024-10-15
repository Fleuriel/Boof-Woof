#pragma once

#ifndef COLLISION_SYSTEM_H
#define COLLISION_SYSTEM_H

#include "../ECS/System.hpp"
#include "../Utilities/Components/CollisionComponent.hpp"
#include "../Utilities/Components/TransformComponent.hpp"  // For transforming AABBs if needed

class CollisionSystem : public System
{
public:
    void Init();
    void Update(float deltaTime);
    void SetDebugDrawEnabled(bool enabled) { debugDrawEnabled = enabled; }

private:
    void CheckCollisions();
    bool debugDrawEnabled = false;  // Flag to enable/disable AABB debug rendering
};

#endif  // COLLISION_SYSTEM_H
