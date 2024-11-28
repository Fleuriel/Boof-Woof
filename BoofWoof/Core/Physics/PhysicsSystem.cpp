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

#include <glm/gtc/quaternion.hpp>   // For glm::quat
#include <glm/gtx/euler_angles.hpp> // For Euler angles
#include <cstdlib>
#include <cstdarg>
#include <cstdio>

std::unordered_map<Entity, float> m_PreviousYPositions;

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
    mPhysicsSystem->SetGravity(JPH::Vec3(0.0f, -9.81f, 0.0f));
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
                    }
                    else {
                        collisionComponent.SetIsColliding(false);
                    }

                    float currentYPosition = transform.GetPosition().y;

                    // Check if the Y-position has minimal change and the entity is colliding
                    bool isGrounded = false;
                    if (previousYPositions.find(entity) != previousYPositions.end()) {
                        float previousYPosition = previousYPositions[entity];
                        float yChange = std::abs(currentYPosition - previousYPosition);

                        //if (yChange < 0.01f && collisionComponent.GetIsColliding()) {
                        //    isGrounded = true;
                        //}
                        if (yChange < 0.01f) {
                            isGrounded = true;
                        }
                    }

                    // Check if still colliding with the floor
                    if (collisionComponent.GetLastCollidedObjectName() == "FloorCastle") {
                        isGrounded = true;
                    }

                    // Update `isGrounded` status
                    collisionComponent.SetIsGrounded(isGrounded);

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

//void MyPhysicsSystem::AddEntityBody(Entity entity, float mass) {
//    if (g_Coordinator.HaveComponent<TransformComponent>(entity) &&
//        g_Coordinator.HaveComponent<GraphicsComponent>(entity) &&
//        g_Coordinator.HaveComponent<CollisionComponent>(entity)) {
//
//        auto& transform = g_Coordinator.GetComponent<TransformComponent>(entity);
//        auto& graphicsComp = g_Coordinator.GetComponent<GraphicsComponent>(entity);
//        auto& collisionComponent = g_Coordinator.GetComponent<CollisionComponent>(entity);
//
//        // Get the texture name from GraphicsComponent to determine the object type
//        std::string modelName = graphicsComp.getModelName();
//
//        if (modelName.empty()) {
//            std::cerr << "AddEntityBody: Entity " << entity << " has an invalid or missing model." << std::endl;
//            return;
//        }
//
//        std::cout << "Model Name: " << modelName << std::endl;
//        ObjectType objectType = GetObjectTypeFromModel(modelName);
//
//        // Get the position, scale, and rotation from the TransformComponent
//        JPH::RVec3 position(transform.GetPosition().x, transform.GetPosition().y, transform.GetPosition().z);
//        glm::vec3 scale = transform.GetScale();
//        glm::quat rotation = transform.GetRotation();
// 
//        // Assign default AABB based on ObjectType
//        glm::vec3 defaultAABB;
//        switch (objectType) {
//        case ObjectType::Bed:
//            defaultAABB = glm::vec3(1.5f, 1.8f, 2.0f);
//            break;
//        case ObjectType::Chair:
//            defaultAABB = glm::vec3(0.5f, 0.8f, 0.5f);
//            break;
//        case ObjectType::Table:
//            defaultAABB = glm::vec3(2.0f, 0.4f, 2.0f);
//            break;
//        case ObjectType::Table2:
//            defaultAABB = glm::vec3(0.8f, 0.4f, 2.0f);
//            break;
//        case ObjectType::Couch:
//            defaultAABB = glm::vec3(1.5f, 0.6f, 1.0f);
//            break;
//        case ObjectType::Corgi:
//            defaultAABB = glm::vec3(2.8f, 0.3f, 1.3f);
//            break;
//        case ObjectType::FloorCastle:
//            defaultAABB = glm::vec3(5.0f, 0.15f, 4.2f);
//            break;
//        case ObjectType::Wall:
//            defaultAABB = glm::vec3(2.5f, 3.0f, 0.8f);
//            break;
//        case ObjectType::Wardrobe:
//            defaultAABB = glm::vec3(2.0f, 2.0f, 1.0f);
//            break;
//        default:
//            defaultAABB = glm::vec3(1.0f, 1.0f, 1.0f); // Default fallback
//            break;
//        }
//
//        // Set the default AABB size in the CollisionComponent (only if it hasn't been customized)
//        if (collisionComponent.GetAABBSize() == glm::vec3(1.0f, 1.0f, 1.0f)) {
//            collisionComponent.SetAABBSize(defaultAABB);
//        }
//
//        // Get the AABB size from the CollisionComponent
//        glm::vec3 customAABB = collisionComponent.GetAABBSize();
//
//        // Compute the scaled AABB
//        glm::vec3 scaledAABB = customAABB * scale;
//
//        // Include offset in body position
//        glm::vec3 offset = collisionComponent.GetAABBOffset();
//        JPH::RVec3 positionWithOffset = JPH::RVec3(
//            transform.GetPosition().x + offset.x,
//            transform.GetPosition().y + offset.y,
//            transform.GetPosition().z + offset.z
//        );
//
//        // Get the rotation from TransformComponent
//        glm::quat glmRotation = transform.GetRotation();
//        JPH::Quat joltRotation(glmRotation.x, glmRotation.y, glmRotation.z, glmRotation.w);
//
//        // Create shape based on object type and custom AABB
//        JPH::Shape* shape = CreateShapeForObjectType(objectType, scaledAABB);
//
//        // Set motion type based on IsDynamic in CollisionComponent
//        bool isDynamic = collisionComponent.IsDynamic();
//        JPH::EMotionType motionType = isDynamic ? JPH::EMotionType::Dynamic : JPH::EMotionType::Static;
//
//        // Define body creation settings
//        JPH::BodyCreationSettings bodySettings(
//            shape,
//            //position,
//            positionWithOffset,
//            //JPH::Quat(rotation.w, rotation.x, rotation.y, rotation.z), // Apply initial rotation
//            joltRotation,
//            motionType,
//            motionType == JPH::EMotionType::Dynamic ? Layers::MOVING : Layers::NON_MOVING // Layer based on motion type
//        );
//
//        JPH::BodyInterface& bodyInterface = mPhysicsSystem->GetBodyInterface();
//        if (&bodyInterface == nullptr) {
//            std::cerr << "BodyInterface is not available!" << std::endl;
//            return;
//        }
//
//        // Create and add the body to the physics system
//        JPH::Body* body = bodyInterface.CreateBody(bodySettings);
//        if (body == nullptr) {
//            std::cerr << "Failed to create a new body!" << std::endl;
//        }
//        else {
//            std::cout << "Body successfully created with ID: " << body->GetID().GetIndex() << std::endl;
//            bodyID = body->GetID();
//            bodyInterface.AddBody(body->GetID(), JPH::EActivation::Activate);
//
//            // Prevent dynamic bodies (like the player) from sleeping
//            if (isDynamic) {
//                body->SetAllowSleeping(false);
//            }
//
//            body->SetUserData(static_cast<uintptr_t>(entity)); // Store Entity ID directly as uintptr_t
//            // Assign the body to the CollisionComponent
//            collisionComponent.SetPhysicsBody(body);
//            bodyToEntityMap[body->GetID()] = entity;
//        }
//    }
//}

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
            float mass = collisionComponent.GetMass();
            bodySettings.mOverrideMassProperties = JPH::EOverrideMassProperties::MassAndInertiaProvided;
            bodySettings.mMassPropertiesOverride.SetMassAndInertiaOfSolidBox(
                JPH::Vec3(scaledAABB.x, scaledAABB.y, scaledAABB.z),
                mass // Default mass
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



#pragma warning(pop)