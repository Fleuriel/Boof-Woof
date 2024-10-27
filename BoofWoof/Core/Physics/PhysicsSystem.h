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
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayerInterfaceMask.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
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

    //// Define broadPhaseLayerInterface here
    //MyBroadPhaseLayerInterface* broadPhaseLayerInterface = nullptr;

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

//class MyBroadPhaseLayerInterface : public JPH::BroadPhaseLayerInterface {
//public:
//    // Gets the number of broad phase layers
//    virtual unsigned int GetNumBroadPhaseLayers() const override {
//        return 2;  // For example, 2 broad phase layers (Static and Dynamic)
//    }
//
//    // Gets the broad phase layer for a given object layer
//    virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer layer) const override {
//        switch (layer) {
//        case 0: return JPH::BroadPhaseLayer(0);  // Static objects
//        case 1: return JPH::BroadPhaseLayer(1);  // Dynamic objects
//        default: return JPH::BroadPhaseLayer(0);
//        }
//    }
//
//    virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer layer) const override {
//        switch (layer.GetValue()) {
//        case 0: return "Static";
//        case 1: return "Dynamic";
//        default: return "Unknown";
//        }
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
        // Log the IDs of the colliding bodies
        std::cout << "Collision detected between Body1 (ID: " << inBody1.GetID().GetIndex()
            << ") and Body2 (ID: " << inBody2.GetID().GetIndex() << ")" << std::endl;

        // Access entity information if stored in bodies
        Entity entity1 = static_cast<Entity>(inBody1.GetUserData());
        Entity entity2 = static_cast<Entity>(inBody2.GetUserData());

        std::cout << "Collision detected between entities: " << entity1 << " and " << entity2 << std::endl;

        // Optionally, implement game-specific logic here based on the collision
    }

    // This method gets called when contact between two bodies is removed
    virtual void OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair) override {
        std::cout << "Collision contact removed!" << std::endl;
    }
};


#endif // PHYSICSSYSTEM_H
