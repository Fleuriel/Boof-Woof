#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayerInterfaceTable.h>
#include <Jolt/Physics/Collision/NarrowPhaseQuery.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/PhysicsMaterial.h>
#include <Jolt/Physics/Constraints/Constraint.h>


class PhysicsSystem {
public:
    PhysicsSystem();
    ~PhysicsSystem();

    void Initialize();
    void Update(float deltaTime);
    void AddObject(/* parameters for adding a physics object */);
    void RemoveObject(/* parameters for removing a physics object */);

private:
    JPH::PhysicsSystem* m_physicsSystem;    // Pointer to the Jolt physics system
    JPH::BodyInterface* m_bodyInterface;    // Interface to manage bodies
    JPH::TempAllocator* m_tempAllocator;    // Temporary allocator for physics system
    JPH::JobSystem* m_jobSystem;            // Job system for multithreading

    // Replace BroadPhaseLayerInterface with the correct interface implementation
    JPH::BroadPhaseLayerInterfaceTable m_broadPhaseLayerInterface;  // Or BroadPhaseLayerInterfaceMask

    // Layers for collision filtering
    class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter {
    public:
        virtual bool ShouldCollide(JPH::ObjectLayer object1, JPH::ObjectLayer object2) const override;
    };

    ObjectLayerPairFilterImpl m_objectLayerPairFilter;
};

#endif  // PHYSICS_SYSTEM_H
