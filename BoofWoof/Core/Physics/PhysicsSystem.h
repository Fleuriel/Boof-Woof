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
#pragma warning(disable: 4100 4189 4081 4458)
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
#include <../Utilities/Components/MetaData.hpp>

#include "../ECS/System.hpp"

using Entity = std::uint32_t; // Assuming Entity is defined as uint32_t
static constexpr Entity invalid_entity = static_cast<Entity>(-1);

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
    Table2,
    Couch,
    Corgi,
    Corgi_small,
    FloorCastle,
    Wall,
    Wardrobe,
    Wardrobe2,
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
    //JPH::Shape* CreateShapeForObjectType(ObjectType type, const glm::vec3& scale);
    //JPH::Shape* CreateShapeForObjectType(ObjectType type, const glm::vec3& scale, const glm::vec3& userAABBSize = glm::vec3(1.0f));
    JPH::Shape* CreateShapeForObjectType(ObjectType type, const glm::vec3& customAABB);
    // UpdateEntityBody(Entity entity);
    void UpdateEntityBody(Entity entity, float mass);

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
    //void AddEntityBody(Entity entity);  // Add a default layer
    void AddEntityBody(Entity entity, float mass);  // Add a default layer

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

    Entity GetEntityFromBody(const JPH::BodyID bodyID);

    /**
     * @brief Casts a ray from a given position in a direction and checks if it hits an entity.
     *
     * @param origin The starting position of the ray.
     * @param direction The normalized direction of the ray.
     * @param maxDistance The maximum distance the ray can travel.
     * @param ignoreEntity The entity to ignore for the raycast
     * @return The Entity that was hit, or invalid_entity if no entity was hit.
     */
    Entity Raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance, Entity ignoreEntity);
    std::vector<Entity> ConeRaycast(
        Entity entity,
        const glm::vec3& forwardDirection,
        float maxDistance,
        int numHorizontalRays, int numVerticalRays,
        float horizontalFOVAngle, float verticalFOVAngle,  // User-defined angles
        const glm::vec3& userOffset);
    std::vector<Entity> ConeRaycastDownward(
        Entity entity,
        const glm::vec3& direction, float maxDistance,
        int numHorizontalRays, int numVerticalRays, float coneAngle,
        const glm::vec3& userOffset);

    static bool RayCastDebug;
    float RaycastFraction(const glm::vec3& origin, const glm::vec3& direction, float maxDistance, Entity ignoreEntity);
    void DisablePhysics(Entity entity);
    void EnablePhysics(Entity entity);
    void UpdatePhysicsTransform(Entity entity);
    void SetEntityGravityFactor(Entity entity, float gravityFactor);

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

    std::unordered_map<JPH::BodyID, Entity> bodyToEntityMap;

};

// An example contact listener
class MyContactListener : public JPH::ContactListener
{
public:

    // Default constructor
    MyContactListener() = default;

    // Constructor with physics system
    MyContactListener(JPH::PhysicsSystem* physicsSystem)
        : mPhysicsSystem(physicsSystem) {}

    // Setter for physics system
    void SetPhysicsSystem(JPH::PhysicsSystem* physicsSystem) {
        mPhysicsSystem = physicsSystem;
    }

    // See: ContactListener
    JPH::ValidateResult OnContactValidate(
        const JPH::Body& /* inBody1 */,
        const JPH::Body& /* inBody2 */,
        JPH::RVec3Arg /* inBaseOffset */,
        const JPH::CollideShapeResult& /* inCollisionResult */) override
    {
        // std::cout << "Contact validate callback" << std::endl;

        // Allows you to ignore a contact before it is created (using layers to not
        // make objects collide is cheaper!)
        return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
    }

    bool IsEntityPlayer(Entity entity) {
        if (g_Coordinator.HaveComponent<CollisionComponent>(entity)) {
            return g_Coordinator.GetComponent<CollisionComponent>(entity).IsPlayer();
        }
        return false;
    }

    void OnContactAdded(const JPH::Body& inBody1,
        const JPH::Body& inBody2,
        const JPH::ContactManifold& inManifold,
        JPH::ContactSettings& ioSettings)
    {
        //std::cout << "Collision detected between bodies with IDs: "
        //    << inBody1.GetID().GetIndex() << " and "
        //    << inBody2.GetID().GetIndex() << std::endl;

        Entity entity1 = static_cast<Entity>(inBody1.GetUserData());
        Entity entity2 = static_cast<Entity>(inBody2.GetUserData());

        if (entity1 != invalid_entity && entity2 != invalid_entity) {
            // Get components for both entities
            auto& collisionComponent1 = g_Coordinator.GetComponent<CollisionComponent>(entity1);
            auto& collisionComponent2 = g_Coordinator.GetComponent<CollisionComponent>(entity2);

            // Capture the normal of the contact
            glm::vec3 contactNormal = glm::vec3(
                inManifold.mWorldSpaceNormal.GetX(),
                inManifold.mWorldSpaceNormal.GetY(),
                inManifold.mWorldSpaceNormal.GetZ());

            Entity playerEntity = IsEntityPlayer(entity1) ? entity1 : (IsEntityPlayer(entity2) ? entity2 : invalid_entity);
            if (playerEntity != invalid_entity) {
                Entity other = (playerEntity == entity1) ? entity2 : entity1;
                if (playerEntity == entity2) {
                    contactNormal = -contactNormal; // Flip normal to be from player�s POV
                }

                //std::cout << "[DEBUG] Normal from Player POV with entity " << other << ": ("
                //    << contactNormal.x << ", " << contactNormal.y << ", " << contactNormal.z << ")\n";

                if (contactNormal.y < -0.7f) {
                    auto& playerCollision = g_Coordinator.GetComponent<CollisionComponent>(playerEntity);
                    playerCollision.SetIsGrounded(true);
                    playerCollision.AddGroundContact();
                }
            }

            collisionComponent1.SetIsColliding(true);
            collisionComponent2.SetIsColliding(true);

            if (g_Coordinator.HaveComponent<MetadataComponent>(entity1))
            {
                auto& metaComponent = g_Coordinator.GetComponent<MetadataComponent>(entity2);

                // Set entity 1's collided object name to entity 2's name
                collisionComponent1.SetIsColliding(true);
				collisionComponent1.SetLastCollidedObjectName(metaComponent.GetName());

				//std::cout << "Entity " << entity1 << " is colliding with: "
				//	<< metaComponent.GetName() << std::endl;
            }

            if (g_Coordinator.HaveComponent<MetadataComponent>(entity2))
            {
                auto& metaComponent = g_Coordinator.GetComponent<MetadataComponent>(entity1);
                collisionComponent2.SetIsColliding(true);
                collisionComponent2.SetLastCollidedObjectName(metaComponent.GetName());

                //std::cout << "Entity " << entity2 << " is colliding with: "
                //    << metaComponent.GetName() << std::endl;
            }
        }
    }

    void OnContactPersisted(const JPH::Body& inBody1,
        const JPH::Body& inBody2,
        const JPH::ContactManifold& inManifold,
        JPH::ContactSettings& ioSettings) override
    {
        Entity entity1 = static_cast<Entity>(inBody1.GetUserData());
        Entity entity2 = static_cast<Entity>(inBody2.GetUserData());

        if (entity1 == invalid_entity || entity2 == invalid_entity) return;

        auto& collision1 = g_Coordinator.GetComponent<CollisionComponent>(entity1);
        auto& collision2 = g_Coordinator.GetComponent<CollisionComponent>(entity2);

        glm::vec3 contactNormal = glm::vec3(inManifold.mWorldSpaceNormal.GetX(),
            inManifold.mWorldSpaceNormal.GetY(),
            inManifold.mWorldSpaceNormal.GetZ());

        Entity playerEntity = IsEntityPlayer(entity1) ? entity1 : (IsEntityPlayer(entity2) ? entity2 : invalid_entity);
        if (playerEntity != invalid_entity) {
            Entity other = (playerEntity == entity1) ? entity2 : entity1;
            if (playerEntity == entity2) {
                contactNormal = -contactNormal;
            }

            //std::cout << "[DEBUG] Persisted Normal from Player POV with entity " << other << ": ("
            //    << contactNormal.x << ", " << contactNormal.y << ", " << contactNormal.z << ")\n";

            if (contactNormal.y < -0.7f) {
                auto& playerCollision = g_Coordinator.GetComponent<CollisionComponent>(playerEntity);
                if (playerCollision.GetGroundContacts() == 0) {
                    playerCollision.AddGroundContact();
                    playerCollision.SetIsGrounded(true);
                }
            }
        }
    }


    void OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair) override
    {
        // Retrieve the BodyIDs from the SubShapeIDPair
        JPH::BodyID bodyID1 = inSubShapePair.GetBody1ID();
        JPH::BodyID bodyID2 = inSubShapePair.GetBody2ID();

        // Map BodyIDs to Entities using the PhysicsSystem's mapping
        Entity entity1 = g_Coordinator.GetSystem<MyPhysicsSystem>()->GetEntityFromBody(bodyID1);
        Entity entity2 = g_Coordinator.GetSystem<MyPhysicsSystem>()->GetEntityFromBody(bodyID2);

        //// Debug message for contact removal
        //std::cout << "Contact removed between Body ID: " << bodyID1.GetIndex()
        //    << " and Body ID: " << bodyID2.GetIndex() << std::endl;

        //// Debug entity mapping
        //std::cout << "Entity 1: " << entity1 << ", Entity 2: " << entity2 << std::endl;

        // Reset isColliding flag for entity1
        if (entity1 != invalid_entity && g_Coordinator.HaveComponent<CollisionComponent>(entity1)) {
            auto& collisionComponent1 = g_Coordinator.GetComponent<CollisionComponent>(entity1);

            // Only remove ground contact if the removed collision was a floor
            if (collisionComponent1.GetCollisionNormal().y > -0.7f) {
                collisionComponent1.RemoveGroundContact();
                //std::cout << "[DEBUG] Entity " << entity1 << " lost a ground contact. Remaining: "
                //    << collisionComponent1.GetGroundContacts() << std::endl;
            }

            if (collisionComponent1.GetGroundContacts() == 0) {
                collisionComponent1.SetIsGrounded(false);
                //std::cout << "[DEBUG] Entity " << entity1 << " is airborne! Resetting isGrounded.\n";
            }
            //// Debugging the state before resetting
            //std::cout << "Entity 1 before reset: isColliding = " << collisionComponent1.GetIsColliding() << std::endl;

            // Check and reset if the body matches
            if (collisionComponent1.GetPhysicsBody() != nullptr && collisionComponent1.GetPhysicsBody()->GetID() == bodyID1) {
                collisionComponent1.SetIsColliding(false);

                //// Debugging the state after resetting
                //std::cout << "Entity 1 after reset: isColliding = " << collisionComponent1.GetIsColliding() << std::endl;
            }
            else {
                //std::cout << "Entity 1: Body mismatch or no physics body found." << std::endl;
            }
        }
        else {
            //std::cout << "Entity 1 is invalid or does not have a CollisionComponent." << std::endl;
        }

        // Reset isColliding flag for entity2
        if (entity2 != invalid_entity && g_Coordinator.HaveComponent<CollisionComponent>(entity2)) {
            auto& collisionComponent2 = g_Coordinator.GetComponent<CollisionComponent>(entity2);

            if (entity2 != invalid_entity && g_Coordinator.HaveComponent<CollisionComponent>(entity2)) {

                // Only remove ground contact if the removed collision was a floor
                if (collisionComponent2.GetCollisionNormal().y > -0.7f) {
                    collisionComponent2.RemoveGroundContact();
                    //std::cout << "[DEBUG] Entity " << entity2 << " lost a ground contact. Remaining: "
                    //    << collisionComponent2.GetGroundContacts() << std::endl;
                }

                if (collisionComponent2.GetGroundContacts() == 0) {
                    collisionComponent2.SetIsGrounded(false);
                    //std::cout << "[DEBUG] Entity " << entity2 << " is airborne! Resetting isGrounded.\n";
                }
            }
            //// Debugging the state before resetting
            //std::cout << "Entity 2 before reset: isColliding = " << collisionComponent2.GetIsColliding() << std::endl;

            // Check and reset if the body matches
            if (collisionComponent2.GetPhysicsBody() != nullptr && collisionComponent2.GetPhysicsBody()->GetID() == bodyID2) {
                collisionComponent2.SetIsColliding(false);

                //// Debugging the state after resetting
                //std::cout << "Entity 2 after reset: isColliding = " << collisionComponent2.GetIsColliding() << std::endl;
            }
            else {
                //std::cout << "Entity 2: Body mismatch or no physics body found." << std::endl;
            }
        }
        else {
            //std::cout << "Entity 2 is invalid or does not have a CollisionComponent." << std::endl;
        }
    }



private:
    JPH::PhysicsSystem* mPhysicsSystem = nullptr; // Reference or pointer to the physics system
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
