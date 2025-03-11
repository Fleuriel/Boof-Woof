#pragma once
#ifndef SCRIPT_TO_ENGINE_H
#define SCRIPT_TO_ENGINE_H

#include "../ECS/Coordinator.hpp"
#include "BehaviourInterface.h"
#include "../Input/Input.h"
#include "../Utilities/Components/CollisionComponent.hpp"
#include "../Utilities/Components/TransformComponent.hpp"
#include "../Utilities/Components/CameraComponent.hpp"
#include "../Core/EngineCore.h"
#include "../Core/AssetManager/FilePaths.h"
#include "../GSM/GameStateMachine.h"
#include "../Utilities/ForGame/TimerTR/TimerTR.h"
#include "../Utilities/ForGame/UI/UI.h"
#include "../Utilities/ForGame/Dialogue/Dialogue.h"

#pragma warning(push)
#pragma warning(disable: 6385 6386)
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Collision/RayCast.h>  // For JPH::RRayCast, JPH::RayCastResult
#include <Jolt/Physics/Collision/NarrowPhaseQuery.h>  // For JPH::NarrowPhaseQuery
#include <Jolt/Physics/Collision/CastResult.h>

class Script_to_Engine : public engine_interface, public input_interface, public audio_interface, public physics_interface
{
public:

	// INPUT INTERFACE
	virtual input_interface& getInputSystem() override
	{
		return *this;
	}

	virtual bool isButtonPressed(std::uint32_t Key) override
	{
		return g_Input.GetKeyState(Key);
	}

	virtual bool isActionPressed(const char * action) override
	{
		
		return g_Input.IsActionPressed(action);
	}

	// END OF INPUT INTERFACE
	
	// AUDIO INTERFACE
	virtual audio_interface& getAudioSystem() override
	{
		return *this;
	}

	virtual void PlaySound(const char* pSoundName) override
	{
		//UNREFERENCED_PARAMETER(pSoundName);
		// Play sound
		 g_Audio.PlayFile(pSoundName);
	}

	virtual void PlaySoundById(const char* soundId) override
	{
		std::string fullPath = std::string(FILEPATH_ASSET_AUDIO) + "/" + soundId;
		g_Audio.PlayFile(fullPath.c_str());
	}

	virtual void PlaySoundByFile(const char* soundId, bool loop = false, const std::string& soundType = "SFX") override
	{
		std::string fullPath = std::string(FILEPATH_ASSET_AUDIO) + "/" + soundId;
		g_Audio.PlayFileOnNewChannel(fullPath, loop, soundType);
	}



	// END OF AUDIO INTERFACE

	// PHYSICS INTERFACE
	virtual physics_interface& getPhysicsSystem() override
	{
		return *this;
	}

	virtual void RemoveBody(Entity entity) override
	{
		g_Coordinator.GetSystem<MyPhysicsSystem>()->RemoveEntityBody(entity);
	}

	// ENGINE INTERFACE

	virtual void DestroyEntity(Entity entity) override
	{
		g_Coordinator.DestroyEntity(entity);
	}
	
	//Transform Component
	virtual glm::vec3 GetPosition(Entity entity) override
	{
		return g_Coordinator.GetComponent<TransformComponent>(entity).GetPosition();
	}

	virtual glm::vec3 GetRotation(Entity entity) override
	{
		return g_Coordinator.GetComponent<TransformComponent>(entity).GetRotation();
	}

	virtual void SetPosition(Entity entity, glm::vec3 position) override
	{
		g_Coordinator.GetComponent<TransformComponent>(entity).SetPosition(position);
	}

	virtual void SetRotation(Entity entity, glm::vec3 rotation) override
	{
		g_Coordinator.GetComponent<TransformComponent>(entity).SetRotation(rotation);
	}

	virtual void SetRotationYawFromVelocity(Entity entity, glm::vec3 velocity) override
	{
		g_Coordinator.GetComponent<TransformComponent>(entity).SetRotationYawFromVelocity(velocity);
	}

	virtual bool HaveTransformComponent(Entity entity) override
	{
		return g_Coordinator.HaveComponent<TransformComponent>(entity);
	}


	//Collision Component
	virtual bool HaveCollisionComponent(Entity entity) override
	{
		return g_Coordinator.HaveComponent<CollisionComponent>(entity);
	}

	virtual bool HavePhysicsBody(Entity entity) override
	{
		return g_Coordinator.GetComponent<CollisionComponent>(entity).GetPhysicsBody() != nullptr;
	}

	
	virtual glm::vec3 GetVelocity(Entity entity) override
	{
		if (HaveCollisionComponent(entity) && HavePhysicsBody(entity))
		{
			auto* body = g_Coordinator.GetComponent<CollisionComponent>(entity).GetPhysicsBody();
			if (body->GetMotionType() == JPH::EMotionType::Dynamic || body->GetMotionType() == JPH::EMotionType::Kinematic)
			{
				JPH::Vec3 velocity = body->GetLinearVelocity();
				return glm::vec3(velocity.GetX(), velocity.GetY(), velocity.GetZ());
			}
		}
		return glm::vec3(0.0f); // Return zero velocity for static or invalid entities
	}

	virtual bool IsColliding(Entity entity) override
	{
		return g_Coordinator.GetComponent<CollisionComponent>(entity).GetIsColliding();
	}

	virtual const char* GetCollidingEntityName(Entity entity) override
	{
		return g_Coordinator.GetComponent<CollisionComponent>(entity).GetLastCollidedObjectName().c_str();
	}

	virtual void SetCollidingEntityName(Entity entity) override
	{
		g_Coordinator.GetComponent<CollisionComponent>(entity).SetLastCollidedObjectName("Floor");
	}

	virtual void SetVelocity(Entity entity, glm::vec3 inputVelocity) override
	{
		if (HaveCollisionComponent(entity) && HavePhysicsBody(entity))
		{
			auto* body = g_Coordinator.GetComponent<CollisionComponent>(entity).GetPhysicsBody();
			if (body->GetMotionType() == JPH::EMotionType::Dynamic || body->GetMotionType() == JPH::EMotionType::Kinematic)
			{
				JPH::Vec3 currentVelocity = body->GetLinearVelocity();

				// Combine gravity with player input velocity
				JPH::Vec3 newVelocity(inputVelocity.x, inputVelocity.y, inputVelocity.z);

				body->SetLinearVelocity(newVelocity);

				// std::cout << "Set Velocity: (" << newVelocity.GetX() << ", " << newVelocity.GetY() << ", " << newVelocity.GetZ() << ")" << std::endl;
			}
		}
		else
		{
			std::cerr << "Entity " << entity << " does not have a valid CollisionComponent or PhysicsBody. Skipping SetVelocity." << std::endl;
		}
	}

	//virtual Entity Raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance) override
	//{
	//	return g_Coordinator.GetSystem<MyPhysicsSystem>()->Raycast(origin, direction, maxDistance);
	//}

	virtual Entity Raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance, Entity ignoreEntity = INVALID_ENTITY) override
	{
		if (!g_Coordinator.GetSystem<MyPhysicsSystem>()) {
			std::cerr << "[Script_to_Engine] ERROR: Physics system is not initialized!" << std::endl;
			return INVALID_ENTITY;
		}

		Entity hitEntity = g_Coordinator.GetSystem<MyPhysicsSystem>()->Raycast(origin, direction, maxDistance, ignoreEntity);

		// Ignore the calling entity itself
		if (hitEntity == ignoreEntity) {
			return INVALID_ENTITY;
		}

		return hitEntity;
	}

	virtual std::vector<Entity> ConeRaycast(
		Entity entity,
		const glm::vec3& forwardDirection,
		float maxDistance,
		int numHorizontalRays, int numVerticalRays,
		float horizontalFOVAngle, float verticalFOVAngle,  // User-defined angles
		const glm::vec3& userOffset) override
	{
		return g_Coordinator.GetSystem<MyPhysicsSystem>()->ConeRaycast(entity, forwardDirection, maxDistance, numHorizontalRays, numVerticalRays, horizontalFOVAngle, verticalFOVAngle, userOffset);
	}

	virtual std::vector<Entity> ConeRaycastDownward(
		Entity entity,
		const glm::vec3& direction, float maxDistance,
		int numHorizontalRays, int numVerticalRays, float coneAngle, const glm::vec3& userOffset) override
	{
		return g_Coordinator.GetSystem<MyPhysicsSystem>()->ConeRaycastDownward(entity, direction, maxDistance, numHorizontalRays, numVerticalRays, coneAngle, userOffset);
	}

	virtual float RaycastFraction(const glm::vec3& origin, const glm::vec3& direction, float maxDistance, Entity ignoreEntity) {
		return g_Coordinator.GetSystem<MyPhysicsSystem>()->RaycastFraction(origin, direction, maxDistance, ignoreEntity);
	}
	virtual bool IsDynamic(Entity entity) override
	{
		if (HaveCollisionComponent(entity) && HavePhysicsBody(entity))
		{
			return g_Coordinator.GetComponent<CollisionComponent>(entity).IsDynamic();
		}
		return false;
	}

	virtual void SetIsDynamic(Entity entity, bool isDynamic) override
	{
		if (HaveCollisionComponent(entity) && HavePhysicsBody(entity))
		{
			return g_Coordinator.GetComponent<CollisionComponent>(entity).SetIsDynamic(isDynamic);
		}
	}

	virtual bool IsKinematic(Entity entity) override
	{
		if (HaveCollisionComponent(entity) && HavePhysicsBody(entity))
		{
			return g_Coordinator.GetComponent<CollisionComponent>(entity).IsKinematic();
		}
		return false;
	}

	virtual void SetIsKinematic(Entity entity, bool isKinematic) override
	{
		if (HaveCollisionComponent(entity) && HavePhysicsBody(entity))
		{
			return g_Coordinator.GetComponent<CollisionComponent>(entity).SetIsKinematic(isKinematic);
		}
	}

	virtual void DisablePhysics(Entity entity) override {
		g_Coordinator.GetSystem<MyPhysicsSystem>()->DisablePhysics(entity);
	}

	virtual void EnablePhysics(Entity entity) override {
		g_Coordinator.GetSystem<MyPhysicsSystem>()->EnablePhysics(entity);
	}

	virtual void UpdatePhysicsTransform(Entity entity) override {
		g_Coordinator.GetSystem<MyPhysicsSystem>()->UpdatePhysicsTransform(entity);
	}


	// Grounded functions
	virtual bool IsGrounded(Entity entity) override
	{
		if (HaveCollisionComponent(entity)) {
			return g_Coordinator.GetComponent<CollisionComponent>(entity).GetIsGrounded();
		}
		return false;
	}

	virtual void SetGrounded(Entity entity, bool grounded) override
	{
		if (HaveCollisionComponent(entity)) {
			g_Coordinator.GetComponent<CollisionComponent>(entity).SetIsGrounded(grounded);
		}
	}

	virtual double GetDeltaTime() override
	{
		return g_Core ? g_Core->m_DeltaTime : 0.0;
	}

	//Camera Component
	virtual bool HaveCameraComponent(Entity entity) override
	{
		return g_Coordinator.HaveComponent<CameraComponent>(entity);
	}

	virtual glm::vec3 GetCameraDirection(Entity entity) override
	{
		return g_Coordinator.GetComponent<CameraComponent>(entity).GetCameraDirection();
	}

	// not used
	virtual float GetCameraYaw(Entity entity) override
	{
		return g_Coordinator.GetComponent<CameraComponent>(entity).GetCameraYaw();
	}

	// not used
	virtual float GetCameraPitch(Entity entity) override
	{
		UNREFERENCED_PARAMETER(entity);
		return g_Coordinator.GetComponent<CameraComponent>(g_Player).GetCameraPitch();
	}

	virtual glm::vec3 GetCameraUp(Entity entity) override
	{
		return g_Coordinator.GetComponent<CameraComponent>(entity).GetCameraUp();
	}

	// Pathfinding Component Functions
	virtual bool HavePathfindingComponent(Entity entity) override {
		bool hasComponent = g_Coordinator.HaveComponent<PathfindingComponent>(entity);
		//std::cout << "[Engine] Checking PathfindingComponent for Entity " << entity << ": "
		//	<< (hasComponent ? "Exists" : "Does Not Exist") << std::endl;
		return hasComponent;
	}

	virtual std::vector<glm::vec3> GetPath(Entity entity) override {
		if (HavePathfindingComponent(entity)) {
			auto& path = g_Coordinator.GetComponent<PathfindingComponent>(entity).GetPath();
			std::cout << "[Engine] Retrieved path of length " << path.size() << " for Entity " << entity << std::endl;
			return path;
		}
		std::cout << "[Engine] No path found for Entity " << entity << std::endl;
		return {};
	}

	virtual void SetStartNode(Entity entity, Entity node) override {
		if (HavePathfindingComponent(entity)) {
			g_Coordinator.GetComponent<PathfindingComponent>(entity).SetStartNode(node);
			std::cout << "[Engine] Set start node for Entity " << entity << " to " << node << std::endl;
		}
	}

	virtual Entity GetStartNode(Entity entity) override {
		if (HavePathfindingComponent(entity)) {
			Entity startNode = g_Coordinator.GetComponent<PathfindingComponent>(entity).GetStartNode();
			//std::cout << "[Engine] Retrieved start node for Entity " << entity << ": " << startNode << std::endl;
			return startNode;
		}
		std::cout << "[Engine] No start node found for Entity " << entity << std::endl;
		return 0;
	}

	virtual void SetGoalNode(Entity entity, Entity node) override {
		if (HavePathfindingComponent(entity)) {
			g_Coordinator.GetComponent<PathfindingComponent>(entity).SetGoalNode(node);
			std::cout << "[Engine] Set goal node for Entity " << entity << " to " << node << std::endl;
		}
	}

	virtual Entity GetGoalNode(Entity entity) override {
		if (HavePathfindingComponent(entity)) {
			Entity goalNode = g_Coordinator.GetComponent<PathfindingComponent>(entity).GetGoalNode();
			//std::cout << "[Engine] Retrieved goal node for Entity " << entity << ": " << goalNode << std::endl;
			return goalNode;
		}
		std::cout << "[Engine] No goal node found for Entity " << entity << std::endl;
		return 0;
	}

	virtual void SetBuilt(Entity entity, bool built) override {
		g_Coordinator.GetComponent<PathfindingComponent>(entity).SetBuilt(built);
	}

	virtual Entity GetNearestNode(Entity entity) override {
		// Get Entity position
		glm::vec3 entityPosition = g_Coordinator.GetComponent<TransformComponent>(entity).GetPosition();

		return g_Coordinator.GetSystem<PathfindingSystem>()->GetClosestNode(entityPosition);
	}

	virtual Entity GetRandomNode(Entity entity) override {
		Entity start = g_Coordinator.GetComponent<PathfindingComponent>(entity).GetStartNode();

		std::vector<Entity> list = g_Coordinator.GetSystem<PathfindingSystem>()->GetNodeList();
		// Get a random node from the list
		std::random_device rd;   // Seed
		std::mt19937 gen(rd());  // Mersenne Twister random engine
		std::uniform_int_distribution<size_t> dist(0, list.size() - 1); // Uniform distribution

		if (gen.max() == 0)
			return list[0]; // Return the only element (if there is only one element in the list
		else if (list.size() == 0)
			return MAX_ENTITIES; // Return invalid entity if no nodes are found (should not happen

		std::cout << "[Engine] Random node selected: " << list[dist(gen)] << std::endl;
		
		// Pick a random element
		return list[dist(gen)];
	}

	virtual Entity GetPlayerEntity() override
	{
		for (auto entity : g_Coordinator.GetAliveEntitiesSet())
		{
			if (g_Coordinator.HaveComponent<MetadataComponent>(entity))
			{
				const auto& metadata = g_Coordinator.GetComponent<MetadataComponent>(entity);
				if (metadata.GetName() == "Player")
				{
					return entity;
				}
			}
		}
		return MAX_ENTITIES; // Return invalid entity if no player is found
	}

	


	virtual bool IsGamePaused() override
	{
		return g_IsPaused;
	}

	virtual double GetTimerTiming() override {
		return g_TimerTR.timer;
	}

	virtual double SetTimerTiming(double timing) override 
	{
		return g_TimerTR.timer = timing;
	}

	virtual void SetTouched(bool touch) override
	{
		g_TimerTR.touched = touch;
	}

	virtual bool GetExhausted() override 
	{
		return g_UI.isExhausted;
	}

	virtual bool GetStunned() override
	{
		return g_UI.isStunned;
	}

	virtual bool MatchEntityName(Entity entity, const char* entityName)
	{
		if (g_Coordinator.HaveComponent<MetadataComponent>(entity)) 
		{			
			if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == entityName)
			{
				return true;
			}
		}
		return false;
	}

	virtual void SetDialogue(int dialogueState)
	{
		g_DialogueText.OnInitialize();
		g_DialogueText.setDialogue(static_cast<DialogueState>(dialogueState));
	}

	virtual double GetTRtimer() override {
		return g_TimerTR.timer;
	}
};

#endif // !SCRIPT_TO_ENGINE_H

#pragma warning(pop)
