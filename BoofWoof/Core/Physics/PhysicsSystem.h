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

#pragma warning(push)
#pragma warning(disable: 4100)
#include <Jolt/Jolt.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
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

// Forward declaration of MyBodyActivationListener
class MyBodyActivationListener;

enum class ObjectType {
    Chair,
    Bed,
    Table,
    Couch,
    Corgi,
    Floor,
    Wall,
    Wardrobe,
    Default
};

namespace Layers
{
    static constexpr JPH::ObjectLayer NON_MOVING{ 0 };
    static constexpr JPH::ObjectLayer MOVING{ 1 };
    static constexpr JPH::ObjectLayer NUM_LAYERS{ 2 };
}; // namespace Layers

/// Class that determines if two object layers can collide
class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
{
public:
    [[nodiscard]] bool ShouldCollide(JPH::ObjectLayer inObject1,
        JPH::ObjectLayer inObject2) const override
    {
        switch (inObject1)
        {
        case Layers::NON_MOVING:
            return inObject2 ==
                Layers::MOVING; // Non moving only collides with moving
        case Layers::MOVING:
            return true; // Moving collides with everything
        default:
            JPH_ASSERT(false);
            return false;
        }
    }
};

//class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
//{
//public:
//    [[nodiscard]] bool ShouldCollide(JPH::ObjectLayer inObject1,
//        JPH::ObjectLayer inObject2) const override
//    {
//        switch (inObject1)
//        {
//        case Layers::NON_MOVING:
//            return inObject2 == Layers::MOVING; // Non-moving only collides with moving
//        case Layers::MOVING:
//            return inObject2 == Layers::MOVING || inObject2 == Layers::NON_MOVING; // Moving collides with everything
//        default:
//            JPH_ASSERT(false);
//            return false;
//        }
//    }
//};

// Each broadphase layer results in a separate bounding volume tree in the broad
// phase. You at least want to have a layer for non-moving and moving objects to
// avoid having to update a tree full of static objects every frame. You can
// have a 1-on-1 mapping between object layers and broadphase layers (like in
// this case) but if you have many object layers you'll be creating many broad
// phase trees, which is not efficient. If you want to fine tune your broadphase
// layers define JPH_TRACK_BROADPHASE_STATS and look at the stats reported on
// the TTY.
namespace BroadPhaseLayers
{
    static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
    static constexpr JPH::BroadPhaseLayer MOVING(1);
    static constexpr JPH::uint NUM_LAYERS(2);
}; // namespace BroadPhaseLayers

// BroadPhaseLayerInterface implementation
// This defines a mapping between object and broadphase layers.
class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface
{
public:
    BPLayerInterfaceImpl()
    {
        // Create a mapping table from object to broad phase layer
        mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
        mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
    }

    [[nodiscard]] JPH::uint GetNumBroadPhaseLayers() const override
    {
        return BroadPhaseLayers::NUM_LAYERS;
    }

    [[nodiscard]] JPH::BroadPhaseLayer GetBroadPhaseLayer(
        JPH::ObjectLayer inLayer) const override
    {
        JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
        return mObjectToBroadPhase[inLayer];
    }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    virtual const char* GetBroadPhaseLayerName(
        JPH::BroadPhaseLayer inLayer) const override
    {
        switch ((JPH::BroadPhaseLayer::Type)inLayer)
        {
        case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:
            return "NON_MOVING";
        case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:
            return "MOVING";
        default:
            JPH_ASSERT(false);
            return "INVALID";
        }
    }
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

private:
    JPH::BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
    //std::array<JPH::BroadPhaseLayer, Layers::NUM_LAYERS> mObjectToBroadPhase;
};

///// Class that determines if an object layer can collide with a broadphase layer
//class ObjectVsBroadPhaseLayerFilterImpl
//    : public JPH::ObjectVsBroadPhaseLayerFilter
//{
//public:
//    [[nodiscard]] bool ShouldCollide(
//        JPH::ObjectLayer inLayer1,
//        JPH::BroadPhaseLayer inLayer2) const override
//    {
//        switch (inLayer1)
//        {
//        case Layers::NON_MOVING:
//            return inLayer2 == BroadPhaseLayers::MOVING;
//        case Layers::MOVING:
//            return true;
//        default:
//            JPH_ASSERT(false);
//            return false;
//        }
//    }
//};

class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
{
public:
    [[nodiscard]] bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override
    {
        switch (inLayer1)
        {
        case Layers::NON_MOVING:
            return inLayer2 == BroadPhaseLayers::MOVING;
        case Layers::MOVING:
            return inLayer2 == BroadPhaseLayers::MOVING || inLayer2 == BroadPhaseLayers::NON_MOVING;
        default:
            JPH_ASSERT(false);
            return false;
        }
    }
};

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

    // Shape creation utility methods
    ObjectType GetObjectTypeFromModel(const std::string& modelName);
    JPH::Shape* CreateShapeForObjectType(ObjectType type, const glm::vec3& scale);

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

    void ClearAllBodies();

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
    JPH::uint _step{ 0 };
    JPH::JobSystemThreadPool* mJobSystem = nullptr;
    JPH::TempAllocatorImpl* mTempAllocator = nullptr;
    JPH::PhysicsSystem* mPhysicsSystem = nullptr;
    
    //// Define broadPhaseLayerInterface here
    //MyBroadPhaseLayerInterface* broadPhaseLayerInterface = nullptr;

    BPLayerInterfaceImpl* m_broad_phase_layer_interface = nullptr;
    ObjectVsBroadPhaseLayerFilterImpl* m_object_vs_broadphase_layer_filter = nullptr;
    ObjectLayerPairFilterImpl* m_object_vs_object_layer_filter = nullptr;

    // Add a pointer to the contact listener
    MyContactListener* mContactListener = nullptr;
    MyBodyActivationListener* mBodyActivationListener = nullptr;

    JPH::BodyID bodyID;

};

// An example contact listener
class MyContactListener : public JPH::ContactListener
{
public:
    // See: ContactListener
    JPH::ValidateResult OnContactValidate(
        const JPH::Body& /* inBody1 */,
        const JPH::Body& /* inBody2 */,
        JPH::RVec3Arg /* inBaseOffset */,
        const JPH::CollideShapeResult& /* inCollisionResult */) override
    {
        std::cout << "Contact validate callback" << std::endl;

        // Allows you to ignore a contact before it is created (using layers to not
        // make objects collide is cheaper!)
        return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
    }

    //void OnContactAdded(const JPH::Body& /* inBody1 */,
    //    const JPH::Body& /* inBody2 */,
    //    const JPH::ContactManifold& /* inManifold */,
    //    JPH::ContactSettings& /* ioSettings */) override
    //{
    //    std::cout << "A contact was added" << std::endl;
    //}

    void OnContactAdded(const JPH::Body& inBody1,
        const JPH::Body& inBody2,
        const JPH::ContactManifold& inManifold,
        JPH::ContactSettings& ioSettings) override
    {
        std::cout << "Collision detected between bodies with IDs: "
            << inBody1.GetID().GetIndex() << " and "
            << inBody2.GetID().GetIndex() << std::endl;
    }

    //void OnContactPersisted(const JPH::Body& /* inBody1 */,
    //    const JPH::Body& /* inBody2 */,
    //    const JPH::ContactManifold& /* inManifold */,
    //    JPH::ContactSettings& /* ioSettings */) override
    //{
    //    std::cout << "A contact was persisted" << std::endl;
    //}

    void OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2,
        const JPH::ContactManifold& inManifold,
        JPH::ContactSettings& ioSettings) override {
        std::cout << "Persisting contact between Body IDs: " << inBody1.GetID().GetIndex()
            << " and " << inBody2.GetID().GetIndex() << std::endl;
        std::cout << "Positions: Body1(" << inBody1.GetPosition().GetX() << ", "
            << inBody1.GetPosition().GetY() << ", " << inBody1.GetPosition().GetZ()
            << ") and Body2(" << inBody2.GetPosition().GetX() << ", "
            << inBody2.GetPosition().GetY() << ", " << inBody2.GetPosition().GetZ()
            << ")" << std::endl;
    }

    void OnContactRemoved(
        const JPH::SubShapeIDPair& /* inSubShapePair */) override
    {
        std::cout << "A contact was removed" << std::endl;
    }
};

// An example activation listener
class MyBodyActivationListener : public JPH::BodyActivationListener
{
public:
    void OnBodyActivated(const JPH::BodyID& /* inBodyID */,
        JPH::uint64 /* inBodyUserData */) override
    {
        std::cout << "A body got activated" << std::endl;
    }

    void OnBodyDeactivated(const JPH::BodyID& /* &inBodyID */,
        JPH::uint64 /* inBodyUserData */) override
    {
        std::cout << "A body went to sleep" << std::endl;
    }
};


#endif // PHYSICSSYSTEM_H

#pragma warning(pop)
