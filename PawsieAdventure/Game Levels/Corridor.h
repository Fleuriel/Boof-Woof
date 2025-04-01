#pragma once
#include "Level Manager/Level.h"
#include "ResourceManager/ResourceManager.h"
#include "ECS/Coordinator.hpp"
#include "../BoofWoof/Core/AssetManager/FilePaths.h"
#include "../Systems/CameraController/CameraController.h"
#include "../GSM/GameStateMachine.h"

// Assuming these globals (or similar) are defined elsewhere:
extern int g_WindowX;
extern int g_WindowY;

class Corridor : public Level
{
    Entity playerEnt{};
    CameraController* cameraController = nullptr;

    double sniffCooldownTimer = 0.0;         // Timer for sniff cooldown
    const double sniffCooldownDuration = 17.0; // 17 seconds cooldown
    bool isSniffOnCooldown = false;            // Track cooldown state

    bool bark = false;

    std::vector<std::string> bitingSounds = {
        "Corgi/DogBite_01.wav",
        "Corgi/DogBite_02.wav",
        "Corgi/DogBite_03.wav",
        "Corgi/DogBite_04.wav",
        "Corgi/DogBite_05.wav",
        "Corgi/DogBite_06.wav",
        "Corgi/DogBite_07.wav",
    };

    // Transition state variables
    bool transitionActive = false;
    float transitionTimer = 0.0f;
    const float transitionDuration = 1.0f; // Duration in seconds

    // Function to get a random sound from a vector
    std::string GetRandomSound(const std::vector<std::string>& soundList) {
        static std::random_device rd;
        static std::mt19937 gen(rd()); // Mersenne Twister PRNG
        std::uniform_int_distribution<std::size_t> dis(0, soundList.size() - 1);
        return soundList[dis(gen)];
    }

public:
    void LoadLevel() override
    {
        g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES + "/SecondCorridor.json");

        g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/BedRoomMusicBGM.wav", true, "BGM");
        g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/ambienceSFX.wav", true, "SFX");

        std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();

        // Use unordered_map for O(1) efficiency.
        std::unordered_map<std::string, std::function<void(Entity)>> nameToAction =
        {
            {"Player", [&](Entity entity) { playerEnt = entity; }},
        };

        for (auto entity : entities)
        {
            if (g_Coordinator.HaveComponent<MetadataComponent>(entity))
            {
                const auto& metadata = g_Coordinator.GetComponent<MetadataComponent>(entity);
                auto it = nameToAction.find(metadata.GetName());
                if (it != nameToAction.end())
                {
                    it->second(entity);
                }
                // Exit early if all entities are found.
                if (playerEnt)
                {
                    break;
                }
            }
        }
        g_Window->HideMouseCursor();
    }

    void InitLevel() override
    {
        // Ensure player entity is valid.
        cameraController = new CameraController(playerEnt);
        camerachange = false;

        g_Audio.SetBGMVolume(g_Audio.GetBGMVolume());
        g_Audio.SetSFXVolume(g_Audio.GetSFXVolume());

        g_DialogueText.OnInitialize();
        g_DialogueText.setDialogue(DialogueState::OUTOFLIBRARY);
        g_UI.OnInitialize();
    }

    void UpdateLevel(double deltaTime) override
    {
        if (!g_DialogueText.dialogueActive)
        {
            pauseLogic::OnUpdate();
        }

        if (!g_IsPaused)
        {
            if (!camerachange)
            {
                cameraController->ChangeToFirstPerson(g_Coordinator.GetComponent<CameraComponent>(playerEnt));
                camerachange = true;
            }

            cameraController->Update(static_cast<float>(deltaTime));
            g_UI.OnUpdate(static_cast<float>(deltaTime));
            g_DialogueText.OnUpdate(deltaTime);

            // Process bark action (bite sound)
            if (g_Input.GetMouseState(GLFW_MOUSE_BUTTON_RIGHT) == 1 && !bark)
            {
                std::string biteSound = FILEPATH_ASSET_AUDIO + "/" + GetRandomSound(bitingSounds);
                g_Audio.PlayFileOnNewChannel(biteSound.c_str(), false, "SFX");
                bark = true;
                // Reset bark after a short delay.
                std::thread([&]() {
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    bark = false;
                    }).detach();
            }
            if (g_Input.GetMouseState(GLFW_MOUSE_BUTTON_RIGHT) == 0)
            {
                bark = false;
            }

            // Accumulate time for sniff cooldown.
            if (isSniffOnCooldown)
            {
                sniffCooldownTimer += deltaTime;
                if (sniffCooldownTimer >= sniffCooldownDuration)
                {
                    isSniffOnCooldown = false;
                    sniffCooldownTimer = 0.0;
                }
            }

            // Sniffing logic with cooldown check.
            if (g_Input.GetKeyState(GLFW_KEY_E) >= 1 && !isSniffOnCooldown)
            {
                g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/CorgiSniff.wav", false, "SFX");
                isSniffOnCooldown = true;
                sniffCooldownTimer = 0.0;
            }

            // Start transition if player collides with the exit.
            if (!transitionActive &&
                g_Coordinator.GetComponent<CollisionComponent>(playerEnt).GetLastCollidedObjectName() == "CastleWallDoor")
            {
                transitionActive = true;
                transitionTimer = 0.0f;
            }

            // If transition is active, update timer and render the effect.
            if (transitionActive)
            {
                transitionTimer += static_cast<float>(deltaTime);
                float progress = transitionTimer / transitionDuration; // Progress from 0.0 to 1.0.
                // Call the GraphicsSystem's modern transition effect.
                g_Coordinator.GetSystem<GraphicsSystem>()->RenderTransitionEffect(progress);

                if (transitionTimer >= transitionDuration)
                {
                    // Transition complete, trigger level change.
                    auto* loading = dynamic_cast<LoadingLevel*>(g_LevelManager.GetLevel("LoadingLevel"));
                    if (loading)
                    {
                        loading->m_NextScene = "MainHall";
                        g_LevelManager.SetNextLevel("LoadingLevel");
                    }
                }
            }
        }
    }

    void FreeLevel() override
    {
        // Reset all state variables upon exit.
        transitionActive = false;
        transitionTimer = 0.0f;
        isSniffOnCooldown = false;
        sniffCooldownTimer = 0.0;
        bark = false;

        if (cameraController)
        {
            delete cameraController;
            cameraController = nullptr;
        }
        g_UI.OnShutdown();
        g_DialogueText.OnShutdown();
    }

    void UnloadLevel() override
    {
        // Reset state variables to ensure a clean exit.
        transitionActive = false;
        transitionTimer = 0.0f;
        isSniffOnCooldown = false;
        sniffCooldownTimer = 0.0;
        bark = false;

        g_Coordinator.GetSystem<MyPhysicsSystem>()->ClearAllBodies();
        g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO + "/BedRoomMusicBGM.wav");
        g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO + "/Music_Danger_Loop.wav");
        g_Coordinator.ResetEntities();
    }

private:
    bool camerachange = false;
};
