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



#include "../ECS/System.hpp"

// Forward declarations
namespace JPH {
    class JobSystemThreadPool;
    class TempAllocatorImpl;
    class PhysicsSystem;
}

// Class to interface with JoltPhysics
class PhysicsSystem : public System {
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

#endif // PHYSICSSYSTEM_H
