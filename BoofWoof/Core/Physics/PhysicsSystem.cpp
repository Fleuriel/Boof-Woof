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
#include "pch.h"
#include "PhysicsSystem.h"
#include <Jolt/Physics/Body/Body.h>                   // For Body
#include <Jolt/Physics/Body/BodyCreationSettings.h>   // For BodyCreationSettings
#include <Jolt/Physics/Collision/Shape/SphereShape.h> // For SphereShape
#include <Jolt/Physics/Collision/Shape/BoxShape.h>    // For BoxShape
#include <Jolt/Physics/Collision/Shape/ConvexShape.h> // For SphereShape
#include <Jolt/Physics/Body/BodyInterface.h>          // For BodyInterface
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <../Utilities/Components/CollisionComponent.hpp>

#include <glm/gtc/quaternion.hpp>   // For glm::quat
#include <glm/gtx/euler_angles.hpp> // For Euler angles
#include <cstdlib>
#include <cstdarg>
#include <cstdio>

using namespace JPH::literals;

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

//// Object-vs-broad-phase-layer filter
//class MyObjectVsBroadPhaseLayerFilter : public JPH::ObjectVsBroadPhaseLayerFilter {
//public:
//    virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override
//    {
//        (void)inLayer1, inLayer2;
//        return true; // Allow all collisions between broad phase layers and object layers for now
//    }
//};
//
//// Object-layer-pair filter
//class MyObjectLayerPairFilter : public JPH::ObjectLayerPairFilter {
//public:
//    virtual bool ShouldCollide(JPH::ObjectLayer inObjectLayer1, JPH::ObjectLayer inObjectLayer2) const override
//    {
//        (void)inObjectLayer1, inObjectLayer2;
//        return true; // Allow all object layers to collide for now
//    }
//};
//
//// Custom BroadPhaseLayerInterface implementation
//unsigned int MyBroadPhaseLayerInterface::GetNumBroadPhaseLayers() const {
//    return 2; // Two layers: Static and Dynamic
//}
//
//JPH::BroadPhaseLayer MyBroadPhaseLayerInterface::GetBroadPhaseLayer(JPH::ObjectLayer layer) const {
//    switch (layer) {
//    case 0: return JPH::BroadPhaseLayer(0);  // Static objects
//    case 1: return JPH::BroadPhaseLayer(1);  // Dynamic objects
//    default:
//        std::cerr << "Invalid ObjectLayer passed: " << layer << std::endl;
//        return JPH::BroadPhaseLayer(0);  // Fallback to a default layer
//    }
//}
//
//const char* MyBroadPhaseLayerInterface::GetBroadPhaseLayerName(JPH::BroadPhaseLayer layer) const {
//    // Use the GetValue() method to retrieve the underlying uint8 value for the switch case
//    switch (layer.GetValue()) {
//    case 0: return "Static";
//    case 1: return "Dynamic";
//    default: return "Unknown";
//    }
//}

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
    //mPhysicsSystem = std::make_unique<JPH::PhysicsSystem>();

    //// Initialize filters
    //MyBroadPhaseLayerInterface broadPhaseLayerInterface;
    //MyObjectVsBroadPhaseLayerFilter objectVsBroadPhaseLayerFilter;
    //MyObjectLayerPairFilter objectLayerPairFilter;

    m_broad_phase_layer_interface = new BPLayerInterfaceImpl();
    m_object_vs_broadphase_layer_filter = new ObjectVsBroadPhaseLayerFilterImpl();
    m_object_vs_object_layer_filter = new ObjectLayerPairFilterImpl();

    // Initialize mTempAllocator with a fixed amount of memory (10 MB here)
    mTempAllocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024);

    // Create the JobSystemThreadPool
    mJobSystem = new JPH::JobSystemThreadPool(
        JPH::cMaxPhysicsJobs,
        JPH::cMaxPhysicsBarriers,
        static_cast<int>(std::thread::hardware_concurrency()) - 1
    );

    //// Initialize the PhysicsSystem with the filters
    //mPhysicsSystem->Init(
    //    cMaxBodies,
    //    cNumBodyMutexes,
    //    cMaxBodyPairs,
    //    cMaxContactConstraints,
    //    broadPhaseLayerInterface,
    //    objectVsBroadPhaseLayerFilter,
    //    objectLayerPairFilter
    //);

    // Initialize the PhysicsSystem with the filters and interfaces
    mPhysicsSystem->Init(
        cMaxBodies,
        cNumBodyMutexes,
        cMaxBodyPairs,
        cMaxContactConstraints,
        *m_broad_phase_layer_interface,
        *m_object_vs_broadphase_layer_filter,
        *m_object_vs_object_layer_filter
    );

    // Set gravity
    mPhysicsSystem->SetGravity(JPH::Vec3(0.0f, -9.81f, 0.0f));

    std::cout << "Jolt physics system created" << std::endl;
    return mPhysicsSystem;
}

void MyPhysicsSystem::OnUpdate(float deltaTime) {
    // Iterate through all entities that match the PhysicsSystem signature
    auto allEntities = g_Coordinator.GetAliveEntitiesSet();
    for (auto& entity : allEntities) {
        if (g_Coordinator.HaveComponent<TransformComponent>(entity) && g_Coordinator.HaveComponent<CollisionComponent>(entity)) {
            // std::cout << "Inside Onupdate Physics" << std::endl;
            AddEntityBody(entity);
        }
    }

    // Simulate physics
    mPhysicsSystem->Update(deltaTime, 1, mTempAllocator, mJobSystem);

    // Update the entities' transforms after simulation
    UpdateEntityTransforms();
}

void MyPhysicsSystem::AddEntityBody(Entity entity) {
    if (g_Coordinator.HaveComponent<TransformComponent>(entity)) {
        auto& transform = g_Coordinator.GetComponent<TransformComponent>(entity);

        // Debug output to check Position
        //std::cout << "x: " << transform.GetPosition().x << " y: " << transform.GetPosition().y << " z: " << transform.GetPosition().z << std::endl;

        JPH::Vec3 position(transform.GetPosition().x, transform.GetPosition().y, transform.GetPosition().z);
        glm::vec3 scale = transform.GetScale();

        // Debug output to check the ObjectLayer and MotionType
        std::cout << "Creating a new body with ObjectLayer = 1 (dynamic), MotionType = Dynamic." << std::endl;

        // Create the box shape
        JPH::BoxShape* boxShape = new JPH::BoxShape(JPH::Vec3(scale.x * 0.5f, scale.y * 0.5f, scale.z * 0.5f));

        // Define body creation settings
        JPH::BodyCreationSettings bodySettings(
            boxShape,
            position,
            JPH::Quat::sIdentity(),  // Assuming no initial rotation
            JPH::EMotionType::Dynamic,  // Use dynamic motion type for now
            1  // ObjectLayer, ensure this is valid
        );

        JPH::BodyInterface& bodyInterface = mPhysicsSystem->GetBodyInterface();
        if (&bodyInterface == nullptr) {
            std::cerr << "BodyInterface is not available!" << std::endl;
            return;
        }

        if (mPhysicsSystem == nullptr) {
            std::cerr << "mPhysicsSystem is not initialized!" << std::endl;
            return;
        }

        // Debug output to check Position
        std::cout << "x: " << position.GetX() << " y: " << position.GetY() << " z: " << position.GetZ() << std::endl;

        // Create and add the body to the physics system
        JPH::Body* body = mPhysicsSystem->GetBodyInterface().CreateBody(bodySettings);

        if (body == nullptr) {
            std::cerr << "Failed to create a new body!" << std::endl;
        }
        else {
            std::cout << "Body successfully created with ID: " << body->GetID().GetIndex() << std::endl;
            mPhysicsSystem->GetBodyInterface().AddBody(body->GetID(), JPH::EActivation::Activate);
        }

        //g_Coordinator.AddComponent(entity, CollisionComponent(body, entity, 1));
    }
}


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
            // std::cout << "is inside updateEntityTransform" << std::endl;
            auto& collisionComp = g_Coordinator.GetComponent<CollisionComponent>(entity);
            JPH::Body* body = collisionComp.GetPhysicsBody();

            // Ensure the body ID is valid before accessing it
            if (body != nullptr && !body->GetID().IsInvalid()) {
                // Debug statement to print the updated body position
                std::cout << "Updating transform for entity: " << entity << std::endl;

                // Get the updated position and rotation from the JoltPhysics body
                JPH::Vec3 updatedPosition = body->GetPosition();
                JPH::Quat updatedRotation = body->GetRotation();

                // Update the entity's TransformComponent
                auto& transform = g_Coordinator.GetComponent<TransformComponent>(entity);
                transform.SetPosition(glm::vec3(updatedPosition.GetX(), updatedPosition.GetY(), updatedPosition.GetZ()));

                glm::quat newRotation(updatedRotation.GetW(), updatedRotation.GetX(), updatedRotation.GetY(), updatedRotation.GetZ());
                glm::vec3 eulerRotation = glm::eulerAngles(newRotation);  // Convert quaternion to Euler angles
                transform.SetRotation(eulerRotation);

                // Debug output of the updated position
                std::cout << "Updated entity position: (" << updatedPosition.GetX() << ", "
                    << updatedPosition.GetY() << ", " << updatedPosition.GetZ() << ")"
                    << "\nUpdated entity rotation: (" << eulerRotation.x << ", "
                    << eulerRotation.y << ", " << eulerRotation.z << ")" << std::endl;
            }
        }
    }
}


//void MyPhysicsSystem::UpdateEntityTransforms() {
//    // std::cout << "inside UpdateEntityTransform" << std::endl;
//    auto allEntities = g_Coordinator.GetAliveEntitiesSet();
//    for (auto& entity : allEntities) {
//        if (g_Coordinator.HaveComponent<CollisionComponent>(entity)) {
//            auto& collisionComp = g_Coordinator.GetComponent<CollisionComponent>(entity);
//            JPH::Body* body = collisionComp.GetPhysicsBody();
//
//            // Ensure the body ID is valid before accessing it
//            if (body != nullptr && !body->GetID().IsInvalid()) {
//                std::cout << "updating transform of entity" << std::endl;
//                // Get the updated position and rotation from the JoltPhysics body
//                JPH::Vec3 updatedPosition = body->GetPosition();
//                JPH::Quat updatedRotation = body->GetRotation();
//
//                // Update the entity's TransformComponent
//                auto& transform = g_Coordinator.GetComponent<TransformComponent>(entity);
//                transform.SetPosition(glm::vec3(updatedPosition.GetX(), updatedPosition.GetY(), updatedPosition.GetZ()));
//
//                glm::quat newRotation(updatedRotation.GetW(), updatedRotation.GetX(), updatedRotation.GetY(), updatedRotation.GetZ());
//                glm::vec3 eulerRotation = glm::eulerAngles(newRotation);  // Convert quaternion to Euler angles
//                transform.SetRotation(eulerRotation);
//            }
//        }
//    }
//}


void MyPhysicsSystem::Cleanup() {


    delete mPhysicsSystem;
    delete mJobSystem;
    delete mTempAllocator;
    delete m_broad_phase_layer_interface;
    delete m_object_vs_broadphase_layer_filter;
    delete m_object_vs_object_layer_filter;

    // Clean up the contact listener
    delete mContactListener;

    // Unregisters all types with the factory and cleans up the default material
    JPH::UnregisterTypes();

    // Destroy the factory
    delete JPH::Factory::sInstance;
    JPH::Factory::sInstance = nullptr;

    std::cout << "Physics system cleanup done" << std::endl;
}