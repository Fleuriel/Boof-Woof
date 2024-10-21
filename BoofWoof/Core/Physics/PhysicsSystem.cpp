/**************************************************************************
 * @file PhysicsSystem.cpp
 * @author 	Ang Jun Sheng Aloysius
 * @param DP email: a.junshengaloysius@digipen.edu [2201807]
 * @param Course: CS 3401
 * @param Course: Game Project 3
 * @date  23/09/2024 (23 SEPT 2024)
 * @brief
 *
 * This file contains the definitions to initialize the JoltPhysics System
 *
 *
 *************************************************************************/

#include "PhysicsSystem.h"
#include <Jolt/Physics/Body/Body.h>                   // For Body
#include <Jolt/Physics/Body/BodyCreationSettings.h>   // For BodyCreationSettings
#include <Jolt/Physics/Collision/Shape/SphereShape.h> // For SphereShape
#include <Jolt/Physics/Collision/Shape/ConvexShape.h> // For SphereShape
#include <Jolt/Physics/Body/BodyInterface.h>          // For BodyInterface
#include <../Utilities/Components/CollisionComponent.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>   // For glm::quat
#include <glm/gtx/euler_angles.hpp> // For Euler angles
#include <cstdint>  // For uint32_t or standard integer types
#include <cstdlib>
#include <cstdarg>
#include <cstdio>

static void MyTrace(const char* inFMT, ...)
{
    va_list args;
    va_start(args, inFMT);
    vprintf(inFMT, args);
    va_end(args);
    printf("\n");
}

static bool MyAssertFailed(const char* inExpression, const char* inMessage, const char* inFile, unsigned int inLine)
{
    printf("Assert failed: %s, Message: %s, File: %s, Line: %d\n", inExpression, inMessage ? inMessage : "", inFile, inLine);
    return true; // Trigger a breakpoint if running under a debugger.
}

// Object-vs-broad-phase-layer filter
class MyObjectVsBroadPhaseLayerFilter : public JPH::ObjectVsBroadPhaseLayerFilter {
public:
    virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override
    {
        (void)inLayer1, inLayer2;
        return true; // Allow all collisions between broad phase layers and object layers for now
    }
};

// Object-layer-pair filter
class MyObjectLayerPairFilter : public JPH::ObjectLayerPairFilter {
public:
    virtual bool ShouldCollide(JPH::ObjectLayer inObjectLayer1, JPH::ObjectLayer inObjectLayer2) const override
    {
        (void)inObjectLayer1, inObjectLayer2;
        return true; // Allow all object layers to collide for now
    }
};

// Custom BroadPhaseLayerInterface implementation
unsigned int MyBroadPhaseLayerInterface::GetNumBroadPhaseLayers() const {
    return 2; // Two layers: Static and Dynamic
}

JPH::BroadPhaseLayer MyBroadPhaseLayerInterface::GetBroadPhaseLayer(JPH::ObjectLayer layer) const {
    switch (layer) {
    case 0: return JPH::BroadPhaseLayer(0); // Static objects
    case 1: return JPH::BroadPhaseLayer(1); // Dynamic objects
    default: return JPH::BroadPhaseLayer(0);
    }
}

const char* MyBroadPhaseLayerInterface::GetBroadPhaseLayerName(JPH::BroadPhaseLayer layer) const {
    // Use the GetValue() method to retrieve the underlying uint8 value for the switch case
    switch (layer.GetValue()) {
    case 0: return "Static";
    case 1: return "Dynamic";
    default: return "Unknown";
    }
}

void MyPhysicsSystem::InitializeJolt() {

    // Register default allocator
    JPH::RegisterDefaultAllocator();

    // Install trace and assert callbacks
    JPH::Trace = MyTrace;
    JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = MyAssertFailed;)

    // Create a factory, this class is responsible for creating instances of classes based on their name or hash.
    JPH::Factory::sInstance = new JPH::Factory();

    // Register physics types
    JPH::RegisterTypes();

    // Initialize the PhysicsSystem (this creates the main physics world for Jolt)
    CreatePhysicsSystem();

    // Register the contact listener to handle collision events
    mContactListener = new MyContactListener();  // Instantiate the listener
    mPhysicsSystem->SetContactListener(mContactListener);  // Register the listener

}

JPH::PhysicsSystem* MyPhysicsSystem::CreatePhysicsSystem() {
    const unsigned int cMaxBodies = 1024;       // Max number of bodies
    const unsigned int cNumBodyMutexes = 16;    // Choose a power of 2 in the range [1, 64]
    const unsigned int cMaxBodyPairs = 1024;    // Max number of body pairs for collision detection
    const unsigned int cMaxContactConstraints = 1024; // Max number of contact constraints

    // Create the PhysicsSystem instance
    mPhysicsSystem = new JPH::PhysicsSystem();

    // Initialize filters
    MyBroadPhaseLayerInterface broadPhaseLayerInterface;
    MyObjectVsBroadPhaseLayerFilter objectVsBroadPhaseLayerFilter;
    MyObjectLayerPairFilter objectLayerPairFilter;

    // Initialize the PhysicsSystem with the filters
    mPhysicsSystem->Init(
        cMaxBodies,
        cNumBodyMutexes,
        cMaxBodyPairs,
        cMaxContactConstraints,
        broadPhaseLayerInterface,
        objectVsBroadPhaseLayerFilter,
        objectLayerPairFilter
    );

    // Set gravity (optional)
    mPhysicsSystem->SetGravity(JPH::Vec3(0.0f, -9.81f, 0.0f));

    std::cout << "Jolt physics system created" << std::endl;
    return mPhysicsSystem;
}

void MyPhysicsSystem::AddBody(JPH::PhysicsSystem* physicsSystem, JPH::Vec3 position, float mass) {
    // Create a dynamic sphere body (for example)
    JPH::SphereShape* sphereShape = new JPH::SphereShape(1.0f); // 1-meter radius sphere

    // Define body creation settings
    JPH::BodyCreationSettings bodySettings(
        sphereShape,                  // Shape
        position,                     // Position
        JPH::Quat::sIdentity(),       // Orientation
        JPH::EMotionType::Dynamic,    // Dynamic motion type
        1                             // Object layer (dynamic objects)
    );

    // Override mass if needed
    bodySettings.mMassPropertiesOverride.mMass = mass;

    // Create the body
    JPH::Body* body = physicsSystem->GetBodyInterface().CreateBody(bodySettings);

    // Add the body to the system
    physicsSystem->GetBodyInterface().AddBody(body->GetID(), JPH::EActivation::Activate);
}


//void PhysicsSystem::SimulatePhysics(JPH::PhysicsSystem* physicsSystem, float deltaTime, JPH::JobSystem* jobSystem) {
//    //int collisionSteps = 1; // Adjust this as needed based on your simulation requirements
//
//    //physicsSystem->Update(deltaTime, collisionSteps, mTempAllocator, jobSystem);
//    physicsSystem->Update(deltaTime, 1, mTempAllocator, jobSystem);  // Run physics update
//    UpdateEntityTransforms();  // After each simulation step, update entity transforms
//}

void MyPhysicsSystem::OnUpdate(float deltaTime) {
    // Iterate through all entities that match the PhysicsSystem signature
    auto allEntities = g_Coordinator.GetAliveEntitiesSet();
    for (auto& entity : allEntities) {
        if (g_Coordinator.HaveComponent<TransformComponent>(entity)) {
            std::cout << "Inside Onupdate Physics" << std::endl;
            // Check if the entity already has a CollisionComponent
            if (!g_Coordinator.HaveComponent<CollisionComponent>(entity)) {
                // If it doesn't have a CollisionComponent, add a physics body
                AddEntityBody(entity);
            }
        }
    }

    // Simulate physics
    mPhysicsSystem->Update(deltaTime, 1, mTempAllocator, mJobSystem);

    // Update the entities' transforms after simulation
    //UpdateEntityTransforms();
}


void MyPhysicsSystem::Cleanup() {

    //// Iterate over all entities with a CollisionComponent and remove their bodies
    //auto allEntities = g_Coordinator.GetAliveEntitiesSet();
    //for (auto& entity : allEntities) {
    //    if (g_Coordinator.HaveComponent<CollisionComponent>(entity)) {
    //        RemoveEntityBody(entity);  // Remove the entity's body from physics system
    //    }
    //}

    delete mPhysicsSystem;
    delete mJobSystem;
    delete mTempAllocator;

    // Clean up the contact listener
    delete mContactListener;

    // Unregisters all types with the factory and cleans up the default material
    JPH::UnregisterTypes();

    // Destroy the factory
    delete JPH::Factory::sInstance;
    JPH::Factory::sInstance = nullptr;

    std::cout << "Physics system cleanup done" << std::endl;
}

void MyPhysicsSystem::AddEntityBody(Entity entity) {
    // Ensure the entity has a TransformComponent before creating a body
    if (g_Coordinator.HaveComponent<TransformComponent>(entity)) {
        auto& transform = g_Coordinator.GetComponent<TransformComponent>(entity);

        // Extract position, rotation, and scale from TransformComponent
        JPH::Vec3 position(transform.GetPosition().x, transform.GetPosition().y, transform.GetPosition().z);
        glm::vec3 rotation = transform.GetRotation();
        JPH::Vec3 scale(transform.GetScale().x, transform.GetScale().y, transform.GetScale().z);

        // Create a basic shape (adjust shape creation based on your game's needs)
        JPH::SphereShape* shape = new JPH::SphereShape(1.0f * transform.GetScale().x);  // Example with sphere shape

        // Define the body creation settings
        JPH::BodyCreationSettings bodySettings(
            shape,
            position,
            JPH::Quat::sEulerAngles(JPH::Vec3(rotation.x, rotation.y, rotation.z)),  // Quaternion for rotation
            JPH::EMotionType::Dynamic,  // Dynamic body
            1  // Object layer (dynamic objects)
        );

        // Create the body and add it to the JoltPhysics system
        JPH::Body* body = mPhysicsSystem->GetBodyInterface().CreateBody(bodySettings);
        mPhysicsSystem->GetBodyInterface().AddBody(body->GetID(), JPH::EActivation::Activate);

        // Add the CollisionComponent to track the body
        g_Coordinator.AddComponent(entity, CollisionComponent(body, entity, 1));  // 1 = dynamic layer
    }
}

//void MyPhysicsSystem::RemoveEntityBody(Entity entity) {
//    if (g_Coordinator.HaveComponent<CollisionComponent>(entity)) {
//        auto& collisionComp = g_Coordinator.GetComponent<CollisionComponent>(entity);
//
//        // Remove the body from the physics system
//        mPhysicsSystem->GetBodyInterface().RemoveBody(collisionComp.GetPhysicsBody()->GetID());
//
//        // Remove the component from the entity
//        g_Coordinator.RemoveComponent<CollisionComponent>(entity);
//    }
//}

void MyPhysicsSystem::RemoveEntityBody(Entity entity) {
    if (g_Coordinator.HaveComponent<CollisionComponent>(entity)) {
        auto& collisionComp = g_Coordinator.GetComponent<CollisionComponent>(entity);

        // Ensure the body exists and is valid
        JPH::BodyID bodyID = collisionComp.GetPhysicsBody()->GetID();
        if (!bodyID.IsInvalid()) {
            // Remove the body from the physics system
            mPhysicsSystem->GetBodyInterface().RemoveBody(bodyID);
        }

        // Optionally, delete the body if necessary
        // delete collisionComp.GetPhysicsBody();

        // Remove the CollisionComponent from the entity
        g_Coordinator.RemoveComponent<CollisionComponent>(entity);
    }
}

void MyPhysicsSystem::UpdateEntityTransforms() {
    auto allEntities = g_Coordinator.GetAliveEntitiesSet();
    for (auto& entity : allEntities) {
        if (g_Coordinator.HaveComponent<CollisionComponent>(entity)) {
            auto& collisionComp = g_Coordinator.GetComponent<CollisionComponent>(entity);
            JPH::Body* body = collisionComp.GetPhysicsBody();

            // Ensure the body ID is valid before accessing it
            if (body != nullptr && !body->GetID().IsInvalid()) {
                // Get the updated position and rotation from the JoltPhysics body
                JPH::Vec3 updatedPosition = body->GetPosition();
                JPH::Quat updatedRotation = body->GetRotation();

                // Update the entity's TransformComponent
                auto& transform = g_Coordinator.GetComponent<TransformComponent>(entity);
                transform.SetPosition(glm::vec3(updatedPosition.GetX(), updatedPosition.GetY(), updatedPosition.GetZ()));

                glm::quat newRotation(updatedRotation.GetW(), updatedRotation.GetX(), updatedRotation.GetY(), updatedRotation.GetZ());
                glm::vec3 eulerRotation = glm::eulerAngles(newRotation);  // Convert quaternion to Euler angles
                transform.SetRotation(eulerRotation);
            }
        }
    }
}

//void MyPhysicsSystem::UpdateEntityTransforms() {
//    auto allEntities = g_Coordinator.GetAliveEntitiesSet();
//    for (auto& entity : allEntities) {
//        std::cout << "physics system running" << std::endl;
//        if (g_Coordinator.HaveComponent<CollisionComponent>(entity)) {
//            auto& collisionComp = g_Coordinator.GetComponent<CollisionComponent>(entity);
//            auto& transform = g_Coordinator.GetComponent<TransformComponent>(entity);
//
//            // Get the updated position and rotation from the JoltPhysics body
//            JPH::Body* body = collisionComp.GetPhysicsBody();
//            JPH::Vec3 updatedPosition = body->GetPosition();
//            JPH::Quat updatedRotation = body->GetRotation();
//
//            // Update the TransformComponent position
//            transform.SetPosition(glm::vec3(updatedPosition.GetX(), updatedPosition.GetY(), updatedPosition.GetZ()));
//
//            // Update the TransformComponent rotation (convert quaternion to Euler angles)
//            glm::quat newRotation(updatedRotation.GetW(), updatedRotation.GetX(), updatedRotation.GetY(), updatedRotation.GetZ());
//            glm::vec3 eulerRotation = glm::eulerAngles(newRotation);  // Get Euler angles from quaternion
//            transform.SetRotation(eulerRotation);  // Set the Euler angles in TransformComponent
//        }
//    }
//}

