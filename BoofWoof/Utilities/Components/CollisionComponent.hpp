#pragma once
#ifndef COLLISION_COMPONENT_HPP
#define COLLISION_COMPONENT_HPP

// Temporarily undefine the `free` macro
#ifdef free
#undef free
#endif
 
#include "ECS/Coordinator.hpp"
#include "Jolt/Jolt.h"
#include "Jolt/Physics/PhysicsSystem.h"
#include "Jolt/Physics/Body/Body.h"
#include "Jolt/Physics/Body/BodyInterface.h"
#include "Jolt/Physics/Body/BodyCreationSettings.h"
#include "Jolt/Physics/Collision/Shape/BoxShape.h"
#include "TransformComponent.hpp"
#include "GraphicsComponent.hpp"

class CollisionComponent {
public:
    // Default constructor
    CollisionComponent() = default;

    // Constructor to initialize collision shape and body
    CollisionComponent(JPH::PhysicsSystem* physicsSystem, const TransformComponent& transform, const GraphicsComponent& graphics, Entity entity)
        : m_EntityID(entity)
    {
        // Use transform component to set initial position
        const glm::vec3& position = transform.GetPosition();
        const glm::vec3& scale = transform.GetScale();  // Use scale for collision size

        // Create a box shape using JoltPhysics (you can switch to other shapes like SphereShape)
        JPH::BoxShapeSettings boxShapeSettings(JPH::Vec3(scale.x / 2.0f, scale.y / 2.0f, scale.z / 2.0f)); // Half extents
        JPH::ShapeSettings::ShapeResult shapeResult = boxShapeSettings.Create();
        JPH::Ref<JPH::Shape> boxShape = shapeResult.Get();

        // Set up body creation settings
        JPH::BodyCreationSettings bodySettings(
            boxShape,                                        // Shape
            JPH::Vec3(position.x, position.y, position.z),   // Position from TransformComponent
            JPH::Quat::sIdentity(),                          // Orientation, could use rotation if needed
            JPH::EMotionType::Static,                        // Set to dynamic/kinematic/static based on object type
            1                                                // Object layer (change based on your layer settings)
        );

        // Create the body in the physics system
        JPH::BodyInterface& bodyInterface = physicsSystem->GetBodyInterface();
        JPH::Body* body = bodyInterface.CreateBody(bodySettings);

        // Add the body to the physics system
        bodyInterface.AddBody(body->GetID(), JPH::EActivation::Activate);

        // Store the BodyID instead of the Body*
        m_BodyID = body->GetID();
    }

    // Copy constructor
    CollisionComponent(const CollisionComponent& other)
        : m_EntityID(other.m_EntityID), m_BodyID(other.m_BodyID)
    {
        // Note: The physics body is shared between components with the same BodyID.
        // If you need a separate body for the new component, you will need to create a new body.
    }

    // Copy assignment operator
    CollisionComponent& operator=(const CollisionComponent& other)
    {
        if (this != &other)
        {
            m_EntityID = other.m_EntityID;
            m_BodyID = other.m_BodyID;
            // Again, the physics body is shared via BodyID.
        }
        return *this;
    }

    // Destructor to clean up
    ~CollisionComponent()
    {
        // Cleanup logic if needed (e.g., remove body from physics system)
        // This depends on how you manage bodies in your physics system.
    }

    // Add this method to set the entity ID
    void SetComponentEntityID(Entity entity) {
        m_EntityID = entity;
    }

    // Update the body position in the physics system based on transform component changes
    void UpdateTransform(const TransformComponent& transform, JPH::PhysicsSystem* physicsSystem)
    {
        const glm::vec3& position = transform.GetPosition();
        JPH::Vec3 joltPosition(position.x, position.y, position.z);

        // Update the body position using the BodyInterface
        physicsSystem->GetBodyInterface().SetPosition(m_BodyID, joltPosition, JPH::EActivation::Activate);
    }

    // Getter for the physics body ID
    JPH::BodyID GetBodyID() const { return m_BodyID; }

private:
    Entity m_EntityID{};       // Entity ID associated with the collision component
    JPH::BodyID m_BodyID;      // Body ID instead of Body* for copyability
};

#endif // COLLISION_COMPONENT_HPP
