#include "SmellAvoidance.h"
#include "../Core/AssetManager/FilePaths.h"
#include "../Utilities/ForGame/TimerTR/TimerTR.h"
#include "../Utilities/ForGame/Dialogue/Dialogue.h"

SmellAvoidance g_SmellAvoidance;

SmellAvoidance::SmellAvoidance(Entity playerEntity, const std::vector<Entity>& peeEntities, 
    const std::vector<Entity>& peeColliders, const std::vector<Entity>& waterBuckets)
    : playerEnt(playerEntity), peeEntities(peeEntities), peeColliders(peeColliders), 
    waterBuckets(waterBuckets), timer(0.0), timerLimit(10.0), timesUp(2.0), TimerInit(false), peeMarked(false),
    peeSoundPlayed(false), waterSoundPlayed(false), rexPeeCollided(peeEntities.size(), false), waterBucketCollided(waterBuckets.size(), false)
{
    peePositions.resize(peeEntities.size());
    peeNewPositions.resize(peeEntities.size());
}

void SmellAvoidance::Initialize()
{
    for (size_t i = 0; i < peeEntities.size(); ++i)
    {
        if (g_Coordinator.HaveComponent<TransformComponent>(peeEntities[i]))
        {
            auto& peeTransform = g_Coordinator.GetComponent<TransformComponent>(peeEntities[i]);
            peePositions[i] = peeTransform.GetPosition();
            peeNewPositions[i] = peePositions[i] - glm::vec3(0.0f, 20.0f, 0.0f);
            peeTransform.SetPosition(peeNewPositions[i]);
        }
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
    if (g_Coordinator.HaveComponent<CollisionComponent>(playerEnt))
    {
        playerCollided = g_Coordinator.GetComponent<CollisionComponent>(playerEnt).GetIsColliding();
    }

    for (size_t i = 0; i < peeColliders.size(); ++i)
    {
        rexPeeCollided[i] = CheckEntityWithPlayerCollision(peeColliders[i]);
    }

    for (size_t i = 0; i < waterBuckets.size(); ++i)
    {
        waterBucketCollided[i] = CheckEntityWithPlayerCollision(waterBuckets[i]);
    }

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

float SmellAvoidance::CalculateDistance(glm::vec3& playerPos, glm::vec3& peePos)
{
	return glm::distance(playerPos, peePos);
}

void SmellAvoidance::HandlePeeCollision()
{
    if (playerCollided && (std::any_of(rexPeeCollided.begin(), rexPeeCollided.end(), [](bool collided) { return collided; }))
        && !peeMarked && !peeSoundPlayed)
    {
        g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/PeePuddle.wav", false, "SFX");
        peeMarked = true;
        peeSoundPlayed = true;
        waterSoundPlayed = false;

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
    if (playerCollided && std::any_of(waterBucketCollided.begin(), waterBucketCollided.end(), [](bool collided) { return collided; }) && !waterSoundPlayed)
    {
        if (peeMarked) {
            g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/WaterPuddle.wav", false, "SFX");
            g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO + "/ClockTicking_Loop.wav");
            g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO + "/GameOver_Hit 1.wav");

            isFading = true;
            fadeTimer = 0.0f;

            g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/BedRoomMusicBGM.wav", true, "BGM");
            g_Audio.SetSoundVolume(FILEPATH_ASSET_AUDIO + "/BedRoomMusicBGM.wav", 0.0f);

            peeMarked = false;
            timer = 0.0;

            peeSoundPlayed = false;
            waterSoundPlayed = true;
            touchedPee = false;

            if (TimerInit) {
                g_TimerTR.OnShutdown();
                TimerInit = false;
            }
        }
    }
}

void SmellAvoidance::SetDefaultPeePosition()
{
    for (size_t i = 0; i < peeEntities.size(); ++i)
    {
        if (g_Coordinator.HaveComponent<TransformComponent>(peeEntities[i]))
        {
            auto& peeTransform = g_Coordinator.GetComponent<TransformComponent>(peeEntities[i]);
            peeTransform.SetPosition(peePositions[i]);
        }
    }
}



void SmellAvoidance::SetNewPeePosition()
{
    if (g_Coordinator.HaveComponent<TransformComponent>(playerEnt))
    {
        auto& playerTransform = g_Coordinator.GetComponent<TransformComponent>(playerEnt);
        glm::vec3 playerPos = playerTransform.GetPosition();

        for (size_t i = 0; i < peeEntities.size(); ++i)
        {
            if (g_Coordinator.HaveComponent<TransformComponent>(peeEntities[i]))
            {
                auto& peeTransform = g_Coordinator.GetComponent<TransformComponent>(peeEntities[i]);
                glm::vec3 peePos = peeTransform.GetPosition();

				float distance = CalculateDistance(playerPos, peePos);
                if (distance <= 10.0f)
                {	// Set your distance threshold here
                    // Keep the pee entity in its current position
                    peeTransform.SetPosition(peePos);
                }
                else
                {
                    // Proceed as usual
                    peeTransform.SetPosition(peeNewPositions[i]);
                }

            }
        }
    }
}

void SmellAvoidance::Reset()
{
    peeSoundPlayed = false;
    waterSoundPlayed = false;
    std::fill(rexPeeCollided.begin(), rexPeeCollided.end(), false);
    std::fill(waterBucketCollided.begin(), waterBucketCollided.end(), false);
    TimerInit = false;
    peeMarked = false;
    touchedPee = false;
    isFading = false;
    fadeTimer = 0.0f;
}