#pragma once
#include "Level Manager/Level.h"
#include "ResourceManager/ResourceManager.h"
#include "ECS/Coordinator.hpp"
#include "../BoofWoof/Core/AssetManager/FilePaths.h"

class Cutscene : public Level
{
    double cutsceneTimer = 0.0;
    double timerLimit = 4.0;
    int textureIndex = 0;  // To track which texture we're currently showing
    double lastBarkTime = 0.0;
    Entity TextEnt{}, DogName{};
    Entity AggroDog{}, CorgiWhimper{}, corgi12sec{};


    void LoadLevel()
    {
        // Use FILEPATH_ASSET_SCENES to construct the scene file path
        g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES + "/Cutscene.json");

        std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();

        for (auto entity : entities)
        {
            if (g_Coordinator.HaveComponent<MetadataComponent>(entity))
            {
                const auto& metadata = g_Coordinator.GetComponent<MetadataComponent>(entity);
                const std::string& name = metadata.GetName();

                if (name == "Text")
                {
                    TextEnt = entity;
                    std::cout << "TEXT FOUND" << std::endl;
                }
                else if (name == "DogName")
                {
                    DogName = entity;
                    std::cout << "DOG NAME FOUND" << std::endl;
                }
                else if (name == "AggressiveDogBarking")
                {
                    AggroDog = entity;
                    std::cout << "AGGRESSIVE DOG BARKING FOUND" << std::endl;
                }
                else if (name == "CorgiWhimper")
                {
                    CorgiWhimper = entity;
                    std::cout << "CORGI WHIMPER FOUND" << std::endl;
                }
                else if (name == "12sGrowlBarkCorgi")
                {
                    corgi12sec = entity;
                    std::cout << "12s GROWL BARK CORGI FOUND" << std::endl;
                }

                // Exit early if all entities are found
                if (TextEnt && DogName && AggroDog && CorgiWhimper && corgi12sec)
                {
                    break;
                }
            }
        }


        if (g_Coordinator.HaveComponent<AudioComponent>(AggroDog)) {

            auto& music = g_Coordinator.GetComponent<AudioComponent>(AggroDog);
            music.SetAudioSystem(&g_Audio);
        }

        if (g_Coordinator.HaveComponent<AudioComponent>(CorgiWhimper)) {
            auto& music1 = g_Coordinator.GetComponent<AudioComponent>(CorgiWhimper);
            music1.SetAudioSystem(&g_Audio);
        }
        if (g_Coordinator.HaveComponent<AudioComponent>(corgi12sec)) {
            auto& music2 = g_Coordinator.GetComponent<AudioComponent>(corgi12sec);
            music2.SetAudioSystem(&g_Audio);
        }
    }

    void InitLevel() { /* Empty by design */ }

    void UpdateLevel(double deltaTime)
    {
        cutsceneTimer += deltaTime;

        if (!g_Coordinator.HaveComponent<GraphicsComponent>(TextEnt)) return;

        auto& text = g_Coordinator.GetComponent<GraphicsComponent>(TextEnt);
        auto& dogName = g_Coordinator.GetComponent<GraphicsComponent>(DogName);

        // If it's time to change the texture
        if (cutsceneTimer >= timerLimit && textureIndex < 4) // Only switch to CorgiText1, CorgiText2, CorgiText3, CorgiText4
        {
            int oldNameTextureId = g_ResourceManager.GetTextureDDS(dogName.getTextureName());
            if (dogName.RemoveTexture(oldNameTextureId))
            {
                dogName.clearTextures();
                int textureIdD = g_ResourceManager.GetTextureDDS("ScruffyTheCorgi");
                dogName.AddTexture(textureIdD);
                dogName.setTexture("ScruffyTheCorgi");
            }

            std::string newTextureName = "CorgiText" + std::to_string(textureIndex + 1);

            int oldTextureId = g_ResourceManager.GetTextureDDS(text.getTextureName());
            if (text.RemoveTexture(oldTextureId))
            {
                // Remove old texture & add new one
                text.clearTextures();

                int textureId = g_ResourceManager.GetTextureDDS(newTextureName);
                text.AddTexture(textureId);
                text.setTexture(newTextureName);
            }

            cutsceneTimer = 0.0;  // Reset the timer after each texture change
            textureIndex++;       // Move to the next texture
        }

        switch (textureIndex)
        {
        case 0:
        {
            // Big dog telling you stay in the room
            //g_Audio.PlayFile(FILEPATH_ASSET_AUDIO + "/AggressiveDogBarking.wav");
            if (g_Coordinator.HaveComponent<AudioComponent>(AggroDog)) {
                auto& music = g_Coordinator.GetComponent<AudioComponent>(AggroDog);
                music.PlayAudio();
            }

            break;
        }
        case 1:
        {
            // I'm scared
            if (cutsceneTimer <= 3.0)
            {
               // g_Audio.PlayFile(FILEPATH_ASSET_AUDIO + "/CorgiWhimper.wav");
                if (g_Coordinator.HaveComponent<AudioComponent>(CorgiWhimper)) {
                    auto& music1 = g_Coordinator.GetComponent<AudioComponent>(CorgiWhimper);
                    music1.PlayAudio();
                }
            }
            break;
        }
        case 2:
        {
            // I can't keep living like this
            // I want to leave this place
            // No.. I WILL LEAVE THE CASTLE
           // g_Audio.PlayFile(FILEPATH_ASSET_AUDIO + "/12sGrowlBarkCorgi.wav");
            if (g_Coordinator.HaveComponent<AudioComponent>(corgi12sec)) {
                auto& music2 = g_Coordinator.GetComponent<AudioComponent>(corgi12sec);
                music2.PlayAudio();
            }
            break;
        }
        }

        // After showing CorgiText4 for 3 seconds, transition to the next scene
        if (textureIndex >= 4 && cutsceneTimer >= timerLimit)
        {
            g_LevelManager.SetNextLevel("StartingRoom");  // Transition to the next level after CorgiText4
        }
    }

    void FreeLevel() { /* Empty by design */ }

    void UnloadLevel()
    {
        g_Coordinator.ResetEntities();
        cutsceneTimer = 0.0;
        textureIndex = 0;
    }
};
