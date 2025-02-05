#pragma once
#ifndef BEHAVIOURINTERFACE_H
#define BEHAVIOURINTERFACE_H

#ifndef GAME_ENGINE
using Entity = std::uint32_t;
#endif

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
};

struct physics_interface
{
	virtual void RemoveBody(Entity entity) = 0;
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
	
	virtual bool HaveCollisionComponent(Entity entity) = 0;
	virtual bool HavePhysicsBody(Entity entity) = 0;
	virtual glm::vec3 GetVelocity(Entity entity) = 0;
	virtual bool IsColliding(Entity entity) = 0;
	virtual const char* GetCollidingEntityName(Entity entity) = 0;
	virtual void SetVelocity(Entity entity, glm::vec3 velocity) = 0;
	virtual glm::vec3 GetSurfaceNormal(Entity entity) = 0;

	// Pathfinding functions
	virtual bool HavePathfindingComponent(Entity entity) = 0;
	virtual std::vector<glm::vec3> GetPath(Entity entity) = 0;

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
};

#ifdef GAME_ENGINE
using GetScripts_cpp_t = std::vector<std::unique_ptr<Behaviour_i>>* (*)(engine_interface* pEI);
#endif

#endif // !BEHAVIOURINTERFACE_H