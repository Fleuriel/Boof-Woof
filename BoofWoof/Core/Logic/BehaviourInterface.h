#pragma once
#include <vector>
#include <string> 
#ifndef BEHAVIOURINTERFACE_H
#define BEHAVIOURINTERFACE_H

#ifndef GAME_ENGINE
using Entity = std::uint32_t;
#endif
constexpr Entity INVALID_ENT = std::numeric_limits<Entity>::max();


struct Behaviour_i
{
	virtual ~Behaviour_i() = default;
	virtual void Init(Entity entity) = 0;
	virtual void Update(Entity entity) = 0;
	virtual void Destroy(Entity entity) = 0;
	virtual const char* getBehaviourName() = 0;
};

struct input_interface
{
	virtual bool isButtonPressed(std::uint32_t Key) = 0;
	virtual bool isActionPressed(const char* pAction) = 0;
};

struct audio_interface
{
	virtual void PlaySound(const char* pSoundName) = 0;
	virtual void PlaySoundById(const char* soundId) = 0;

	virtual void PlaySoundByFile(const char* soundId, bool loop = false, const std::string& soundType = "SFX") = 0;

};

struct physics_interface
{
	virtual void RemoveBody(Entity entity) = 0;
	virtual Entity Raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance, Entity ignoreEntity = INVALID_ENT) = 0;
	virtual std::vector<Entity> ConeRaycast(
		Entity entity,
		const glm::vec3& forwardDirection,
		float maxDistance,
		int numHorizontalRays, int numVerticalRays,
		float horizontalFOVAngle, float verticalFOVAngle,  // User-defined angles
		const glm::vec3& userOffset) = 0;
	virtual std::vector<Entity> ConeRaycastDownward(
		Entity entity,
		const glm::vec3& direction, float maxDistance,
		int numHorizontalRays, int numVerticalRays, float coneAngle, const glm::vec3& userOffset) = 0;
};

struct engine_interface
{
	virtual ~engine_interface() = default;
	virtual input_interface& getInputSystem() = 0;
	virtual audio_interface& getAudioSystem() = 0;
	virtual physics_interface& getPhysicsSystem() = 0;

	virtual void DestroyEntity(Entity entity) = 0;

	virtual bool HaveTransformComponent(Entity entity) = 0;
	virtual glm::vec3 GetPosition(Entity entity) = 0;
	virtual glm::vec3 GetRotation(Entity entity) = 0;
	virtual void SetPosition(Entity entity, glm::vec3 position) = 0;
	virtual void SetRotation(Entity entity, glm::vec3 rotation) = 0;
	virtual void SetRotationYawFromVelocity(Entity entity, glm::vec3 velocity) = 0;
	
	virtual bool HaveCollisionComponent(Entity entity) = 0;
	virtual bool HavePhysicsBody(Entity entity) = 0;
	virtual glm::vec3 GetVelocity(Entity entity) = 0;
	virtual bool IsColliding(Entity entity) = 0;
	virtual const char* GetCollidingEntityName(Entity entity) = 0;
	virtual void SetCollidingEntityName(Entity entity) = 0;
	virtual void SetVelocity(Entity entity, glm::vec3 velocity) = 0;
	virtual bool IsDynamic(Entity entity) = 0;

	// Pathfinding functions
	virtual bool HavePathfindingComponent(Entity entity) = 0;
	virtual std::vector<glm::vec3> GetPath(Entity entity) = 0;
	virtual void SetStartNode(Entity entity, Entity node) = 0;
	virtual Entity GetStartNode(Entity entity) = 0;
	virtual void SetGoalNode(Entity entity, Entity node) = 0;
	virtual Entity GetGoalNode(Entity entity) = 0;
	virtual void SetBuilt(Entity entity, bool built) = 0;
	virtual Entity GetNearestNode(Entity entity) = 0;
	virtual Entity GetRandomNode(Entity entity) = 0;

	// Grounded functions
	virtual bool IsGrounded(Entity entity) = 0;
	virtual void SetGrounded(Entity entity, bool grounded) = 0;

	virtual bool HaveCameraComponent(Entity entity) = 0;
	virtual glm::vec3 GetCameraDirection(Entity entity) = 0;
	virtual float GetCameraYaw(Entity entity) = 0;
	virtual float GetCameraPitch(Entity entity) = 0;
	virtual glm::vec3 GetCameraUp(Entity entity) = 0;

	virtual double GetDeltaTime() = 0;
	virtual bool IsGamePaused() = 0;

	// Toys
	virtual double SetTimerTiming(double timing) = 0;
	virtual double GetTimerTiming() = 0;
	virtual void SetTouched(bool touch) = 0;

	virtual Entity GetPlayerEntity() = 0; // Expose the function
	virtual bool GetExhausted() = 0;

	virtual bool GetStunned() = 0;

	virtual bool MatchEntityName(Entity entity, const char* entityName) = 0;
	virtual void SetDialogue(int dialogueState) = 0;

	virtual double GetTRtimer() = 0;
};

#ifdef GAME_ENGINE
using GetScripts_cpp_t = std::vector<std::unique_ptr<Behaviour_i>>* (*)(engine_interface* pEI);
#endif

#endif // !BEHAVIOURINTERFACE_H