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

void PhysicsSystem::InitializeJolt() {
    //// Initialize JobSystem with multiple threads
    //mJobSystem = new JPH::JobSystemThreadPool();

    //// Set up max jobs, barriers, and threads (configure according to your needs)
    //unsigned int maxJobs = 512;
    //unsigned int maxBarriers = 4;
    //int numThreads = 4;

    //mJobSystem->Init(maxJobs, maxBarriers, numThreads); // Call Init with proper parameters

    //// Create TempAllocator
    //mTempAllocator = new JPH::TempAllocatorImpl(1024 * 1024); // 1 MB allocator

    //// Register JoltPhysics types
    //JPH::RegisterTypes();

    // Register default allocator
    JPH::RegisterDefaultAllocator();

    // Install trace and assert callbacks
    JPH::Trace = MyTrace;
    JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = MyAssertFailed;)

    // Create a factory, this class is responsible for creating instances of classes based on their name or hash.
    JPH::Factory::sInstance = new JPH::Factory();

    // Register physics types
    JPH::RegisterTypes();

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

JPH::PhysicsSystem* PhysicsSystem::CreatePhysicsSystem() {
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

    return mPhysicsSystem;
}

void PhysicsSystem::AddBody(JPH::PhysicsSystem* physicsSystem, JPH::Vec3 position, float mass) {
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


void PhysicsSystem::SimulatePhysics(JPH::PhysicsSystem* physicsSystem, float deltaTime, JPH::JobSystem* jobSystem) {
    int collisionSteps = 1; // Adjust this as needed based on your simulation requirements

    physicsSystem->Update(deltaTime, collisionSteps, mTempAllocator, jobSystem);
}

void PhysicsSystem::Cleanup() {
    delete mPhysicsSystem;
    delete mJobSystem;
    delete mTempAllocator;

    // Unregisters all types with the factory and cleans up the default material
    JPH::UnregisterTypes();

    // Destroy the factory
    delete JPH::Factory::sInstance;
    JPH::Factory::sInstance = nullptr;
}

//unsigned int MyBroadPhaseLayerInterface::GetNumBroadPhaseLayers() const {
//    return 2; // Two layers: Static and Dynamic
//}
//
//JPH::BroadPhaseLayer MyBroadPhaseLayerInterface::GetBroadPhaseLayer(JPH::ObjectLayer layer) const {
//    switch (layer) {
//    case 0: return JPH::BroadPhaseLayer(0); // Static objects
//    case 1: return JPH::BroadPhaseLayer(1); // Dynamic objects
//    default: return JPH::BroadPhaseLayer(0);
//    }
//}

