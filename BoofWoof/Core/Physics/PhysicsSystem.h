/**************************************************************************
 * @file PhysicsSystem.h
 * @author 	Ang Jun Sheng Aloysius
 * @param DP email: a.junshengaloysius@digipen.edu [2201807]
 * @param Course: CS 3401
 * @param Course: Game Project 3
 * @date  23/09/2024 (23 SEPT 2024)
 * @brief
 *
 * This file contains the declarations to initialize the JoltPhysics System
 *
 *
 *************************************************************************/

#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

#include <Jolt/Jolt.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <../Utilities/Components/TransformComponent.hpp>


#include "../ECS/System.hpp"

// Forward declarations
namespace JPH {
    class JobSystemThreadPool;
    class TempAllocatorImpl;
    class PhysicsSystem;
}

// Forward declaration of MyContactListener
class MyContactListener;

// Class to interface with JoltPhysics
class MyPhysicsSystem : public System {
public:

    /**************************************************************************/
    /*!
    \brief Initializes the JoltPhysics engine.
    */
    /**************************************************************************/
    void InitializeJolt();

    /**************************************************************************/
    /*!
    \brief Creates and configures the PhysicsSystem.

    \return
    A pointer to the created PhysicsSystem.
    */
    /**************************************************************************/
    JPH::PhysicsSystem* CreatePhysicsSystem();

    /**************************************************************************/
    /*!
    \brief Adds a dynamic body to the PhysicsSystem.

    \param physicsSystem
    Pointer to the PhysicsSystem where the body will be added.

    \param position
    The initial position of the body.

    \param mass
    The mass of the body.
    */
    /**************************************************************************/
    void AddBody(JPH::PhysicsSystem* physicsSystem, JPH::Vec3 position, float mass);

    // Modify AddEntityBody to use the CollisionComponent
    void AddEntityBody(Entity entity);  // Add a default layer

    void RemoveEntityBody(Entity entity);

    /**************************************************************************/
    /*!
    \brief Steps the physics simulation.

    \param physicsSystem
    Pointer to the PhysicsSystem to be simulated.

    \param deltaTime
    The time step for the simulation.

    \param jobSystem
    Pointer to the JobSystem used for multithreading.
    */
    /**************************************************************************/
    void SimulatePhysics(JPH::PhysicsSystem* physicsSystem, float deltaTime, JPH::JobSystem* jobSystem);
    void OnUpdate(float deltaTime);

    void UpdateEntityTransforms();  // This function will be called after simulation to update entity transforms

    /**************************************************************************/
    /*!
    \brief Cleans up resources used by the PhysicsManager.
    */
    /**************************************************************************/
    void Cleanup();

private:
    JPH::JobSystemThreadPool* mJobSystem = nullptr;
    JPH::TempAllocatorImpl* mTempAllocator = nullptr;
    JPH::PhysicsSystem* mPhysicsSystem = nullptr;

    //// Add a pointer to the contact listener
    //std::unique_ptr<MyContactListener> mContactListener;

    // Add a pointer to the contact listener
    MyContactListener* mContactListener = nullptr;
};


// Custom BroadPhaseLayerInterface
class MyBroadPhaseLayerInterface : public JPH::BroadPhaseLayerInterface {
public:
    // Gets the number of broad phase layers
    virtual unsigned int GetNumBroadPhaseLayers() const override;

    // Gets the broad phase layer for a given object layer
    virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer layer) const override;

    // Implement the missing method that was causing the error
    virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer layer) const override;
};

//// An example contact listener
//class MyContactListener : public JPH::ContactListener
//{
//public:
//    // See: ContactListener
//    JPH::ValidateResult OnContactValidate(
//        const JPH::Body& /* inBody1 */,
//        const JPH::Body& /* inBody2 */,
//        JPH::RVec3Arg /* inBaseOffset */,
//        const JPH::CollideShapeResult& /* inCollisionResult */) override
//    {
//        std::cout << "Contact validate callback" << std::endl;
//
//        // Allows you to ignore a contact before it is created (using layers to not
//        // make objects collide is cheaper!)
//        return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
//    }
//
//    void OnContactAdded(const JPH::Body& /* inBody1 */,
//        const JPH::Body& /* inBody2 */,
//        const JPH::ContactManifold& /* inManifold */,
//        JPH::ContactSettings& /* ioSettings */) override
//    {
//        std::cout << "A contact was added" << std::endl;
//    }
//
//    void OnContactPersisted(const JPH::Body& /* inBody1 */,
//        const JPH::Body& /* inBody2 */,
//        const JPH::ContactManifold& /* inManifold */,
//        JPH::ContactSettings& /* ioSettings */) override
//    {
//        std::cout << "A contact was persisted" << std::endl;
//    }
//
//    void OnContactRemoved(
//        const JPH::SubShapeIDPair& /* inSubShapePair */) override
//    {
//        std::cout << "A contact was removed" << std::endl;
//    }
//};

class MyContactListener : public JPH::ContactListener {
public:
    // This method gets called whenever a new contact is detected between two bodies
    virtual void OnContactAdded(
        const JPH::Body& inBody1,  // First body in contact
        const JPH::Body& inBody2,  // Second body in contact
        const JPH::ContactManifold& inManifold,  // Contains collision details
        JPH::ContactSettings& ioSettings  // Modify settings if necessary
    ) override {
        // You can access the bodies and do custom collision logic here
        std::cout << "Collision detected between Body1 and Body2!\n";

        // Access entity information from bodies (if you store entity information in bodies)
        // Entity entity1 = static_cast<Entity>(inBody1.GetUserData());
        // Entity entity2 = static_cast<Entity>(inBody2.GetUserData());
    }

    // This method gets called when contact between two bodies is removed
    virtual void OnContactRemoved(
        const JPH::SubShapeIDPair& inSubShapePair
    ) override {
        // Handle contact removal (if needed)
        std::cout << "Collision contact removed!\n";
    }
};

#endif // PHYSICSSYSTEM_H
