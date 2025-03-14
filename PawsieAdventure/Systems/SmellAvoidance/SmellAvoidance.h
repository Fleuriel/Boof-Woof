#pragma once

#pragma warning(disable : 4005)
#include <EngineCore.h>

class SmellAvoidance
{
public:
	SmellAvoidance() = default;
    SmellAvoidance(Entity playerEntity, const std::vector<Entity>& peeEntities, 
        const std::vector<Entity>& peeColliders, const std::vector<Entity>& waterBuckets);

    void Initialize();
    void Update(double deltaTime);
    void CheckCollision();
    void HandlePeeCollision();
    void HandleWaterCollision();
    void SetDefaultPeePosition();
    void SetNewPeePosition();

	bool CheckEntityWithPlayerCollision(Entity entity);
	float CalculateDistance(glm::vec3& playerPos, glm::vec3& peePos);
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
	std::vector<Entity> peeEntities;
    std::vector<Entity> peeColliders;
    std::vector<Entity> waterBuckets;

    std::vector<glm::vec3> peePositions;
    std::vector<glm::vec3> peeNewPositions;

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
    bool touchedPee{ false };
    std::vector<bool> rexPeeCollided;
    std::vector<bool> waterBucketCollided;
};

extern SmellAvoidance g_SmellAvoidance;