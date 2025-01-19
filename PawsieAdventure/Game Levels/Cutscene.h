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

	Entity P1{}, P2{}, P3{}, P4{}, P5{};
    bool rightAppeared{ false };

    void LoadLevel()
    {
        // Use FILEPATH_ASSET_SCENES to construct the scene file path
        g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES + "/Cutscene3_1.json");

        std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();

        for (auto entity : entities)
        {
            if (g_Coordinator.HaveComponent<MetadataComponent>(entity))
            {
                if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Panel1")
                {
                    P1 = entity;
                }

                if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Panel2")
                {
                    P2 = entity;
                }

				if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Panel3")
				{
                    P3 = entity;
				}

                if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Panel4")
                {
                    P4 = entity;
                }

                if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Panel5")
                {
                    P5 = entity;
                }
            }
        }
    }

    void InitLevel() { /* Empty by design */ }

    void UpdateLevel(double deltaTime)
    {
        // Whole panel panning upwards
        if (g_Coordinator.HaveComponent<UIComponent>(P1)) 
        {
			auto& onePiece = g_Coordinator.GetComponent<UIComponent>(P1);
			onePiece.set_position(glm::vec2(onePiece.get_position().x, onePiece.get_position().y + (0.2f * static_cast<float>(deltaTime))));

            if (onePiece.get_position().y >= 0.6f) 
            {
				// Left half & Right will pan upwards tgt but Right will appear later
				if (g_Coordinator.HaveComponent<UIComponent>(P2) && g_Coordinator.HaveComponent<UIComponent>(P3) && g_Coordinator.HaveComponent<UIComponent>(P4))
				{
					auto& leftHalf = g_Coordinator.GetComponent<UIComponent>(P2);
					leftHalf.set_position(glm::vec2(leftHalf.get_position().x, leftHalf.get_position().y + (0.2f * static_cast<float>(deltaTime))));

                    auto& quote = g_Coordinator.GetComponent<UIComponent>(P3);
                    quote.set_position(glm::vec2(quote.get_position().x, quote.get_position().y + (0.2f * static_cast<float>(deltaTime))));

                    auto& rightHalf = g_Coordinator.GetComponent<UIComponent>(P4);
                    rightHalf.set_position(glm::vec2(rightHalf.get_position().x, rightHalf.get_position().y + (0.2f * static_cast<float>(deltaTime))));

                    // Gradually increase opacity for LeftHalf
                    if (leftHalf.get_opacity() < 1.0f)
                    {
                        leftHalf.set_opacity(leftHalf.get_opacity() + 0.3f * static_cast<float>(deltaTime));
                    }

                    if (leftHalf.get_opacity() >= 0.8f)
                    {
                        if (quote.get_opacity() < 1.0f)
                        {
                            quote.set_opacity(quote.get_opacity() + 0.3f * static_cast<float>(deltaTime));
                        }                       
                    }

                    if (quote.get_opacity() >= 0.6f)
                    {
                        if (rightHalf.get_opacity() < 1.0f)
                        {
                            rightHalf.set_opacity(rightHalf.get_opacity() + 0.3f * static_cast<float>(deltaTime));
                        }
                    }		

                    if (rightHalf.get_position().y >= 0.6f)
                    {
                        rightAppeared = true;
                    }
				}
            }

            if (g_Coordinator.HaveComponent<UIComponent>(P5) && rightAppeared) 
            {
                auto& secondPiece = g_Coordinator.GetComponent<UIComponent>(P5);
                secondPiece.set_position(glm::vec2(secondPiece.get_position().x, secondPiece.get_position().y + (0.2f * static_cast<float>(deltaTime))));

                if (secondPiece.get_opacity() < 1.0f)
                {
                    secondPiece.set_opacity(secondPiece.get_opacity() + 0.3f * static_cast<float>(deltaTime));
                }
            }
        }

        //if (!g_Coordinator.HaveComponent<GraphicsComponent>(TextEnt)) return;

        //auto& text = g_Coordinator.GetComponent<GraphicsComponent>(TextEnt);
        //auto& dogName = g_Coordinator.GetComponent<GraphicsComponent>(DogName);

        //// If it's time to change the texture
        //if (cutsceneTimer >= timerLimit && textureIndex < 4) // Only switch to CorgiText1, CorgiText2, CorgiText3, CorgiText4
        //{
        //    int oldNameTextureId = g_ResourceManager.GetTextureDDS(dogName.getTextureName());
        //    if (dogName.RemoveTexture(oldNameTextureId))
        //    {
        //        dogName.clearTextures();
        //        int textureIdD = g_ResourceManager.GetTextureDDS("ScruffyTheCorgi");
        //        dogName.AddTexture(textureIdD);
        //        dogName.setTexture("ScruffyTheCorgi");
        //    }

        //    std::string newTextureName = "CorgiText" + std::to_string(textureIndex + 1);

        //    int oldTextureId = g_ResourceManager.GetTextureDDS(text.getTextureName());
        //    if (text.RemoveTexture(oldTextureId))
        //    {
        //        // Remove old texture & add new one
        //        text.clearTextures();

        //        int textureId = g_ResourceManager.GetTextureDDS(newTextureName);
        //        text.AddTexture(textureId);
        //        text.setTexture(newTextureName);
        //    }

        //    cutsceneTimer = 0.0;  // Reset the timer after each texture change
        //    textureIndex++;       // Move to the next texture
        //}

        //switch (textureIndex)
        //{
        //case 0:
        //{
        //    // Big dog telling you stay in the room
        //    g_Audio.PlayFile(FILEPATH_ASSET_AUDIO + "/AggressiveDogBarking.wav");
        //    break;
        //}
        //case 1:
        //{
        //    // I'm scared
        //    if (cutsceneTimer <= 3.0)
        //    {
        //        g_Audio.PlayFile(FILEPATH_ASSET_AUDIO + "/CorgiWhimper.wav");
        //    }
        //    break;
        //}
        //case 2:
        //{
        //    // I can't keep living like this
        //    // I want to leave this place
        //    // No.. I WILL LEAVE THE CASTLE
        //    g_Audio.PlayFile(FILEPATH_ASSET_AUDIO + "/12sGrowlBarkCorgi.wav");
        //    break;
        //}
        //}

        // After showing CorgiText4 for 3 seconds, transition to the next scene
        //if (textureIndex >= 4 && cutsceneTimer >= timerLimit)
        //{
        //    g_LevelManager.SetNextLevel("StartingRoom");  // Transition to the next level after CorgiText4
        //}
    }

    void FreeLevel() { /* Empty by design */ }

    void UnloadLevel()
    {
        g_Coordinator.ResetEntities();
        cutsceneTimer = 0.0;
        textureIndex = 0;
    }
};
