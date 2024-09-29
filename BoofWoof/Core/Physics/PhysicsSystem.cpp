#include "PhysicsSystem.h"
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/GroupFilterTable.h>


//PhysicsSystem::PhysicsSystem()
//    : m_physicsSystem(nullptr),
//    m_bodyInterface(nullptr),
//    m_tempAllocator(nullptr),
//    m_jobSystem(nullptr) {
//}
//
//PhysicsSystem::~PhysicsSystem() {
//    delete m_physicsSystem;
//    delete m_tempAllocator;
//    delete m_jobSystem;
//}
//
//void PhysicsSystem::Initialize() {
//    // Create the temp allocator for temporary memory allocations
//    m_tempAllocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024);  // Allocate 10 MB
//
//    // Create a job system for multithreading
//    m_jobSystem = new JPH::JobSystemThreadPool(cMaxPhysicsJobs, cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);
//
//    // Initialize the physics system with broad phase and narrow phase layers
//    m_physicsSystem = new JPH::PhysicsSystem(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints);
//
//    m_physicsSystem->Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints,
//        m_broadPhaseLayerInterface, m_objectLayerPairFilter);
//
//    m_bodyInterface = &m_physicsSystem->GetBodyInterface();
//}
//
//void PhysicsSystem::Update(float deltaTime) {
//    // Step the physics system
//    m_physicsSystem->Update(deltaTime, /* gravity */ JPH::Vec3(0, -9.81f, 0), m_tempAllocator, m_jobSystem);
//}
//
//void PhysicsSystem::AddObject(/* parameters for adding a physics object */) {
//    // Example: Create a rigid body and add it to the physics system
//    JPH::BodyCreationSettings settings(JPH::BoxShapeSettings(JPH::Vec3(1.0f, 1.0f, 1.0f)), JPH::Vec3(0, 10, 0),
//        JPH::EMotionType::Dynamic, JPH::ObjectLayer::MOVING);
//    JPH::BodyID bodyID = m_bodyInterface->CreateBody(settings);
//    m_bodyInterface->AddBody(bodyID, JPH::EActivation::Activate);
//}
//
//void PhysicsSystem::RemoveObject(/* parameters for removing a physics object */) {
//    // Example: Remove a body from the physics system
//    JPH::BodyID bodyID = /* body ID to remove */;
//    m_bodyInterface->RemoveBody(bodyID);
//}
//
//// Implementation for ObjectLayerPairFilter
//bool PhysicsSystem::ObjectLayerPairFilterImpl::ShouldCollide(JPH::ObjectLayer object1, JPH::ObjectLayer object2) const {
//    return true;  // Example: Allow all collisions
//}

