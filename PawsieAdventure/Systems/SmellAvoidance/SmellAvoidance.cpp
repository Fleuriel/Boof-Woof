#include "SmellAvoidance.h"
#include "../Core/AssetManager/FilePaths.h"
#include "../Utilities/ForGame/TimerTR/TimerTR.h"
#include "../Utilities/ForGame/Dialogue/Dialogue.h"

SmellAvoidance g_SmellAvoidance;

SmellAvoidance::SmellAvoidance(Entity playerEntity, Entity testPee, Entity testCollider, Entity pee1, Entity pee2, Entity pee3, Entity pee4, Entity pee5, Entity pee6, 
    Entity pee1Collider, Entity pee2Collider, Entity pee3Collider, Entity pee4Collider, Entity pee5Collider, Entity pee6Collider, Entity waterBucket, Entity waterBucket2, Entity waterBucket3)
    : playerEnt(playerEntity), TestPee(testPee), TestCollider(testCollider), pee1(pee1), pee2(pee2), pee3(pee3), pee4(pee4), pee5(pee5), pee6(pee6),
    pee1Collider(pee1Collider), pee2Collider(pee2Collider), pee3Collider(pee3Collider), pee4Collider(pee4Collider), pee5Collider(pee5Collider), pee6Collider(pee6Collider), 
    WaterBucket(waterBucket), WaterBucket2(waterBucket2), WaterBucket3(waterBucket3),
    timer(0.0), timerLimit(10.0), timesUp(2.0), TimerInit(false), peeMarked(false),
    peeSoundPlayed(false), waterSoundPlayed(false), testCollided(false), rexPee1collided(false), rexPee2collided(false),
    rexPee3collided(false), rexPee4collided(false), waterBucketcollided(false), waterBucket2collided(false), waterBucket3collided(false)
{
	// Empty by design
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

    if (g_Coordinator.HaveComponent<TransformComponent>(pee5))
    {
        auto& pee5Transform = g_Coordinator.GetComponent<TransformComponent>(pee5);
        pee5Pos = pee5Transform.GetPosition();
        pee5NewPos = pee5Pos - glm::vec3(0.0f, 20.0f, 0.0f);
        pee5Transform.SetPosition(pee5NewPos);
    }

    if (g_Coordinator.HaveComponent<TransformComponent>(pee6))
    {
        auto& pee6Transform = g_Coordinator.GetComponent<TransformComponent>(pee6);
        pee6Pos = pee6Transform.GetPosition();
        pee6NewPos = pee6Pos - glm::vec3(0.0f, 20.0f, 0.0f);
        pee6Transform.SetPosition(pee6NewPos);
    }

    touchedPee = false;
}

void SmellAvoidance::Update(double deltaTime)
{
    // Update logic for smell avoidance
    CheckCollision();
    // ? Handle BGM Fading if triggered
    if (isFading)
    {
        fadeTimer += static_cast<float>(deltaTime); // Increase fade timer
        float progress = fadeTimer / fadeDuration; // Normalize fade time (0.0 ? 1.0)

        if (progress > 1.0f) progress = 1.0f; // Clamp to max value

        // ? Gradually fade out `Music_Danger_Loop`
        float dangerVolume = 1.0f - progress;
        g_Audio.SetSoundVolume(FILEPATH_ASSET_AUDIO + "/Music_Danger_Loop.wav", dangerVolume);

        // ? Gradually fade in `BedRoomMusicBGM`
        g_Audio.SetSoundVolume(FILEPATH_ASSET_AUDIO + "/BedRoomMusicBGM.wav", progress);

        // ? When fade is complete, stop the danger music
        if (progress >= 1.0f)
        {
            g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO + "/Music_Danger_Loop.wav");
            isFading = false; // ? Stop fading process
        }
    }
}

void SmellAvoidance::CheckCollision()
{
	//playerCollided = CheckEntityWithPlayerCollision(playerEnt);
    if (g_Coordinator.HaveComponent<CollisionComponent>(playerEnt))
    {
        playerCollided = g_Coordinator.GetComponent<CollisionComponent>(playerEnt).GetIsColliding();
    }
    rexPee1collided = CheckEntityWithPlayerCollision(pee1Collider);
    rexPee2collided = CheckEntityWithPlayerCollision(pee2Collider);
    rexPee3collided = CheckEntityWithPlayerCollision(pee3Collider);
    rexPee4collided = CheckEntityWithPlayerCollision(pee4Collider);
    rexPee5collided = CheckEntityWithPlayerCollision(pee5Collider);
    rexPee6collided = CheckEntityWithPlayerCollision(pee6Collider);
    testCollided = CheckEntityWithPlayerCollision(TestCollider);
    waterBucketcollided = CheckEntityWithPlayerCollision(WaterBucket);
    waterBucket2collided = CheckEntityWithPlayerCollision(WaterBucket2);
    waterBucket3collided = CheckEntityWithPlayerCollision(WaterBucket3);

    HandlePeeCollision();
    HandleWaterCollision();
}

bool SmellAvoidance::CheckEntityWithPlayerCollision(Entity entity)
{
    //Check Entity Collision with Player
    if (g_Coordinator.HaveComponent<CollisionComponent>(entity) && g_Coordinator.HaveComponent<CollisionComponent>(playerEnt))
    {
        auto collider1 = g_Coordinator.GetComponent<CollisionComponent>(entity);
        if (collider1.GetIsColliding() && std::strcmp(collider1.GetLastCollidedObjectName().c_str(), "Player") == 0)
            return true;
    }
    return false;
}

void SmellAvoidance::HandlePeeCollision()
{
    if (playerCollided && (rexPee1collided || rexPee2collided || rexPee3collided || rexPee4collided || rexPee5collided || rexPee6collided || testCollided) 
        && !peeMarked && !peeSoundPlayed)
    {
        g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/PeePuddle.wav", false, "SFX");
        peeMarked = true;
        peeSoundPlayed = true;  // Ensure the sound plays only once
        waterSoundPlayed = false; // Reset water sound state

        if (!touchedPee)
        {
            g_DialogueText.OnInitialize();
            g_DialogueText.setDialogue(DialogueState::DISGUSTED2);
            touchedPee = true;
        }
    }
}

void SmellAvoidance::HandleWaterCollision()
{
    if (playerCollided && (waterBucketcollided || waterBucket2collided || waterBucket3collided) && !waterSoundPlayed)
    {
        if (peeMarked) {
            g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/WaterPuddle.wav", false, "SFX");
            g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO + "/ClockTicking_Loop.wav");
            g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO + "/GameOver_Hit 1.wav");

            // ? Start fading process instead of blocking the game loop
            isFading = true;
            fadeTimer = 0.0f;

            // ? Start playing new BGM at **0 volume**
            g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/BedRoomMusicBGM.wav", true, "BGM");
            g_Audio.SetSoundVolume(FILEPATH_ASSET_AUDIO + "/BedRoomMusicBGM.wav", 0.0f); // Start silent

            peeMarked = false;
            timer = 0.0;

            // Reset sound state
            peeSoundPlayed = false;
            waterSoundPlayed = true; // Ensure water sound plays only once
            touchedPee = false;

            if (TimerInit) {
                g_TimerTR.OnShutdown();
                TimerInit = false;
            }
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

    if (g_Coordinator.HaveComponent<TransformComponent>(pee5))
    {
        auto& pee5Transform = g_Coordinator.GetComponent<TransformComponent>(pee5);
        pee5Transform.SetPosition(pee5Pos);
    }

    if (g_Coordinator.HaveComponent<TransformComponent>(pee6))
    {
        auto& pee6Transform = g_Coordinator.GetComponent<TransformComponent>(pee6);
        pee6Transform.SetPosition(pee6Pos);
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

    if (g_Coordinator.HaveComponent<TransformComponent>(pee5))
    {
        auto& pee5Transform = g_Coordinator.GetComponent<TransformComponent>(pee5);
        pee5Transform.SetPosition(pee5NewPos);
    }

    if (g_Coordinator.HaveComponent<TransformComponent>(pee6))
    {
        auto& pee6Transform = g_Coordinator.GetComponent<TransformComponent>(pee6);
        pee6Transform.SetPosition(pee6NewPos);
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
	rexPee5collided = false;
	rexPee6collided = false;
	waterBucketcollided = false;
	waterBucket2collided = false;
	waterBucket3collided = false;
	TimerInit = false;
	peeMarked = false;
    touchedPee = false;
    isFading = false;
    fadeTimer = 0.0f;
}