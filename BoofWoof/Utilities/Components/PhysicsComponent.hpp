#pragma once

#include <Jolt/Physics/Collision/Shape/Shape.h>

class PhysicsComponent {
public:
    JPH::Shape* shape; // Pointer to the shape of the physics body
    JPH::Body* body;   // Pointer to the physics body

    // Constructor
    PhysicsComponent(JPH::Shape* shape) : shape(shape), body(nullptr) {}
};
