#include "SmellAvoidance.h"
#include "../Core/AssetManager/FilePaths.h"
#include "../Utilities/ForGame/TimerTR/TimerTR.h"
#include "../Utilities/ForGame/Dialogue/Dialogue.h"

SmellAvoidance g_SmellAvoidance;

SmellAvoidance::SmellAvoidance(Entity playerEntity, Entity pee1, Entity pee2, Entity pee3, Entity pee4, Entity pee1Collider, Entity pee2Collider, Entity pee3Collider, Entity pee4Collider, Entity waterBucket, Entity waterBucket2, Entity waterBucket3, Entity testPee, Entity testCollider)
    : playerEnt(playerEntity), pee1(pee1), pee2(pee2), pee3(pee3), pee4(pee4), pee1Collider(pee1Collider), pee2Collider(pee2Collider), pee3Collider(pee3Collider), pee4Collider(pee4Collider), WaterBucket(waterBucket), WaterBucket2(waterBucket2), WaterBucket3(waterBucket3), TestPee(testPee), TestCollider(testCollider),
    timer(0.0), timerLimit(10.0), timesUp(2.0), TimerInit(false), peeMarked(false),
    peeSoundPlayed(false), waterSoundPlayed(false), testCollided(false), rexPee1collided(false), rexPee2collided(false),
    rexPee3collided(false), rexPee4collided(false), waterBucketcollided(false), waterBucket2collided(false), waterBucket3collided(false)
{
}

void SmellAvoidance::Initialize()
{
    // Initialize positions and other necessary components
    if (g_Coordinator.HaveComponent<TransformComponent>(TestPee))
    {
        auto& testPeeTransform = g_Coordinator.GetComponent<TransformComponent>(TestPee);
        TestPos = testPeeTransform.GetPosition();
        NewPos = TestPos - glm::vec3(0.0f, 20.0f, 0.0f);
        testPeeTransform.SetPosition(NewPos);
    }

    if (g_Coordinator.HaveComponent<TransformComponent>(pee1))
    {
        auto& pee1Transform = g_Coordinator.GetComponent<TransformComponent>(pee1);
        pee1Pos = pee1Transform.GetPosition();
        pee1NewPos = pee1Pos - glm::vec3(0.0f, 20.0f, 0.0f);
        pee1Transform.SetPosition(pee1NewPos);
    }

    if (g_Coordinator.HaveComponent<TransformComponent>(pee2))
    {
        auto& pee2Transform = g_Coordinator.GetComponent<TransformComponent>(pee2);
        pee2Pos = pee2Transform.GetPosition();
        pee2NewPos = pee2Pos - glm::vec3(0.0f, 20.0f, 0.0f);
        pee2Transform.SetPosition(pee2NewPos);
    }

    if (g_Coordinator.HaveComponent<TransformComponent>(pee3))
    {
        auto& pee3Transform = g_Coordinator.GetComponent<TransformComponent>(pee3);
        pee3Pos = pee3Transform.GetPosition();
        pee3NewPos = pee3Pos - glm::vec3(0.0f, 20.0f, 0.0f);
        pee3Transform.SetPosition(pee3NewPos);
    }

    if (g_Coordinator.HaveComponent<TransformComponent>(pee4))
    {
        auto& pee4Transform = g_Coordinator.GetComponent<TransformComponent>(pee4);
        pee4Pos = pee4Transform.GetPosition();
        pee4NewPos = pee4Pos - glm::vec3(0.0f, 20.0f, 0.0f);
        pee4Transform.SetPosition(pee4NewPos);
    }
}

void SmellAvoidance::Update(double deltaTime)
{
    // Update logic for smell avoidance
    CheckCollision();
}

void SmellAvoidance::CheckCollision()
{
	playerCollided = CheckEntityCollision(playerEnt);
    rexPee1collided = CheckEntityCollision(pee1Collider);
    rexPee2collided = CheckEntityCollision(pee2Collider);
    rexPee3collided = CheckEntityCollision(pee3Collider);
    rexPee4collided = CheckEntityCollision(pee4Collider);
    testCollided = CheckEntityCollision(TestCollider);
    waterBucketcollided = CheckEntityCollision(WaterBucket);
    waterBucket2collided = CheckEntityCollision(WaterBucket2);
    waterBucket3collided = CheckEntityCollision(WaterBucket3);

    HandlePeeCollision();
    HandleWaterCollision();
}

bool SmellAvoidance::CheckEntityCollision(Entity entity)
{
    if (g_Coordinator.HaveComponent<CollisionComponent>(entity))
    {
        return g_Coordinator.GetComponent<CollisionComponent>(entity).GetIsColliding();
    }
    return false;
}

void SmellAvoidance::HandlePeeCollision()
{
    if (playerCollided && (rexPee1collided || rexPee2collided || rexPee3collided || rexPee4collided || testCollided) && !peeMarked && !peeSoundPlayed)
    {
        g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/PeePuddle.wav", false, "SFX");
        peeMarked = true;
        peeSoundPlayed = true;  // Ensure the sound plays only once
        waterSoundPlayed = false; // Reset water sound state

        if (!firstPeeTouched) 
        {
            g_DialogueText.OnInitialize();
            g_DialogueText.setDialogue(DialogueState::DISGUSTED);
            firstPeeTouched = true;
        }
    }
}

void SmellAvoidance::HandleWaterCollision()
{
    if (playerCollided && (waterBucketcollided || waterBucket2collided || waterBucket3collided) && !waterSoundPlayed)
    {
        g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/WaterPuddle.wav", false, "SFX");
        g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO + "/ClockTicking_Loop.wav");
        g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO + "/GameOver_Hit 1.wav");

        peeMarked = false;
        timer = 0.0;

        // Reset sound state
        peeSoundPlayed = false;
        waterSoundPlayed = true; // Ensure water sound plays only once
        if (TimerInit) {
            g_TimerTR.OnShutdown();
            TimerInit = false;
        }
    }
}

void SmellAvoidance::SetDefaultPeePosition() const
{
    if (g_Coordinator.HaveComponent<TransformComponent>(TestPee))
    {
        auto& testPeeTransform = g_Coordinator.GetComponent<TransformComponent>(TestPee);
        testPeeTransform.SetPosition(TestPos);
    }

    if (g_Coordinator.HaveComponent<TransformComponent>(pee1))
    {
        auto& pee1Transform = g_Coordinator.GetComponent<TransformComponent>(pee1);
        pee1Transform.SetPosition(pee1Pos);
    }

    if (g_Coordinator.HaveComponent<TransformComponent>(pee2))
    {
        auto& pee2Transform = g_Coordinator.GetComponent<TransformComponent>(pee2);
        pee2Transform.SetPosition(pee2Pos);
    }

    if (g_Coordinator.HaveComponent<TransformComponent>(pee3))
    {
        auto& pee3Transform = g_Coordinator.GetComponent<TransformComponent>(pee3);
        pee3Transform.SetPosition(pee3Pos);
    }

    if (g_Coordinator.HaveComponent<TransformComponent>(pee4))
    {
        auto& pee4Transform = g_Coordinator.GetComponent<TransformComponent>(pee4);
        pee4Transform.SetPosition(pee4Pos);
    }
}

void SmellAvoidance::SetNewPeePosition() const
{
    if (g_Coordinator.HaveComponent<TransformComponent>(TestPee))
    {
        auto& testPeeTransform = g_Coordinator.GetComponent<TransformComponent>(TestPee);
        testPeeTransform.SetPosition(NewPos);
    }

    if (g_Coordinator.HaveComponent<TransformComponent>(pee1))
    {
        auto& pee1Transform = g_Coordinator.GetComponent<TransformComponent>(pee1);
        pee1Transform.SetPosition(pee1NewPos);
    }

    if (g_Coordinator.HaveComponent<TransformComponent>(pee2))
    {
        auto& pee2Transform = g_Coordinator.GetComponent<TransformComponent>(pee2);
        pee2Transform.SetPosition(pee2NewPos);
    }

    if (g_Coordinator.HaveComponent<TransformComponent>(pee3))
    {
        auto& pee3Transform = g_Coordinator.GetComponent<TransformComponent>(pee3);
        pee3Transform.SetPosition(pee3NewPos);
    }

    if (g_Coordinator.HaveComponent<TransformComponent>(pee4))
    {
        auto& pee4Transform = g_Coordinator.GetComponent<TransformComponent>(pee4);
        pee4Transform.SetPosition(pee4NewPos);
    }
}

void SmellAvoidance::Reset()
{
	peeSoundPlayed = false;
	waterSoundPlayed = false;
	testCollided = false;
	rexPee1collided = false;
	rexPee2collided = false;
	rexPee3collided = false;
	rexPee4collided = false;
	waterBucketcollided = false;
	waterBucket2collided = false;
	waterBucket3collided = false;
	TimerInit = false;
	peeMarked = false;
    firstPeeTouched = false;
}