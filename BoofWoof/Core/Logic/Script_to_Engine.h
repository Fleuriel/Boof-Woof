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

#pragma warning(push)
#pragma warning(disable: 6385 6386)
#include <Jolt/Physics/Body/Body.h>

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
			if (body->GetMotionType() == JPH::EMotionType::Dynamic)
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
			if (body->GetMotionType() == JPH::EMotionType::Dynamic)
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
};

#endif // !SCRIPT_TO_ENGINE_H

#pragma warning(pop)
