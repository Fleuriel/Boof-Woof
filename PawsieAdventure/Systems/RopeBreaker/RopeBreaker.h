#pragma once
#pragma warning(disable : 4005)
#include <EngineCore.h>
#include "../BoneCatcher/BoneCatcher.h"

class RopeBreaker 
{
public:

	// Default constructor
	RopeBreaker() : player(Entity()), rope1(Entity()), rope2(Entity()), bridge(Entity()), PlayerCollidedRope1(false), PlayerCollidedRope2(false), RopeCount(2) {}
	RopeBreaker(Entity playerEntity, Entity ropeEntity1, Entity ropeEntity2, Entity bridgeEntity) 
		: player(playerEntity), rope1(ropeEntity1), rope2(ropeEntity2), bridge(bridgeEntity), PlayerCollidedRope1(false), PlayerCollidedRope2(false), RopeCount(2) {}

	void OnUpdate(double deltaTime);

	void CheckCollision();

	void SpawnBoneCatcher();
	void DespawnRope();

	// Drawbridge
	void DropBridge();

public:
	bool PlayerCollidedRope1{ false }, PlayerCollidedRope2{ false }, BoneSpawned{ false };
	int RopeCount{ 2 };

private:

	// Rope
	Entity player{}, rope1{}, rope2{};
	bool PlayerColliding{}, Rope1Colliding{}, Rope2Colliding{};
	int RopeDespawned{ 0 };

	// Drawbridge
	Entity bridge{};

	glm::vec3 TargetPos = glm::vec3(0.0f,-2.3f, -2.6f);

	glm::vec3 initialPos{};
	glm::vec3 initialRotation{};

	bool bridgeAudio{ false }, deletedRope1{ false }, deletedRope2{ false };

	float ElapsedTime = 0.0f;	// Time since animation started
	float FallDuration = 2.0f;	// Total duration of animation
	bool isFalling{ false };
};

extern RopeBreaker g_RopeBreaker;