#pragma once
#pragma warning(disable : 4005)
#include <EngineCore.h>
#include "../BoneCatcher/BoneCatcher.h"

class CageBreaker
{
public:

	// Default constructor
	CageBreaker() : player(Entity()), cage1(Entity()), cage2(Entity()), cage3(Entity()), cage1Collider(Entity()), cage2Collider(Entity()), cage3Collider(Entity()) {}
	CageBreaker(Entity playerEntity, Entity cageOne, Entity cageTwo, Entity cageThree, Entity cageOneCollider, Entity cageTwoCollider, Entity cageThreeCollider)
		: player(playerEntity), cage1(cageOne), cage2(cageTwo), cage3(cageThree),
		cage1Collider(cageOneCollider), cage2Collider(cageTwoCollider), cage3Collider(cageThreeCollider), CollidedCage1(false), CollidedCage2(false), CollidedCage3(false) {}

	void OnUpdate(double deltaTime);

	void CheckCageCollision();
	void SpawnBoneCatcher();
	void DespawnCage();
	bool CheckEntityWithPlayerCollision(Entity entity) const;
	void ResetCB();

public:
	bool CollidedCage1{ false }, CollidedCage2{ false }, CollidedCage3{ false };
	bool BarSpawned{ false };

private:

	// Rope
	Entity player{}, cage1{}, cage2{}, cage3{}, cage1Collider{}, cage2Collider{}, cage3Collider{};
	bool Cage1Colliding{}, Cage2Colliding{}, Cage3Colliding{}, firstCageTouched{ false }, secondCageTouched{ false }, thirdCageTouched{ false };
	bool deletedCage1{ false }, deletedCage2{ false }, deletedCage3{ false };
	bool playedCageSound1 = false;
	bool playedCageSound2 = false;
	bool playedCageSound3 = false;
};

extern CageBreaker g_CageBreaker;