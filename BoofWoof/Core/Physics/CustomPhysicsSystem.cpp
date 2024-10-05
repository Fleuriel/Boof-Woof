//#include "CustomPhysicsSystem.h"
//#include <Jolt/Physics/Body/Body.h>                   // For Body
//#include <Jolt/Physics/Collision/Shape/SphereShape.h> // For SphereShape
//#include <Jolt/Physics/Collision/Shape/ConvexShape.h> // For SphereShape
//#include <Jolt/Physics/Body/BodyInterface.h>          // For BodyInterface
//#include <cstdint>  // For uint32_t or standard integer types
//
//void PhysicsManager::InitializeJolt() {
//    // Initialize JobSystem with multiple threads
//    mJobSystem = new JPH::JobSystemThreadPool();
//
//    // Set up max jobs, barriers, and threads (configure according to your needs)
//    unsigned int maxJobs = 512;
//    unsigned int maxBarriers = 4;
//    int numThreads = 4;
//
//    mJobSystem->Init(maxJobs, maxBarriers, numThreads); // Call Init with proper parameters
//
//    // Create TempAllocator
//    mTempAllocator = new JPH::TempAllocatorImpl(1024 * 1024); // 1 MB allocator
//
//    // Register JoltPhysics types
//    JPH::RegisterTypes();
//
//    // Any initialization specific to collision handling
//    std::cout << "Initializing Physics System" << std::endl;
//}
//
//// Object-vs-broad-phase-layer filter
//class MyObjectVsBroadPhaseLayerFilter : public JPH::ObjectVsBroadPhaseLayerFilter {
//public:
//    virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override {
//        return true; // Allow all collisions between broad phase layers and object layers for now
//    }
//};
//
//// Object-layer-pair filter
//class MyObjectLayerPairFilter : public JPH::ObjectLayerPairFilter {
//public:
//    virtual bool ShouldCollide(JPH::ObjectLayer inObjectLayer1, JPH::ObjectLayer inObjectLayer2) const override {
//        return true; // Allow all object layers to collide for now
//    }
//};
//
//JPH::PhysicsSystem* PhysicsManager::CreatePhysicsSystem() {
//    const unsigned int cMaxBodies = 1024;       // Max number of bodies
//    const unsigned int cNumBodyMutexes = 16;    // Choose a power of 2 in the range [1, 64]
//    const unsigned int cMaxBodyPairs = 1024;    // Max number of body pairs for collision detection
//    const unsigned int cMaxContactConstraints = 1024; // Max number of contact constraints
//
//    // Create the PhysicsSystem instance
//    mPhysicsSystem = new JPH::PhysicsSystem();
//
//    // Initialize filters
//    MyBroadPhaseLayerInterface broadPhaseLayerInterface;
//    MyObjectVsBroadPhaseLayerFilter objectVsBroadPhaseLayerFilter;
//    MyObjectLayerPairFilter objectLayerPairFilter;
//
//    // Initialize the PhysicsSystem with the filters
//    mPhysicsSystem->Init(
//        cMaxBodies,
//        cNumBodyMutexes,
//        cMaxBodyPairs,
//        cMaxContactConstraints,
//        broadPhaseLayerInterface,
//        objectVsBroadPhaseLayerFilter,
//        objectLayerPairFilter
//    );
//
//    // Set gravity (optional)
//    mPhysicsSystem->SetGravity(JPH::Vec3(0.0f, -9.81f, 0.0f));
//
//    return mPhysicsSystem;
//}
//
//void PhysicsManager::AddBody(JPH::PhysicsSystem* physicsSystem, JPH::Vec3 position, float mass) {
//    // Create a dynamic sphere body (for example)
//    JPH::SphereShape* sphereShape = new JPH::SphereShape(1.0f); // 1-meter radius sphere
//
//    // Define body creation settings
//    JPH::BodyCreationSettings bodySettings(
//        sphereShape,                  // Shape
//        position,                     // Position
//        JPH::Quat::sIdentity(),       // Orientation
//        JPH::EMotionType::Dynamic,    // Dynamic motion type
//        1                             // Object layer (dynamic objects)
//    );
//
//    // Override mass if needed
//    bodySettings.mMassPropertiesOverride.mMass = mass;
//
//    // Create the body
//    JPH::Body* body = physicsSystem->GetBodyInterface().CreateBody(bodySettings);
//
//    // Add the body to the system
//    physicsSystem->GetBodyInterface().AddBody(body->GetID(), JPH::EActivation::Activate);
//}
//
//
//void PhysicsManager::SimulatePhysics(JPH::PhysicsSystem* physicsSystem, float deltaTime, JPH::JobSystem* jobSystem) {
//    int collisionSteps = 1; // Adjust this as needed based on your simulation requirements
//
//    physicsSystem->Update(deltaTime, collisionSteps, mTempAllocator, jobSystem);
//}
//
//void PhysicsManager::Cleanup() {
//    delete mPhysicsSystem;
//    delete mJobSystem;
//    delete mTempAllocator;
//}
//
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

#include "CustomPhysicsSystem.h"
#include "IssueReportingCustom.h"
#include <iostream>
#include <cstdlib>
#include <cstdarg>
#include <cstdio>

// Some necessary global definitions for job system and allocator
static constexpr unsigned int cMaxPhysicsJobs = 512;
static constexpr unsigned int cMaxPhysicsBarriers = 4;

// Initialize Jolt Physics
void PhysicsManager::InitializeJolt() {
    // Install the trace and assert callbacks
    JPH::Trace = [](const char* fmt, ...) {
        va_list list;
        va_start(list, fmt);
        vprintf(fmt, list);
        va_end(list);
    };
    JPH::AssertFailed = [](const char* expr, const char* msg, const char* file, unsigned int line) {
        printf("%s(%u): Assert failed: %s - %s\n", file, line, expr, msg ? msg : "");
        return true;
    };
   

    //// Install custom trace and assert handlers from IssueReportingCustom.cpp
    //JPH::Trace = JPH::MyTrace; // Install custom trace function
    //JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = JPH::MyAssertFailed;) // Install custom assert failed function

    // Register default allocator
    JPH::RegisterDefaultAllocator();

    // Register types
    JPH::RegisterTypes();

    // Create job system with a pool of threads
    mJobSystem = new JPH::JobSystemThreadPool(cMaxPhysicsJobs, cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);

    // Create a temp allocator
    mTempAllocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024); // 10 MB temp allocator

    // Create the physics system
    mPhysicsSystem = new JPH::PhysicsSystem();

    // Layer filtering setup
    BPLayerInterfaceImpl layerInterface;
    ObjectVsBroadPhaseLayerFilterImpl broadPhaseLayerFilter;
    ObjectLayerPairFilterImpl objectLayerFilter;

    mPhysicsSystem->Init(1024, 16, 1024, 1024, layerInterface, broadPhaseLayerFilter, objectLayerFilter);
}

// Add a dynamic body (like a sphere)
void PhysicsManager::AddDynamicBody(const JPH::Vec3& position, float mass) {
    // Create a sphere shape
    JPH::SphereShape* sphereShape = new JPH::SphereShape(1.0f); // 1-meter radius sphere

    // Define body creation settings
    JPH::BodyCreationSettings bodySettings(
        sphereShape,                     // Shape
        position,                        // Position
        JPH::Quat::sIdentity(),          // Orientation
        JPH::EMotionType::Dynamic,       // Dynamic body
        1                                // Layer (dynamic objects)
    );

    // Set mass
    bodySettings.mMassPropertiesOverride.mMass = mass;

    // Create body and add to the physics system
    JPH::Body* body = mPhysicsSystem->GetBodyInterface().CreateBody(bodySettings);
    mPhysicsSystem->GetBodyInterface().AddBody(body->GetID(), JPH::EActivation::Activate);
}

// Add a static floor (like a box)
void PhysicsManager::AddStaticFloor() {
    JPH::BoxShapeSettings floorShapeSettings(JPH::Vec3(100.0f, 1.0f, 100.0f)); // Large floor box
    floorShapeSettings.SetEmbedded(); // Mark as embedded for ref counting

    JPH::ShapeSettings::ShapeResult result = floorShapeSettings.Create();
    JPH::ShapeRefC floorShape = result.Get();

    JPH::BodyCreationSettings floorSettings(
        floorShape,
        JPH::RVec3(0.0f, -1.0f, 0.0f), // Position slightly below origin
        JPH::Quat::sIdentity(),
        JPH::EMotionType::Static,      // Static body
        0                              // Layer (static objects)
    );

    JPH::Body* floor = mPhysicsSystem->GetBodyInterface().CreateBody(floorSettings);
    mPhysicsSystem->GetBodyInterface().AddBody(floor->GetID(), JPH::EActivation::DontActivate);
}

// Step the physics system
void PhysicsManager::Simulate(float deltaTime) {
    const int numSteps = 1; // One step per frame (can increase for stability)
    mPhysicsSystem->Update(deltaTime, numSteps, mTempAllocator, mJobSystem);
}

// Cleanup the physics system
void PhysicsManager::Cleanup() {
    delete mPhysicsSystem;
    delete mTempAllocator;
    delete mJobSystem;
}

