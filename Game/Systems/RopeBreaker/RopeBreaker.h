#pragma once
#include <EngineCore.h>
#include "../BoneCatcher/BoneCatcher.h"

class RopeBreaker 
{
public:
	RopeBreaker(Entity playerEntity, Entity ropeEntity) : player(playerEntity), rope(ropeEntity), PlayerCollidedRope(false) {}

	void OnUpdate(double deltaTime);

	void CheckCollision();
	void SpawnBoneCatcher();

private:
	Entity player{}, rope{};
	bool PlayerCollidedRope{ false };
	bool PlayerColliding{}, RopeColliding{};
};