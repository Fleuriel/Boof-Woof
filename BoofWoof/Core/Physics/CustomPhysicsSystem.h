#ifndef CUSTOMPHYSICSSYSTEM_H
#define CUSTOMPHYSICSSYSTEM_H

// Temporarily undefine the `free` macro
#ifdef free
#undef free
#endif

#include <Jolt/Jolt.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>   // For BodyCreationSettings
#include "../ECS/System.hpp"

// Forward declarations
namespace JPH {
    class JobSystemThreadPool;
    class TempAllocatorImpl;
    class PhysicsSystem;
}

// Class to interface with JoltPhysics
class PhysicsManager {
public:
    // Initialize JoltPhysics engine
    void InitializeJolt();

    // Create and configure the PhysicsSystem
    JPH::PhysicsSystem* CreatePhysicsSystem();

    // Add a dynamic body to the PhysicsSystem
    void AddBody(JPH::PhysicsSystem* physicsSystem, JPH::Vec3 position, float mass);

    // Step the physics simulation
    void SimulatePhysics(JPH::PhysicsSystem* physicsSystem, float deltaTime, JPH::JobSystem* jobSystem);

    // Cleanup resources
    void Cleanup();

private:
    JPH::JobSystemThreadPool* mJobSystem = nullptr;
    JPH::TempAllocatorImpl* mTempAllocator = nullptr;
    JPH::PhysicsSystem* mPhysicsSystem = nullptr;
};

// Custom BroadPhaseLayerInterface
class MyBroadPhaseLayerInterface : public JPH::BroadPhaseLayerInterface {
public:
    virtual unsigned int GetNumBroadPhaseLayers() const override;
    virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer layer) const override;
};

#endif // CUSTOMPHYSICSSYSTEM_H
