#pragma once
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
	bool PlayerCollidedRope1{ false }, PlayerCollidedRope2{ false };
	int RopeCount{ 2 };

private:

	// Rope
	Entity player{}, rope1{}, rope2{};
	bool PlayerColliding{}, Rope1Colliding{}, Rope2Colliding{};
	int RopeDespawned{ 0 };

	// Drawbridge
	Entity bridge{};

	glm::vec3 TargetPos = glm::vec3(2.5f,-2.5f,0.0f);

	glm::vec3 initialPos{};
	glm::vec3 initialRotation{};


	float ElapsedTime = 0.0f;	// Time since animation started
	float FallDuration = 2.0f;	// Total duration of animation
	bool isFalling{ false };
};

extern RopeBreaker g_RopeBreaker;