#pragma once

#pragma warning(disable : 4005)
#include <EngineCore.h>

class SmellAvoidance
{
public:
	SmellAvoidance() = default;
	SmellAvoidance(Entity playerEntity, Entity testPee, Entity testCollider, Entity pee1, Entity pee2, Entity pee3, Entity pee4, Entity pee5, Entity pee6,
        Entity pee1Collider, Entity pee2Collider, Entity pee3Collider, Entity pee4Collider, Entity pee5Collider, Entity pee6Collider, Entity waterBucket, Entity waterBucket2, Entity waterBucket3);

    void Initialize();
    void Update(double deltaTime);
    void CheckCollision();
    void HandlePeeCollision();
    void HandleWaterCollision();
    void SetDefaultPeePosition() const;
    void SetNewPeePosition() const;

	bool CheckEntityCollision(Entity entity);
    bool GetPeeMarked() const { return peeMarked; }
	void SetPeeMarked(bool value) { peeMarked = value; }

	bool GetTimerLimit() const { return timerLimit; }
	void SetTimerLimit(bool value) { timerLimit = value; }

	bool GetTimesUp() const { return timesUp; }
	void SetTimesUp(bool value) { timesUp = value; }

	bool GetTimerInit() const { return TimerInit; }
	void SetTimerInit(bool value) { TimerInit = value; }

    void Reset();

private:
    Entity playerEnt;
    Entity WaterBucket, WaterBucket2, WaterBucket3;
    Entity TestPee, TestCollider;
    Entity pee1, pee2, pee3, pee4, pee5, pee6;
	Entity pee1Collider, pee2Collider, pee3Collider, pee4Collider, pee5Collider, pee6Collider;

    glm::vec3 TestPos, NewPos;
	glm::vec3 pee1Pos, pee2Pos, pee3Pos, pee4Pos, pee5Pos, pee6Pos;
	glm::vec3 pee1NewPos, pee2NewPos, pee3NewPos, pee4NewPos, pee5NewPos, pee6NewPos;

    bool isFading = false;
    float fadeTimer = 0.0f;
    float fadeDuration = 2.0f; // 2-second fade


    double timer = 0.0;
    double timerLimit = 10.0;
    double timesUp = 2.0;
    bool TimerInit;
    bool peeMarked;
    bool peeSoundPlayed;
    bool waterSoundPlayed;
    bool playerCollided;
    bool testCollided;
    bool rexPee1collided, rexPee2collided, rexPee3collided, rexPee4collided, rexPee5collided, rexPee6collided;
    bool waterBucketcollided, waterBucket2collided, waterBucket3collided;
    bool touchedPee{ false };
};

extern SmellAvoidance g_SmellAvoidance;