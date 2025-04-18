#pragma once
#pragma warning(disable : 4005)
#include <EngineCore.h>
#include "../BoneCatcher/BoneCatcher.h"

class CageBreaker
{
public:

	// Default constructor
	CageBreaker() : player(Entity()), cage1(Entity()), cage2(Entity()), cage3(Entity()), cage1Collider(Entity()), cage2Collider(Entity()), cage3Collider(Entity()),
		puppy1(Entity()), puppy2(Entity()), puppy3(Entity())
	{

	}

	CageBreaker(Entity playerEntity, Entity cageOne, Entity cageTwo, Entity cageThree, Entity cageOneCollider, Entity cageTwoCollider, Entity cageThreeCollider,
		Entity puppyFirst, Entity puppySecond, Entity puppyThird)
		: player(playerEntity), cage1(cageOne), cage2(cageTwo), cage3(cageThree), cage1Collider(cageOneCollider), cage2Collider(cageTwoCollider), cage3Collider(cageThreeCollider),
		puppy1(puppyFirst), puppy2(puppySecond), puppy3(puppyThird), CollidedCage1(false), CollidedCage2(false), CollidedCage3(false)
	{

	}

	void OnUpdate(double deltaTime);

	void CheckCageCollision();
	void SpawnBoneCatcher();
	void DespawnCage();

	void SaveProgress();

	bool CheckEntityWithPlayerCollision(Entity entity) const;
	void DisablePuppyCollision(Entity puppy);
	void EnablePuppyCollision(Entity puppy);
	void ResetCB();

public:
	bool Cage1Colliding{}, Cage2Colliding{}, Cage3Colliding{};
	bool BarSpawned{ false };
	std::unordered_map<int, int> CageHitCounts; // Map to store hit counts per cage
	std::unordered_map<int, float> speedCage;
	std::unordered_map<int, int> directionCage;
	Entity cage1{}, cage2{}, cage3{};

private:

	// Rope
	Entity player{}, cage1Collider{}, cage2Collider{}, cage3Collider{}, puppy1{}, puppy2{}, puppy3{};
	bool firstCageTouched{ false }, secondCageTouched{ false }, thirdCageTouched{ false };
	bool deletedCage1{ false }, deletedCage2{ false }, deletedCage3{ false };
	bool CollidedCage1{ false }, CollidedCage2{ false }, CollidedCage3{ false };

	bool playedCageSound1 = false;
	bool playedCageSound2 = false;
	bool playedCageSound3 = false;
};

extern CageBreaker g_CageBreaker;