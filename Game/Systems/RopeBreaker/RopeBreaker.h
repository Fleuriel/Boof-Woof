#pragma once
#include <EngineCore.h>
#include "../BoneCatcher/BoneCatcher.h"

class RopeBreaker 
{
public:

	// Default constructor
	RopeBreaker() : player(Entity()), rope1(Entity()), rope2(Entity()), PlayerCollidedRope1(false), PlayerCollidedRope2(false), RopeCount(2) {}
	RopeBreaker(Entity playerEntity, Entity ropeEntity1, Entity ropeEntity2) 
		: player(playerEntity), rope1(ropeEntity1), rope2(ropeEntity2), PlayerCollidedRope1(false), PlayerCollidedRope2(false), RopeCount(2) {}

	void OnUpdate(double deltaTime);

	void CheckCollision();
	void SpawnBoneCatcher();
	void DespawnRope();

public:
	bool PlayerCollidedRope1{ false }, PlayerCollidedRope2{ false };
	int RopeCount{ 2 };

private:
	Entity player{}, rope1{}, rope2{};
	bool PlayerColliding{}, Rope1Colliding{}, Rope2Colliding{};
};

extern RopeBreaker g_RopeBreaker;