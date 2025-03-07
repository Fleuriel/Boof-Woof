/**************************************************************************
 * @file PhysicsSystem.cpp
 * @author 	Ang Jun Sheng Aloysius
 * @param DP email: a.junshengaloysius@digipen.edu [2201807]
 * @param Course: CS 3401
 * @param Course: Game Project 3
 * @date  23/09/2024 (23 SEPT 2024)
 * @brief
 *
 * This file contains the definitions to initialize the JoltPhysics System
 *
 *
 *************************************************************************/
#include "pch.h"
#include "PhysicsSystem.h"

#pragma warning(push)
#pragma warning(disable: 4505 4458)

#include <Jolt/Physics/Body/Body.h>                   // For Body
#include <Jolt/Physics/Body/BodyCreationSettings.h>   // For BodyCreationSettings
#include <Jolt/Physics/Collision/Shape/SphereShape.h> // For SphereShape
#include <Jolt/Physics/Collision/Shape/BoxShape.h>    // For BoxShape
#include <Jolt/Physics/Collision/Shape/ConvexShape.h> // For SphereShape
#include <Jolt/Physics/Body/BodyInterface.h>          // For BodyInterface
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <../Utilities/Components/CollisionComponent.hpp>
#include <../Utilities/Components/MetaData.hpp> // To get name of entity
#include <Jolt/Physics/Collision/RayCast.h>  // For JPH::RRayCast, JPH::RayCastResult
#include <Jolt/Physics/Collision/NarrowPhaseQuery.h>  // For JPH::NarrowPhaseQuery
#include <Jolt/Physics/Collision/CastResult.h>
#include <glm/gtx/rotate_vector.hpp> // Needed for glm::rotateX/Y
#include <vector>


std::unordered_map<Entity, float> m_PreviousYPositions;
bool MyPhysicsSystem::RayCastDebug = false;
using namespace JPH::literals;

static void MyTrace(const char* inFMT, ...)
{
    va_list args;
    va_start(args, inFMT);
    vprintf(inFMT, args);
    va_end(args);
    printf("\n");
}

static bool MyAssertFailed(const char* inExpression, const char* inMessage, const char* inFile, unsigned int inLine)
{
    printf("Assert failed: %s, Message: %s, File: %s, Line: %d\n", inExpression, inMessage ? inMessage : "", inFile, inLine);
    return true; // Trigger a breakpoint if running under a debugger.
}

// Define mappings between texture names and ObjectType
ObjectType MyPhysicsSystem::GetObjectTypeFromModel(const std::string& modelName) {
    ObjectType type = ObjectType::Default;

    if (modelName == "Table") {
        type = ObjectType::Table;
    }
    else if (modelName == "Table2") {
        type = ObjectType::Table2;
    }
    else if (modelName.find("Chair") != std::string::npos) {
        type = ObjectType::Chair;
    }
    else if (modelName.find("Bed") != std::string::npos) {
        type = ObjectType::Bed;
    }
    else if (modelName.find("Couch") != std::string::npos) {
        type = ObjectType::Couch;
    }
    else if (modelName == "Corgi2") {
        type = ObjectType::Corgi;
    }
    else if (modelName == "Corgi_small") {
        type = ObjectType::Corgi_small;
    }
    else if (modelName.find("FloorCastle") != std::string::npos) {
        type = ObjectType::FloorCastle;
    }
    else if (modelName.find("Wall") != std::string::npos) {
        type = ObjectType::Wall;
    }
    else if (modelName == "Wardrobe") {
        type = ObjectType::Wardrobe;
    }
    else if (modelName == "Wardrobe2") {
        type = ObjectType::Wardrobe2;
    }

    // Debug statement to check which ObjectType is set
    std::cout << "Model Name: " << modelName << " | Set ObjectType: ";
    switch (type) {
    case ObjectType::Table:
        std::cout << "Table";
        break;
    case ObjectType::Table2:
        std::cout << "Table2";
        break;
    case ObjectType::Chair:
        std::cout << "Chair";
        break;
    case ObjectType::Bed:
        std::cout << "Bed";
        break;
    case ObjectType::Couch:
        std::cout << "Couch";
        break;
    case ObjectType::Corgi:
        std::cout << "Corgi2";
        break;
    case ObjectType::Corgi_small:
        std::cout << "Corgi_small";
        break;
    case ObjectType::FloorCastle:
        std::cout << "FloorCastle";
        break;
    case ObjectType::Wall:
        std::cout << "Wall";
        break;
    case ObjectType::Wardrobe:
        std::cout << "Wardrobe";
        break;
    case ObjectType::Wardrobe2:
        std::cout << "Wardrobe2";
        break;
    default:
        std::cout << "Default";
        break;
    }
    std::cout << std::endl;

    return type;
}

JPH::Shape* MyPhysicsSystem::CreateShapeForObjectType(ObjectType type, const glm::vec3& customAABB) {
    // If customAABB is provided, use it. Otherwise, fall back to default dimensions.
    glm::vec3 dimensions = (customAABB != glm::vec3(0.0f)) ? customAABB : glm::vec3(1.0f); // Default base dimension for safety

    switch (type) {
    case ObjectType::Bed:
        dimensions = (customAABB != glm::vec3(0.0f)) ? customAABB : glm::vec3(1.5f, 1.8f, 2.0f);
        break;
    case ObjectType::Chair:
        dimensions = (customAABB != glm::vec3(0.0f)) ? customAABB : glm::vec3(0.5f, 0.8f, 0.5f);
        break;
    case ObjectType::Table:
        dimensions = (customAABB != glm::vec3(0.0f)) ? customAABB : glm::vec3(2.0f, 0.4f, 2.0f);
        break;
    case ObjectType::Table2:
        dimensions = (customAABB != glm::vec3(0.0f)) ? customAABB : glm::vec3(0.8f, 0.4f, 2.0f);
        break;
    case ObjectType::Couch:
        dimensions = (customAABB != glm::vec3(0.0f)) ? customAABB : glm::vec3(1.5f, 0.6f, 1.0f);
        break;
    case ObjectType::Corgi:
        dimensions = (customAABB != glm::vec3(0.0f)) ? customAABB : glm::vec3(2.8f, 0.3f, 1.3f);
        break;
    case ObjectType::Corgi_small:
        dimensions = (customAABB != glm::vec3(0.0f)) ? customAABB : glm::vec3(1.4f, 0.5f, 0.6f);
        break;
    case ObjectType::FloorCastle:
        dimensions = (customAABB != glm::vec3(0.0f)) ? customAABB : glm::vec3(5.0f, 0.15f, 4.2f);
        break;
    case ObjectType::Wall:
        dimensions = (customAABB != glm::vec3(0.0f)) ? customAABB : glm::vec3(2.5f, 3.0f, 0.8f);
        break;
    case ObjectType::Wardrobe:
        dimensions = (customAABB != glm::vec3(0.0f)) ? customAABB : glm::vec3(2.0f, 2.0f, 1.0f);
        break;
    case ObjectType::Wardrobe2:
        dimensions = (customAABB != glm::vec3(0.0f)) ? customAABB : glm::vec3(2.8f, 2.0f, 1.0f);
        break;
    default:
        dimensions = (customAABB != glm::vec3(0.0f)) ? customAABB : glm::vec3(0.5f, 0.5f, 0.5f);
        break;
    }

    return new JPH::BoxShape(JPH::Vec3(dimensions.x, dimensions.y, dimensions.z));
}


class CustomRayCastCollector : public JPH::CastRayCollector {
public:
    Entity ignoreEntity;
    Entity hitEntity = invalid_entity;
    float closestFraction = FLT_MAX;

    explicit CustomRayCastCollector(Entity ignore) : ignoreEntity(ignore) {}

    void AddHit(const JPH::RayCastResult& result) override {
        Entity entityHit = g_Coordinator.GetSystem<MyPhysicsSystem>()->GetEntityFromBody(result.mBodyID);

        // Ignore self-hits (Rex)
        if (entityHit == ignoreEntity) return;

        // Keep track of the closest valid hit
        if (result.mFraction < closestFraction) {
            closestFraction = result.mFraction;
            hitEntity = entityHit;
        }
    }
};

void MyPhysicsSystem::InitializeJolt() {

    // Register default allocator
    JPH::RegisterDefaultAllocator();

    // Install trace and assert callbacks
    JPH::Trace = MyTrace;
    JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = MyAssertFailed;)

    // Create a factory, this class is responsible for creating instances of classes based on their name or hash.
    JPH::Factory::sInstance = new JPH::Factory();

    // Register physics types
    JPH::RegisterTypes();

    // Initialize the PhysicsSystem (this creates the main physics world for Jolt)
    CreatePhysicsSystem();

    // Register the contact listener to handle collision events
    mContactListener = new MyContactListener(mPhysicsSystem);
    mPhysicsSystem->SetContactListener(mContactListener);

    //mContactListener = new MyContactListener();  // Instantiate the listener
    //mContactListener->SetPhysicsSystem(mPhysicsSystem); // Set physics system after initialization
    //mPhysicsSystem->SetContactListener(mContactListener);

    mPhysicsSystem->SetContactListener(mContactListener);  // Register the listener

    // Register the body activation listener to handle collision events
    mBodyActivationListener = new MyBodyActivationListener();  // Instantiate the listener
    mPhysicsSystem->SetBodyActivationListener(mBodyActivationListener);  // Register the listener

}

JPH::PhysicsSystem* MyPhysicsSystem::CreatePhysicsSystem() {
    const unsigned int cMaxBodies = 1024;       // Max number of bodies
    const unsigned int cNumBodyMutexes = 16;    // Choose a power of 2 in the range [1, 64]
    const unsigned int cMaxBodyPairs = 1024;    // Max number of body pairs for collision detection
    const unsigned int cMaxContactConstraints = 1024; // Max number of contact constraints

    // Create the PhysicsSystem instance
    mPhysicsSystem = new JPH::PhysicsSystem();

    m_broad_phase_layer_interface = new BPLayerInterfaceImpl();
    m_object_vs_broadphase_layer_filter = new ObjectVsBroadPhaseLayerFilterImpl();
    m_object_vs_object_layer_filter = new ObjectLayerPairFilterImpl();

    // Initialize mTempAllocator with a fixed amount of memory (10 MB here)
    mTempAllocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024);

    // Create the JobSystemThreadPool
    mJobSystem = new JPH::JobSystemThreadPool(
        JPH::cMaxPhysicsJobs,
        JPH::cMaxPhysicsBarriers,
        static_cast<int>(std::thread::hardware_concurrency()) - 1
    );

    // Initialize the PhysicsSystem with the filters and interfaces
    mPhysicsSystem->Init(
        cMaxBodies,
        cNumBodyMutexes,
        cMaxBodyPairs,
        cMaxContactConstraints,
        *m_broad_phase_layer_interface,
        *m_object_vs_broadphase_layer_filter,
        *m_object_vs_object_layer_filter
    );

    // Set gravity
    mPhysicsSystem->SetGravity(JPH::Vec3(0.0f, -14.f, 0.0f));
    //mPhysicsSystem->SetGravity(JPH::Vec3(0.0f, 0.0f, 0.0f));

    std::cout << "Jolt physics system created" << std::endl;
    return mPhysicsSystem;
}

void MyPhysicsSystem::OnUpdate(float deltaTime) {
    ++_step;

    JPH::BodyInterface& bodyInterface = mPhysicsSystem->GetBodyInterface();

    // Track previous Y-positions for entities
    static std::unordered_map<Entity, float> previousYPositions;

    // Iterate through all entities that match the PhysicsSystem signature
    auto allEntities = g_Coordinator.GetAliveEntitiesSet();

    //bool isGrounded = false;

    for (auto& entity : allEntities) {
        if (g_Coordinator.HaveComponent<TransformComponent>(entity) && g_Coordinator.HaveComponent<CollisionComponent>(entity) 
            && g_Coordinator.HaveComponent<GraphicsComponent>(entity)) {
            auto& collisionComponent = g_Coordinator.GetComponent<CollisionComponent>(entity);
            auto& transform = g_Coordinator.GetComponent<TransformComponent>(entity);
            auto& graphicsComp = g_Coordinator.GetComponent<GraphicsComponent>(entity);

            JPH::RVec3 position(transform.GetPosition().x, transform.GetPosition().y, transform.GetPosition().z);
            glm::vec3 scale = transform.GetScale();

            // Validate the GraphicsComponent's model
            if (graphicsComp.getModelName().empty()) {
                //std::cerr << "Entity " << entity << " has an invalid or missing model. Skipping body creation." << std::endl;
                continue;
            }

            // Only add a new body if it hasn't already been added
            if (!collisionComponent.HasBodyAdded()) {
                AddEntityBody(entity, 0.0f);
                collisionComponent.SetHasBodyAdded(true); // Mark as added
            }

            // Retrieve the physics body
            JPH::Body* body = collisionComponent.GetPhysicsBody();

            if (body != nullptr && body->GetMotionType() == JPH::EMotionType::Dynamic) {
                JPH::Vec3 gravity = mPhysicsSystem->GetGravity();
                JPH::Vec3 velocity = body->GetLinearVelocity();
                //std::cout << "Entity ID: " << entity
                //    << " Gravity: (" << gravity.GetX() << ", " << gravity.GetY() << ", " << gravity.GetZ() << ")"
                //    << " Velocity: (" << velocity.GetX() << ", " << velocity.GetY() << ", " << velocity.GetZ() << ")"
                //    << std::endl;
            }

            if (body != nullptr && !body->GetID().IsInvalid()) {

                // Check if the entity is grounded
                if (collisionComponent.IsDynamic() && collisionComponent.IsPlayer()) {

                    if (collisionComponent.HasOngoingCollisions()) {
                        collisionComponent.SetIsColliding(true);
                        //isGrounded = true; // Ensure grounding as soon as a collision is detected
                    }
                    else {
                        //collisionComponent.SetIsColliding(false);
                    }

                    float currentYPosition = transform.GetPosition().y;

                    //// Check if the Y-position has minimal change and the entity is colliding
                    //if (previousYPositions.find(entity) != previousYPositions.end()) {
                    //    float previousYPosition = previousYPositions[entity];
                    //    float yChange = std::abs(currentYPosition - previousYPosition);

                    //    //if (yChange < 0.0000001f && collisionComponent.GetIsColliding()) {
                    //    //    isGrounded = true;       
                    //    //}
                    //    if (yChange < 0.000001f) {
                    //        isGrounded = true;
                    //    }
                    //}

                   // If got ground contacts set to true
                    if (collisionComponent.GetGroundContacts() > 0) {
                        collisionComponent.SetIsGrounded(true);
                        //std::cout << "[DEBUG] Entity " << entity << " is still grounded due to active ground contacts.\n";
                    }

                    // Update `isGrounded` status
                    //collisionComponent.SetIsGrounded(isGrounded);

                    // Update the previous Y-position
                    previousYPositions[entity] = currentYPosition;
                }

                // Debug output to check Position and Velocity for Dynamic Bodies
                if (body->GetMotionType() == JPH::EMotionType::Dynamic) {
                    JPH::Vec3 velocity = bodyInterface.GetLinearVelocity(body->GetID());
                    //std::cout << "Entity ID: " << entity << " Position = ("
                    //    << position.GetX() << ", "
                    //    << position.GetY() << ", "
                    //    << position.GetZ() << "), Velocity = ("
                    //    << velocity.GetX() << ", "
                    //    << velocity.GetY() << ", "
                    //    << velocity.GetZ() << ")" << std::endl;
                }
                else 
                {
                    //std::cout << "Entity ID: " << entity << " Position = ("
                    //    << position.GetX() << ", "
                    //    << position.GetY() << ", "
                    //    << position.GetZ() << ")" << std::endl;

                }
            }

            //if (collisionComponent.GetIsColliding()) 
            //{
            //    std::cout << "Entity " << entity << " is colliding with: " << collisionComponent.GetLastCollidedObjectName() << std::endl;
            //}
        }
    }

    // Simulate physics
    mPhysicsSystem->Update(deltaTime, 1, mTempAllocator, mJobSystem);

    // Update the entities' transforms after simulation
    UpdateEntityTransforms();
}

void MyPhysicsSystem::AddEntityBody(Entity entity, float mass) {
    if (g_Coordinator.HaveComponent<TransformComponent>(entity) &&
        g_Coordinator.HaveComponent<GraphicsComponent>(entity) &&
        g_Coordinator.HaveComponent<CollisionComponent>(entity)) {

        auto& transform = g_Coordinator.GetComponent<TransformComponent>(entity);
        auto& graphicsComp = g_Coordinator.GetComponent<GraphicsComponent>(entity);
        auto& collisionComponent = g_Coordinator.GetComponent<CollisionComponent>(entity);

        std::string modelName = graphicsComp.getModelName();

        if (modelName.empty()) {
            std::cerr << "AddEntityBody: Entity " << entity << " has an invalid or missing model." << std::endl;
            return;
        }

        ObjectType objectType = GetObjectTypeFromModel(modelName);

        glm::vec3 scale = transform.GetScale();
        glm::quat glmRotation = transform.GetRotation();
        JPH::Quat joltRotation(glmRotation.x, glmRotation.y, glmRotation.z, glmRotation.w);
        glm::vec3 defaultAABB = collisionComponent.GetAABBSize() == glm::vec3(1.0f, 1.0f, 1.0f)
            ? glm::vec3(1.0f)
            : collisionComponent.GetAABBSize();
        glm::vec3 scaledAABB = defaultAABB * scale;

        glm::vec3 offset = collisionComponent.GetAABBOffset();
        JPH::RVec3 positionWithOffset(transform.GetPosition().x + offset.x,
            transform.GetPosition().y + offset.y,
            transform.GetPosition().z + offset.z);

        JPH::Shape* shape = CreateShapeForObjectType(objectType, scaledAABB);
        bool isDynamic = collisionComponent.IsDynamic();
        JPH::EMotionType motionType = isDynamic ? JPH::EMotionType::Dynamic : JPH::EMotionType::Static;

        JPH::BodyCreationSettings bodySettings(
            shape,
            positionWithOffset,
            joltRotation,
            motionType,
            motionType == JPH::EMotionType::Dynamic ? Layers::MOVING : Layers::NON_MOVING
        );

        JPH::BodyInterface& bodyInterface = mPhysicsSystem->GetBodyInterface();
        JPH::Body* body = bodyInterface.CreateBody(bodySettings);

        if (!body) {
            std::cerr << "Failed to create a new body for entity: " << entity << std::endl;
            return;
        }

        bodySettings.mAllowedDOFs = JPH::EAllowedDOFs::All; // Allow full motion
        
        if (isDynamic) {
            body->SetAllowSleeping(false);

            // Provide mass and inertia for dynamic bodies
            bodySettings.mOverrideMassProperties = JPH::EOverrideMassProperties::MassAndInertiaProvided;
            bodySettings.mMassPropertiesOverride.SetMassAndInertiaOfSolidBox(
                JPH::Vec3(scaledAABB.x, scaledAABB.y, scaledAABB.z),
                0.0f // Default mass
            );

            float actualMass = bodySettings.mMassPropertiesOverride.mMass;
            collisionComponent.SetActualMass(actualMass);
            //std::cout << "Mass: " << actualMass << std::endl;
        }

        bodyInterface.AddBody(body->GetID(), JPH::EActivation::Activate);
        body->SetUserData(static_cast<uintptr_t>(entity));
        collisionComponent.SetPhysicsBody(body);
        bodyToEntityMap[body->GetID()] = entity;
    }
}


void MyPhysicsSystem::UpdateEntityBody(Entity entity, float mass)
{
    if (g_Coordinator.HaveComponent<CollisionComponent>(entity))
    {
        auto& collisionComponent = g_Coordinator.GetComponent<CollisionComponent>(entity);
        auto& transform = g_Coordinator.GetComponent<TransformComponent>(entity);

        // Remove the existing body if it exists
        JPH::Body* oldBody = collisionComponent.GetPhysicsBody();
        if (oldBody != nullptr && !oldBody->GetID().IsInvalid())
        {
            // Reset isColliding flag for the old body
            collisionComponent.SetIsColliding(false);

            mPhysicsSystem->GetBodyInterface().RemoveBody(oldBody->GetID());

            // Destroy the body
            mPhysicsSystem->GetBodyInterface().DestroyBody(oldBody->GetID());

            // Remove the body from the mapping
            bodyToEntityMap.erase(oldBody->GetID());
        }

        // If the entity is a player, it must be dynamic
        if (collisionComponent.IsPlayer())
        {
            collisionComponent.SetIsDynamic(true);
        }

        // Determine motion type based on the dynamic/static flag
        //JPH::EMotionType motionType = collisionComponent.IsDynamic() ? JPH::EMotionType::Dynamic : JPH::EMotionType::Static;

        // Set motion type based on IsDynamic in CollisionComponent
        bool isDynamic = collisionComponent.IsDynamic();
        JPH::EMotionType motionType = isDynamic ? JPH::EMotionType::Dynamic : JPH::EMotionType::Static;

        // Compute the scaled AABB
        glm::vec3 scale = transform.GetScale();
        glm::vec3 scaledAABB = collisionComponent.GetAABBSize() * scale;

        // Include offset in body position
        glm::vec3 offset = collisionComponent.GetAABBOffset();
        JPH::RVec3 positionWithOffset = JPH::RVec3(
            transform.GetPosition().x + offset.x,
            transform.GetPosition().y + offset.y,
            transform.GetPosition().z + offset.z
        );

        // Get the rotation from TransformComponent
        glm::quat glmRotation = transform.GetRotation();
        JPH::Quat joltRotation(glmRotation.x, glmRotation.y, glmRotation.z, glmRotation.w);

        //// Debug: Print both rotations for comparison
        //std::cout << "[DEBUG] Entity: " << entity << std::endl;
        //std::cout << "  GLM Rotation: (" << glmRotation.w << ", "
        //    << glmRotation.x << ", "
        //    << glmRotation.y << ", "
        //    << glmRotation.z << ")" << std::endl;

        //std::cout << "  Jolt Rotation: (" << joltRotation.GetW() << ", "
        //    << joltRotation.GetX() << ", "
        //    << joltRotation.GetY() << ", "
        //    << joltRotation.GetZ() << ")" << std::endl;

        // Create a new shape and body
        //JPH::Shape* newShape = CreateShapeForObjectType(ObjectType::Default, transform.GetScale(), collisionComponent.GetAABBSize());
        JPH::Shape* newShape = CreateShapeForObjectType(ObjectType::Default, scaledAABB);

        JPH::BodyCreationSettings bodySettings(
            newShape,
            //JPH::RVec3(transform.GetPosition().x, transform.GetPosition().y, transform.GetPosition().z),
            positionWithOffset,
            //JPH::Quat::sIdentity(), // Default rotation
            joltRotation,
            motionType,
            motionType == JPH::EMotionType::Dynamic ? Layers::MOVING : Layers::NON_MOVING
        );

        // Allow full rotational freedom
        bodySettings.mAllowedDOFs = JPH::EAllowedDOFs::All;

        JPH::BodyInterface& bodyInterface = mPhysicsSystem->GetBodyInterface();
        JPH::Body* newBody = bodyInterface.CreateBody(bodySettings);

        if (isDynamic) {
            newBody->SetAllowSleeping(false);
            float massD = collisionComponent.GetMass();
            bodySettings.mOverrideMassProperties = JPH::EOverrideMassProperties::MassAndInertiaProvided;
            bodySettings.mMassPropertiesOverride.SetMassAndInertiaOfSolidBox(
                JPH::Vec3(scaledAABB.x, scaledAABB.y, scaledAABB.z),
                massD // Default mass
            );

            float actualMass = bodySettings.mMassPropertiesOverride.mMass;
            collisionComponent.SetActualMass(actualMass);
            //std::cout << "Mass: " << actualMass << std::endl;
        }

        bodyInterface.AddBody(newBody->GetID(), JPH::EActivation::Activate);

        // Update the CollisionComponent with the new body
        collisionComponent.SetPhysicsBody(newBody);
        collisionComponent.SetHasBodyAdded(true);
        bodyToEntityMap[newBody->GetID()] = entity;

        //// Debug log for motion type
        //std::cout << "Updated body for Entity " << entity
        //    << " to " << (collisionComponent.IsDynamic() ? "Dynamic" : "Static")
        //    << (collisionComponent.IsPlayer() ? " (Player)" : "") << "." << std::endl;
    }
}


void MyPhysicsSystem::UpdateEntityTransforms() {
    auto allEntities = g_Coordinator.GetAliveEntitiesSet();
    for (auto& entity : allEntities) {
        if (g_Coordinator.HaveComponent<CollisionComponent>(entity)) {
            auto& collisionComp = g_Coordinator.GetComponent<CollisionComponent>(entity);
            auto& transform = g_Coordinator.GetComponent<TransformComponent>(entity);
            JPH::Body* body = collisionComp.GetPhysicsBody();

            // Skip updating if the entity is being edited
            if (transform.IsEditing()) {
                continue;
            }

            if (body != nullptr && !body->GetID().IsInvalid()) {
                // Physics position from Jolt
                JPH::Vec3 bodyPosition = body->GetPosition();

                // Remove the offset to get the entity's true position
                glm::vec3 offset = collisionComp.GetAABBOffset();
                glm::vec3 truePosition = glm::vec3(
                    bodyPosition.GetX() - offset.x,
                    bodyPosition.GetY() - offset.y,
                    bodyPosition.GetZ() - offset.z
                );

                // Update the engine transform to match true position
                transform.SetPosition(truePosition);
            }
        }
    }
}

void MyPhysicsSystem::RemoveEntityBody(Entity entity) {
    // Check if the entity has a CollisionComponent
    if (g_Coordinator.HaveComponent<CollisionComponent>(entity)) {
        auto& collisionComponent = g_Coordinator.GetComponent<CollisionComponent>(entity);
        JPH::Body* body = collisionComponent.GetPhysicsBody();

        if (body != nullptr && !body->GetID().IsInvalid()) {
            // Get the BodyInterface to manage the physics body
            JPH::BodyInterface& bodyInterface = mPhysicsSystem->GetBodyInterface();

            // Remove and destroy the body
            bodyInterface.RemoveBody(body->GetID());      // Remove the body from the simulation
            bodyInterface.DestroyBody(body->GetID());     // Destroy the body to free resources

            // Set the CollisionComponent's physics body pointer to nullptr
            collisionComponent.SetPhysicsBody(nullptr);
            std::cout << "Body for entity " << entity << " removed and destroyed." << std::endl;
        }

        // Finally, remove the CollisionComponent from the entity
        g_Coordinator.RemoveComponent<CollisionComponent>(entity);
    }
}


void MyPhysicsSystem::ClearAllBodies() {
    if (mPhysicsSystem == nullptr) return;

    JPH::BodyInterface& bodyInterface = mPhysicsSystem->GetBodyInterface();

    // Iterate through all entities in the ECS
    auto allEntities = g_Coordinator.GetAliveEntitiesSet();
    for (auto& entity : allEntities) {
        // Check if the entity has a CollisionComponent with a physics body
        if (g_Coordinator.HaveComponent<CollisionComponent>(entity)) {
            auto& collisionComponent = g_Coordinator.GetComponent<CollisionComponent>(entity);
            JPH::Body* body = collisionComponent.GetPhysicsBody();

            if (body != nullptr && !body->GetID().IsInvalid()) {
                // First, remove the body from the physics system
                bodyInterface.RemoveBody(body->GetID());

                // Then, destroy the body using the BodyInterface to fully remove it
                bodyInterface.DestroyBody(body->GetID());

                // Set the CollisionComponent's physics body to nullptr
                collisionComponent.SetPhysicsBody(nullptr);
            }

            // Remove CollisionComponent from the entity
            g_Coordinator.RemoveComponent<CollisionComponent>(entity);
        }
    }

    std::cout << "All bodies removed from the physics system and components cleared." << std::endl;
}

void MyPhysicsSystem::Cleanup() {

    ClearAllBodies(); // Ensure all bodies are removed before other cleanup

    delete mPhysicsSystem;
    delete mJobSystem;
    delete mTempAllocator;
    delete m_broad_phase_layer_interface;
    delete m_object_vs_broadphase_layer_filter;
    delete m_object_vs_object_layer_filter;

    // Clean up the contact listener
    delete mContactListener;
    delete mBodyActivationListener;

    // Unregisters all types with the factory and cleans up the default material
    JPH::UnregisterTypes();

    // Destroy the factory
    delete JPH::Factory::sInstance;
    JPH::Factory::sInstance = nullptr;

    std::cout << "Physics system cleanup done" << std::endl;
}

Entity MyPhysicsSystem::GetEntityFromBody(const JPH::BodyID bodyID) {
    auto it = bodyToEntityMap.find(bodyID);
    if (it != bodyToEntityMap.end()) {
        return it->second;
    }
    return invalid_entity; // Sentinel value for invalid entity
}

Entity MyPhysicsSystem::Raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance, Entity ignoreEntity) {
    if (!mPhysicsSystem) {
        std::cerr << "[PhysicsSystem] ERROR: Physics system is not initialized!" << std::endl;
        return invalid_entity;
    }

    glm::vec3 normalizedDir = glm::normalize(direction);
    JPH::RRayCast ray(JPH::RVec3(origin.x, origin.y, origin.z), JPH::Vec3(normalizedDir.x, normalizedDir.y, normalizedDir.z) * maxDistance);

    CustomRayCastCollector collector(ignoreEntity);
    const JPH::NarrowPhaseQuery& npQuery = mPhysicsSystem->GetNarrowPhaseQueryNoLock();

    // Use the multi-hit version of CastRay()
    npQuery.CastRay(ray, JPH::RayCastSettings(), collector);

    // Compute the end point of the ray
    glm::vec3 endPoint = origin + normalizedDir * maxDistance;

    //// Debug output
    //std::cout << "[PhysicsSystem] Raycast Debug -> Origin: (" << origin.x << ", " << origin.y << ", " << origin.z
    //    << ") | Direction: (" << normalizedDir.x << ", " << normalizedDir.y << ", " << normalizedDir.z
    //    << ") | Max Distance: " << maxDistance << std::endl;

    //// Return the closest valid hit
    //if (collector.hitEntity != invalid_entity) {
    //    std::cout << "[PhysicsSystem] Ray HIT entity: " << collector.hitEntity << " at fraction: " << collector.closestFraction << std::endl;
    //}
    //else {
    //    std::cout << "[PhysicsSystem] No objects detected along the entire ray!" << std::endl;
    //}

    // If a hit is detected
    if (collector.hitEntity != invalid_entity) {
        glm::vec3 hitPoint = origin + normalizedDir * collector.closestFraction * maxDistance;

        // **Fetch entity name (if exists)**
        std::string entityName = "Unknown";
        if (g_Coordinator.HaveComponent<MetadataComponent>(collector.hitEntity)) {
            entityName = g_Coordinator.GetComponent<MetadataComponent>(collector.hitEntity).GetName();
        }

        //std::cout << "[PhysicsSystem] Ray HIT entity: " << collector.hitEntity
        //    << " (" << entityName << ") at fraction: " << collector.closestFraction << std::endl;

        // **Draw a green debug line to the hit point**
        if (RayCastDebug == true)
            GraphicsSystem::AddDebugLine(origin, hitPoint, glm::vec3(0.0f, 1.0f, 0.0f)); // Green for hit

        return collector.hitEntity;
    }

    // **Draw a red debug line for a full-length miss**
    if (RayCastDebug == true)
        GraphicsSystem::AddDebugLine(origin, endPoint, glm::vec3(1.0f, 0.0f, 0.0f)); // Red for miss

    return collector.hitEntity;
}

std::vector<Entity> MyPhysicsSystem::ConeRaycast(
    Entity entity,
    const glm::vec3& forwardDirection,
    float maxDistance,
    int numHorizontalRays, int numVerticalRays,
    float horizontalFOVAngle, float verticalFOVAngle,  // User-defined angles
    const glm::vec3& userOffset)
{
    std::vector<Entity> detectedEntities;

    // Ensure entity exists
    if (!g_Coordinator.HaveComponent<CollisionComponent>(entity) ||
        !g_Coordinator.HaveComponent<TransformComponent>(entity))
    {
        std::cerr << "[PhysicsSystem] Error: Entity " << entity << " does not have necessary components!\n";
        return detectedEntities;
    }

    // Get Entity Position and AABB offset
    glm::vec3 entityPosition = g_Coordinator.GetComponent<TransformComponent>(entity).GetPosition();
    auto& collisionComp = g_Coordinator.GetComponent<CollisionComponent>(entity);
    glm::vec3 aabbOffset = collisionComp.GetAABBOffset();

    // Adjust the origin based on the user-defined offset
    glm::vec3 adjustedOrigin = entityPosition + aabbOffset + userOffset;

    // Compute right and upward vectors relative to forward
    glm::vec3 rightVector = glm::cross(forwardDirection, glm::vec3(0.0f, 1.0f, 0.0f)); // Right vector
    glm::vec3 upVector = glm::cross(rightVector, forwardDirection); // Upward vector

    // Convert user-defined angles to radians
    float horizontalAngleRad = glm::radians(horizontalFOVAngle);
    float verticalAngleRad = glm::radians(verticalFOVAngle);

    // Iterate over the cone angles using **local space spherical coordinates**
    for (int h = 0; h < numHorizontalRays; h++)  // Spread horizontally
    {
        float horizontalAngle = glm::mix(-horizontalAngleRad, horizontalAngleRad, float(h) / (numHorizontalRays - 1));

        for (int v = 0; v < numVerticalRays; v++)  // Spread vertically (including downward)
        {
            float verticalAngle = glm::mix(-verticalAngleRad, verticalAngleRad, float(v) / (numVerticalRays - 1));

            // Apply horizontal rotation (around the Y-axis)
            glm::vec3 rotatedDirection = glm::normalize(
                glm::rotate(glm::mat4(1.0f), horizontalAngle, glm::vec3(0, 1, 0)) *
                glm::vec4(forwardDirection, 0.0f)
            );

            // Apply vertical tilt (around the right vector) to include downward angles
            rotatedDirection = glm::normalize(
                glm::rotate(glm::mat4(1.0f), verticalAngle, rightVector) *
                glm::vec4(rotatedDirection, 0.0f)
            );

            // Construct the ray
            JPH::RRayCast ray(
                JPH::RVec3(adjustedOrigin.x, adjustedOrigin.y, adjustedOrigin.z),
                JPH::Vec3(rotatedDirection.x, rotatedDirection.y, rotatedDirection.z) * maxDistance
            );

            // Create a custom collector for this ray
            CustomRayCastCollector collector(entity);
            const JPH::NarrowPhaseQuery& npQuery = mPhysicsSystem->GetNarrowPhaseQueryNoLock();
            npQuery.CastRay(ray, JPH::RayCastSettings(), collector);

            // If a hit is found (and ignore self-hits), compute the hit point:
            if (collector.hitEntity != invalid_entity && collector.hitEntity != entity)
            {
                detectedEntities.push_back(collector.hitEntity);
                glm::vec3 hitPoint = adjustedOrigin + rotatedDirection * collector.closestFraction * maxDistance;

                // **Fetch entity name (if exists)**
                std::string entityName = "Unknown";
                if (g_Coordinator.HaveComponent<MetadataComponent>(collector.hitEntity)) {
                    entityName = g_Coordinator.GetComponent<MetadataComponent>(collector.hitEntity).GetName();
                }

                std::cout << "[PhysicsSystem] Cone Ray HIT entity: " << collector.hitEntity
                    << " (" << entityName << ") at fraction: " << collector.closestFraction << std::endl;

                if (RayCastDebug == true)
                    GraphicsSystem::AddDebugLine(adjustedOrigin, hitPoint, glm::vec3(0.0f, 1.0f, 0.0f));  // Green for hit
            }
            else
            {
                if (RayCastDebug == true)
                    GraphicsSystem::AddDebugLine(adjustedOrigin, adjustedOrigin + rotatedDirection * maxDistance, glm::vec3(1.0f, 0.0f, 0.0f));  // Red for miss
            }
        }
    }

    return detectedEntities;
}

std::vector<Entity> MyPhysicsSystem::ConeRaycastDownward(
    Entity entity,
    const glm::vec3& baseDirection,
    float maxDistance,
    int numLengthwiseRays, int numVerticalRays,
    float coneAngle,
    const glm::vec3& userOffset)
{
    std::vector<Entity> detectedEntities;

    // Ensure entity exists
    if (!g_Coordinator.HaveComponent<CollisionComponent>(entity) ||
        !g_Coordinator.HaveComponent<TransformComponent>(entity))
    {
        std::cerr << "[PhysicsSystem] Error: Entity " << entity << " does not have necessary components!\n";
        return detectedEntities;
    }

    // Get Entity Position and AABB offset
    glm::vec3 entityPosition = g_Coordinator.GetComponent<TransformComponent>(entity).GetPosition();
    auto& collisionComp = g_Coordinator.GetComponent<CollisionComponent>(entity);
    glm::vec3 aabbOffset = collisionComp.GetAABBOffset();
    glm::vec3 entityScale = g_Coordinator.GetComponent<TransformComponent>(entity).GetScale();
    glm::vec3 entityRotation = g_Coordinator.GetComponent<TransformComponent>(entity).GetRotation();

    // Adjust the origin to the center of the entity
    glm::vec3 adjustedOrigin = entityPosition + aabbOffset + userOffset;

    // Compute the forward vector from rotation
    float yaw = entityRotation.y;
    glm::vec3 forward = glm::normalize(glm::vec3(glm::cos(yaw), 0.0f, -glm::sin(yaw)));

    // Compute the right and up vectors relative to the forward direction
    glm::vec3 rightVector = glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)); // Right vector (for lengthwise spread)
    glm::vec3 downVector = glm::vec3(0.0f, -1.0f, 0.0f); // Always downward

    // Iterate over the cone angles using Rex's local space (lengthwise spread along forward direction)
    for (int l = 0; l < numLengthwiseRays; l++)  // Spread along the length of the dog
    {
        float lengthAngle = glm::radians(glm::mix(-coneAngle, coneAngle, float(l) / (numLengthwiseRays - 1)));

        for (int v = 0; v < numVerticalRays; v++)  // Vertical spread
        {
            float verticalAngle = glm::radians(glm::mix(-coneAngle / 2.0f, coneAngle / 2.0f, float(v) / (numVerticalRays - 1)));

            // Rotate the downward vector along the **rightVector** (spreading along the length of the dog)
            glm::vec3 rotatedDirection = glm::normalize(
                glm::rotate(glm::mat4(1.0f), lengthAngle, rightVector) *
                glm::vec4(downVector, 0.0f)
            );

            // Apply vertical tilt
            rotatedDirection = glm::normalize(
                glm::rotate(glm::mat4(1.0f), verticalAngle, forward) *
                glm::vec4(rotatedDirection, 0.0f)
            );

            // Construct the ray
            JPH::RRayCast ray(
                JPH::RVec3(adjustedOrigin.x, adjustedOrigin.y, adjustedOrigin.z),
                JPH::Vec3(rotatedDirection.x, rotatedDirection.y, rotatedDirection.z) * maxDistance
            );

            // Create a custom collector for this ray
            CustomRayCastCollector collector(entity);
            const JPH::NarrowPhaseQuery& npQuery = mPhysicsSystem->GetNarrowPhaseQueryNoLock();
            npQuery.CastRay(ray, JPH::RayCastSettings(), collector);

            // If a hit is found (and ignore self-hits), compute the hit point:
            if (collector.hitEntity != invalid_entity && collector.hitEntity != entity)
            {
                detectedEntities.push_back(collector.hitEntity);
                glm::vec3 hitPoint = adjustedOrigin + rotatedDirection * collector.closestFraction * maxDistance;

                // **Fetch entity name (if exists)**
                std::string entityName = "Unknown";
                if (g_Coordinator.HaveComponent<MetadataComponent>(collector.hitEntity)) {
                    entityName = g_Coordinator.GetComponent<MetadataComponent>(collector.hitEntity).GetName();
                }

                // Debug log for hit entity
                std::cout << "[PhysicsSystem] Cone Ray HIT entity: " << collector.hitEntity
                    << " (" << entityName << ") at fraction: " << collector.closestFraction << std::endl;

                // Draw a green debug line from the origin to the hit point
                if (RayCastDebug == true)
                    GraphicsSystem::AddDebugLine(adjustedOrigin, hitPoint, glm::vec3(0.0f, 1.0f, 0.0f));
            }
            else
            {
                // No hit: draw a red debug line showing the full ray length
                if (RayCastDebug == true)
                    GraphicsSystem::AddDebugLine(adjustedOrigin, adjustedOrigin + rotatedDirection * maxDistance, glm::vec3(1.0f, 0.0f, 0.0f));
            }

            // For debugging: print the full ray endpoints
            glm::vec3 fullEndPoint = adjustedOrigin + rotatedDirection * maxDistance;
        }
    }

    return detectedEntities;
}





#pragma warning(pop)