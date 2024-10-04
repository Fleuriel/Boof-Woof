//#ifndef CUSTOMPHYSICSSYSTEM_H
//#define CUSTOMPHYSICSSYSTEM_H
//
//// Temporarily undefine the `free` macro
//#ifdef free
//#undef free
//#endif
//
//#include <Jolt/Jolt.h>
//#include <Jolt/Core/JobSystemThreadPool.h>
//#include <Jolt/Physics/PhysicsSystem.h>
//#include <Jolt/Physics/PhysicsSettings.h>
//#include <Jolt/RegisterTypes.h>
//#include <Jolt/Physics/Body/BodyCreationSettings.h>   // For BodyCreationSettings
//#include "../ECS/System.hpp"
//
//// Forward declarations
//namespace JPH {
//    class JobSystemThreadPool;
//    class TempAllocatorImpl;
//    class PhysicsSystem;
//}
//
//// Class to interface with JoltPhysics
//class PhysicsManager : public System {
//public:
//    // Initialize JoltPhysics engine
//    void InitializeJolt();
//
//    // Create and configure the PhysicsSystem
//    JPH::PhysicsSystem* CreatePhysicsSystem();
//
//    // Add a dynamic body to the PhysicsSystem
//    void AddBody(JPH::PhysicsSystem* physicsSystem, JPH::Vec3 position, float mass);
//
//    // Step the physics simulation
//    void SimulatePhysics(JPH::PhysicsSystem* physicsSystem, float deltaTime, JPH::JobSystem* jobSystem);
//
//    // Cleanup resources
//    void Cleanup();
//
//private:
//    JPH::JobSystemThreadPool* mJobSystem = nullptr;
//    JPH::TempAllocatorImpl* mTempAllocator = nullptr;
//    JPH::PhysicsSystem* mPhysicsSystem = nullptr;
//};
//
//// Custom BroadPhaseLayerInterface
//class MyBroadPhaseLayerInterface : public JPH::BroadPhaseLayerInterface {
//public:
//    virtual unsigned int GetNumBroadPhaseLayers() const override;
//    virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer layer) const override;
//};
//
//#endif // CUSTOMPHYSICSSYSTEM_H

#ifndef CUSTOMPHYSICSSYSTEM_H
#define CUSTOMPHYSICSSYSTEM_H

#include <Jolt/Jolt.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/RegisterTypes.h>
//#include <Jolt.h>
//#include <Core/JobSystemThreadPool.h>
//#include <Physics/PhysicsSystem.h>
//#include <Physics/Body/BodyCreationSettings.h>
//#include <Physics/Collision/Shape/BoxShape.h>
//#include <Physics/Collision/Shape/SphereShape.h>
//#include <RegisterTypes.h>
#include "../ECS/System.hpp"

namespace Layers
{
    static constexpr JPH::ObjectLayer NON_MOVING = 0;
    static constexpr JPH::ObjectLayer MOVING = 1;
    static constexpr unsigned int NUM_LAYERS = 2;
}

class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
{
public:
    virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override
    {
        switch (inObject1)
        {
        case Layers::NON_MOVING:
            return inObject2 == Layers::MOVING; // Non-moving only collides with moving
        case Layers::MOVING:
            return true; // Moving collides with everything
        default:
            JPH_ASSERT(false);
            return false;
        }
    }
};

class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
{
public:
    virtual bool ShouldCollide(JPH::ObjectLayer inLayer, JPH::BroadPhaseLayer inBroadPhaseLayer) const override
    {
        switch (inLayer)
        {
        case Layers::NON_MOVING:
            return inBroadPhaseLayer == JPH::BroadPhaseLayer(1); // Non-moving only collides with moving
        case Layers::MOVING:
            return true; // Moving collides with everything
        default:
            JPH_ASSERT(false);
            return false;
        }
    }
};

// Custom broadphase layer interface
class BPLayerInterfaceImpl : public JPH::BroadPhaseLayerInterface
{
public:
    virtual unsigned int GetNumBroadPhaseLayers() const override
    {
        return Layers::NUM_LAYERS; // Static and dynamic
    }

    virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer layer) const override
    {
        return (layer == Layers::NON_MOVING) ? JPH::BroadPhaseLayer(0) : JPH::BroadPhaseLayer(1);
    }
};

// Custom class to manage physics
class PhysicsManager : public System {
public:
    void InitializeJolt(); // Initialize JoltPhysics
    void Simulate(float deltaTime); // Step the physics system
    void Cleanup(); // Cleanup the physics system
    void AddDynamicBody(const JPH::Vec3& position, float mass); // Add a dynamic body
    void AddStaticFloor(); // Add static floor

private:
    JPH::PhysicsSystem* mPhysicsSystem;
    JPH::TempAllocatorImpl* mTempAllocator;
    JPH::JobSystemThreadPool* mJobSystem;
};

#endif // CUSTOMPHYSICSSYSTEM_H

