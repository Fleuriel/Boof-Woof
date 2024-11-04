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

    if (modelName.find("Table") != std::string::npos) {
        type = ObjectType::Table;
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
    else if (modelName.find("Corgi") != std::string::npos) {
        type = ObjectType::Corgi;
    }
    else if (modelName.find("Floor") != std::string::npos) {
        type = ObjectType::Floor;
    }
    else if (modelName.find("Wall") != std::string::npos) {
        type = ObjectType::Wall;
    }
    else if (modelName.find("Wardrobe") != std::string::npos) {
        type = ObjectType::Wardrobe;
    }

    // Debug statement to check which ObjectType is set
    std::cout << "Model Name: " << modelName << " | Set ObjectType: ";
    switch (type) {
    case ObjectType::Table:
        std::cout << "Table";
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
        std::cout << "Corgi";
        break;
    case ObjectType::Floor:
        std::cout << "Floor";
        break;
    case ObjectType::Wall:
        std::cout << "Wall";
        break;
    case ObjectType::Wardrobe:
        std::cout << "Wardrobe";
        break;
    default:
        std::cout << "Default";
        break;
    }
    std::cout << std::endl;

    return type;
}

// Define different shapes based on ObjectType
JPH::Shape* MyPhysicsSystem::CreateShapeForObjectType(ObjectType type, const glm::vec3& scale) {
    switch (type) {
    case ObjectType::Bed:
        return new JPH::BoxShape(JPH::Vec3(scale.x * 1.5f, scale.y * 0.5f, scale.z * 2.0f)); // Dimensions for bed

    case ObjectType::Chair:
        return new JPH::BoxShape(JPH::Vec3(scale.x * 0.5f, scale.y * 0.8f, scale.z * 0.5f)); // Dimensions for chair

    case ObjectType::Table:
        return new JPH::BoxShape(JPH::Vec3(scale.x * 1.0f, scale.y * 0.4f, scale.z * 1.0f)); // Dimensions for table

    case ObjectType::Couch:
        return new JPH::BoxShape(JPH::Vec3(scale.x * 1.5f, scale.y * 0.6f, scale.z * 1.0f)); // Dimensions for couch

    case ObjectType::Corgi:
        return new JPH::BoxShape(JPH::Vec3(scale.x * 2.0f, scale.y * 0.5f, scale.z * 0.6f)); // Dimensions for corgi

    case ObjectType::Floor:
        return new JPH::BoxShape(JPH::Vec3(scale.x * 5.0f, scale.y * 0.1f, scale.z * 5.0f)); // Large, flat shape for the floor

    case ObjectType::Wall:
        return new JPH::BoxShape(JPH::Vec3(scale.x * 0.1f, scale.y * 2.5f, scale.z * 5.0f)); // Thin, tall shape for walls

    case ObjectType::Wardrobe:
        return new JPH::BoxShape(JPH::Vec3(scale.x * 1.0f, scale.y * 2.0f, scale.z * 0.8f)); // Tall and narrow for wardrobe

    default:
        return new JPH::BoxShape(JPH::Vec3(scale.x * 0.5f, scale.y * 0.5f, scale.z * 0.5f)); // Default dimensions
    }
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
    mContactListener = new MyContactListener();  // Instantiate the listener
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
    //mPhysicsSystem->SetGravity(JPH::Vec3(0.0f, -9.81f, 0.0f));
    mPhysicsSystem->SetGravity(JPH::Vec3(0.0f, 0.0f, 0.0f));

    std::cout << "Jolt physics system created" << std::endl;
    return mPhysicsSystem;
}

//void MyPhysicsSystem::OnUpdate(float deltaTime) {
//    ++_step;
//
//    JPH::BodyInterface& bodyInterface = mPhysicsSystem->GetBodyInterface();
//
//    // Iterate through all entities that match the PhysicsSystem signature
//    auto allEntities = g_Coordinator.GetAliveEntitiesSet();
//
//    for (auto& entity : allEntities) {
//        if (g_Coordinator.HaveComponent<TransformComponent>(entity) && g_Coordinator.HaveComponent<CollisionComponent>(entity)) {
//            auto& collisionComponent = g_Coordinator.GetComponent<CollisionComponent>(entity);
//            auto& transform = g_Coordinator.GetComponent<TransformComponent>(entity);
//
//            JPH::RVec3 position(transform.GetPosition().x, transform.GetPosition().y, transform.GetPosition().z);
//            glm::vec3 scale = transform.GetScale();
//
//            // Debug output to check Position
//            std::cout << "Entity ID: " << entity << " Position = (" << transform.GetPosition().x << ", " << transform.GetPosition().y << ", " << transform.GetPosition().z << ")" << std::endl;
//
//            // Only add a new body if it hasn't already been added
//            if (!collisionComponent.HasBodyAdded()) {
//                AddEntityBody(entity);
//                collisionComponent.SetHasBodyAdded(true); // Mark as added
//            }
//
//            JPH::Vec3 velocity = bodyInterface.GetLinearVelocity(bodyID);
//
//            // Debug output to check Position
//            std::cout << "Entity ID: " << entity << " Jolt x: " << position.GetX() << " Jolt y: " << position.GetY() << " Jolt z: " << position.GetZ() << std::endl;
//            std::cout << "Step " << _step << ": Position = (" << position.GetX() << ", " << position.GetY() << ", " << position.GetZ() 
//                << "), Velocity = (" << velocity.GetX() << ", " << velocity.GetY() << ", " << velocity.GetZ() << ")" << std::endl;
//
//            // Simulate physics
//            //mPhysicsSystem->Update(deltaTime, 1, mTempAllocator, mJobSystem);
//
//        }
//    }
//
//    // Simulate physics
//    mPhysicsSystem->Update(deltaTime, 1, mTempAllocator, mJobSystem);
//
//    // Update the entities' transforms after simulation
//    UpdateEntityTransforms();
//}

void MyPhysicsSystem::OnUpdate(float deltaTime) {
    ++_step;

    JPH::BodyInterface& bodyInterface = mPhysicsSystem->GetBodyInterface();

    // Iterate through all entities that match the PhysicsSystem signature
    auto allEntities = g_Coordinator.GetAliveEntitiesSet();

    for (auto& entity : allEntities) {
        if (g_Coordinator.HaveComponent<TransformComponent>(entity) && g_Coordinator.HaveComponent<CollisionComponent>(entity) 
            && g_Coordinator.HaveComponent<GraphicsComponent>(entity)) {
            auto& collisionComponent = g_Coordinator.GetComponent<CollisionComponent>(entity);
            auto& transform = g_Coordinator.GetComponent<TransformComponent>(entity);

            JPH::RVec3 position(transform.GetPosition().x, transform.GetPosition().y, transform.GetPosition().z);
            glm::vec3 scale = transform.GetScale();

            // Only add a new body if it hasn't already been added
            if (!collisionComponent.HasBodyAdded()) {
                AddEntityBody(entity);
                collisionComponent.SetHasBodyAdded(true); // Mark as added
            }

            // Retrieve the physics body
            JPH::Body* body = collisionComponent.GetPhysicsBody();
            if (body != nullptr && !body->GetID().IsInvalid()) {
                // Get the new rotation from the TransformComponent
                glm::quat newRotation = transform.GetRotation();

                // Convert the current Jolt rotation to glm for comparison
                JPH::Quat currentPhysicsRotation = body->GetRotation();
                glm::quat currentRotation(
                    currentPhysicsRotation.GetW(),
                    currentPhysicsRotation.GetX(),
                    currentPhysicsRotation.GetY(),
                    currentPhysicsRotation.GetZ()
                );

                // Check if the rotation has changed
                if (currentRotation != newRotation) {
                    // Update the physics body with the new rotation
                    JPH::Quat joltRotation(newRotation.w, newRotation.x, newRotation.y, newRotation.z);
                    bodyInterface.SetRotation(body->GetID(), joltRotation, JPH::EActivation::Activate);

                    // Debug output to verify rotation change
                    glm::vec3 eulerRotation = glm::eulerAngles(newRotation);
                    eulerRotation = glm::degrees(eulerRotation);
                    std::cout << "Entity ID: " << entity
                        << " Rotation Updated to (" << eulerRotation.x << "°, "
                        << eulerRotation.y << "°, "
                        << eulerRotation.z << "°)" << std::endl;
                }

                // Debug output to check Position and Velocity for Dynamic Bodies
                if (body->GetMotionType() == JPH::EMotionType::Dynamic) {
                    JPH::Vec3 velocity = bodyInterface.GetLinearVelocity(body->GetID());
                    std::cout << "Entity ID: " << entity << " Position = ("
                        << position.GetX() << ", "
                        << position.GetY() << ", "
                        << position.GetZ() << "), Velocity = ("
                        << velocity.GetX() << ", "
                        << velocity.GetY() << ", "
                        << velocity.GetZ() << ")" << std::endl;
                }
                else 
                {
                    std::cout << "Entity ID: " << entity << " Position = ("
                        << position.GetX() << ", "
                        << position.GetY() << ", "
                        << position.GetZ() << ")" << std::endl;

                }
            }
        }
    }

    // Simulate physics
    mPhysicsSystem->Update(deltaTime, 1, mTempAllocator, mJobSystem);

    // Update the entities' transforms after simulation
    UpdateEntityTransforms();
}

void MyPhysicsSystem::AddEntityBody(Entity entity) {
    if (g_Coordinator.HaveComponent<TransformComponent>(entity)) {
        auto& transform = g_Coordinator.GetComponent<TransformComponent>(entity);
        auto& graphicsComp = g_Coordinator.GetComponent<GraphicsComponent>(entity);
        auto& collisionComponent = g_Coordinator.GetComponent<CollisionComponent>(entity);

        // Get the texture name from GraphicsComponent to determine the object type
        std::string modelName = graphicsComp.getModelName();
        std::cout << "Model Name: " << modelName << std::endl;
        ObjectType objectType = GetObjectTypeFromModel(modelName);

        // Use GetName() to check if this entity is the player
        std::string name = g_Coordinator.GetComponent<MetadataComponent>(entity).GetName();
        bool isPlayer = (name == "Player");

        JPH::RVec3 position(transform.GetPosition().x, transform.GetPosition().y, transform.GetPosition().z);
        glm::vec3 scale = transform.GetScale();
        glm::quat rotation = transform.GetRotation();

        // Create shape based on object type
        JPH::Shape* shape = CreateShapeForObjectType(objectType, scale);

        // Set motion type based on whether the entity is the player
        JPH::EMotionType motionType = isPlayer ? JPH::EMotionType::Dynamic : JPH::EMotionType::Static;

        // Define body creation settings
        JPH::BodyCreationSettings bodySettings(
            //boxShape,
            shape,
            position,
            JPH::Quat(rotation.w, rotation.x, rotation.y, rotation.z),  // Apply initial rotation
            motionType, // Set Motion Type for entity
            motionType == JPH::EMotionType::Dynamic ? Layers::MOVING : Layers::NON_MOVING  // Layer based on motion type
        );

        JPH::BodyInterface& bodyInterface = mPhysicsSystem->GetBodyInterface();
        if (&bodyInterface == nullptr) {
            std::cerr << "BodyInterface is not available!" << std::endl;
            return;
        }

        // Create and add the body to the physics system
        JPH::Body* body = bodyInterface.CreateBody(bodySettings);
        if (body == nullptr) {
            std::cerr << "Failed to create a new body!" << std::endl;
        }
        else {
            std::cout << "Body successfully created with ID: " << body->GetID().GetIndex() << std::endl;
            bodyID = body->GetID();
            bodyInterface.AddBody(body->GetID(), JPH::EActivation::Activate);

            // Prevent player body from sleeping
            if (isPlayer) {
                body->SetAllowSleeping(false);
            }

            // Assign the body to the CollisionComponent
            collisionComponent.SetPhysicsBody(body);
        }
    }
}


void MyPhysicsSystem::UpdateEntityTransforms() {
    auto allEntities = g_Coordinator.GetAliveEntitiesSet();
    for (auto& entity : allEntities) {
        if (g_Coordinator.HaveComponent<CollisionComponent>(entity)) {
            auto& collisionComp = g_Coordinator.GetComponent<CollisionComponent>(entity);
            JPH::Body* body = collisionComp.GetPhysicsBody();

            if (body != nullptr && !body->GetID().IsInvalid()) {
                // Physics position
                JPH::Vec3 updatedPosition = body->GetPosition();
                //std::cout << "Physics Position for Entity " << entity << ": ("
                //    << updatedPosition.GetX() << ", " << updatedPosition.GetY()
                //    << ", " << updatedPosition.GetZ() << ")" << std::endl;

                // Transform component position
                auto& transform = g_Coordinator.GetComponent<TransformComponent>(entity);
                glm::vec3 enginePosition = transform.GetPosition();
                //std::cout << "Engine Position for Entity " << entity << ": ("
                //    << enginePosition.x << ", " << enginePosition.y << ", "
                //    << enginePosition.z << ")" << std::endl;

                // Update the engine transform to match Jolt
                transform.SetPosition(glm::vec3(updatedPosition.GetX(), updatedPosition.GetY(), updatedPosition.GetZ()));
            }
        }
    }
}


//void MyPhysicsSystem::UpdateEntityTransforms() {
//    auto allEntities = g_Coordinator.GetAliveEntitiesSet();
//    for (auto& entity : allEntities) {
//        if (g_Coordinator.HaveComponent<CollisionComponent>(entity)) {
//            // std::cout << "is inside updateEntityTransform" << std::endl;
//            auto& collisionComp = g_Coordinator.GetComponent<CollisionComponent>(entity);
//            JPH::Body* body = collisionComp.GetPhysicsBody();
//
//            // Ensure the body ID is valid before accessing it
//            if (body != nullptr && !body->GetID().IsInvalid()) {
//                // Debug statement to print the updated body position
//                std::cout << "Updating transform for entity: " << entity << std::endl;
//
//                // Get the updated position and rotation from the JoltPhysics body
//                JPH::Vec3 updatedPosition = body->GetPosition();
//                JPH::Quat updatedRotation = body->GetRotation();
//
//                // Update the entity's TransformComponent
//                auto& transform = g_Coordinator.GetComponent<TransformComponent>(entity);
//                transform.SetPosition(glm::vec3(updatedPosition.GetX(), updatedPosition.GetY(), updatedPosition.GetZ()));
//
//                glm::quat newRotation(updatedRotation.GetW(), updatedRotation.GetX(), updatedRotation.GetY(), updatedRotation.GetZ());
//                glm::vec3 eulerRotation = glm::eulerAngles(newRotation);  // Convert quaternion to Euler angles
//                transform.SetRotation(eulerRotation);
//
//                // Debug output of the updated position
//                std::cout << "Updated entity position: (" << updatedPosition.GetX() << ", "
//                    << updatedPosition.GetY() << ", " << updatedPosition.GetZ() << ")"
//                    << "\nUpdated entity rotation: (" << eulerRotation.x << ", "
//                    << eulerRotation.y << ", " << eulerRotation.z << ")" << std::endl;
//            }
//        }
//    }
//}

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