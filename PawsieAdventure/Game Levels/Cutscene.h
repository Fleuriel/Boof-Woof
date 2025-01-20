#pragma once
#include "Level Manager/Level.h"
#include "ResourceManager/ResourceManager.h"
#include "ECS/Coordinator.hpp"
#include "../BoofWoof/Core/AssetManager/FilePaths.h"

class Cutscene : public Level
{
	double cutsceneTimer = 0.0;
	double timerLimit = 4.0;
	double lastBarkTime = 0.0;

	Entity P1{}, P2{}, P3{}, P4{}, P5{}, P6{}, P7{}, P8{}, P9{}, P10{}, P11{}, P12{};
	bool rightAppeared{ false }, finishCutscene{ false };

	void LoadLevel()
	{
		// Use FILEPATH_ASSET_SCENES to construct the scene file path
		g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES + "/Cutscene.json");

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

				if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Panel6")
				{
					P6 = entity;
				}

				if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Panel7")
				{
					P7 = entity;
				}

				if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Panel8")
				{
					P8 = entity;
				}

				if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Panel9")
				{
					P9 = entity;
				}

				if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Panel10")
				{
					P10 = entity;
				}

				if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Panel11")
				{
					P11 = entity;
				}

				if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Panel12")
				{
					P12 = entity;
				}
			}
		}
	}

	void InitLevel() { /* Empty by design */ }

	void UpdateLevel(double deltaTime)
	{
		// Whole panel panning upwards
		if (g_Coordinator.HaveComponent<UIComponent>(P1) && !finishCutscene)
		{
			auto& onePiece = g_Coordinator.GetComponent<UIComponent>(P1);
			auto& leftHalf = g_Coordinator.GetComponent<UIComponent>(P2);
			auto& quote = g_Coordinator.GetComponent<UIComponent>(P3);
			auto& rightHalf = g_Coordinator.GetComponent<UIComponent>(P4);

			onePiece.set_position(glm::vec2(onePiece.get_position().x, onePiece.get_position().y + (0.2f * static_cast<float>(deltaTime))));		

			if (onePiece.get_position().y >= 0.6f)
			{
				// Left half & Right will pan upwards tgt but Right will appear later

				leftHalf.set_position(glm::vec2(leftHalf.get_position().x, leftHalf.get_position().y + (0.2f * static_cast<float>(deltaTime))));
				quote.set_position(glm::vec2(quote.get_position().x, quote.get_position().y + (0.2f * static_cast<float>(deltaTime))));
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

				if (rightHalf.get_position().y >= 0.6f && !rightAppeared)
				{
					rightAppeared = true;
				}

			}

			if (g_Coordinator.HaveComponent<UIComponent>(P5) && rightAppeared)
			{
				auto& twoPiece = g_Coordinator.GetComponent<UIComponent>(P5);
				auto& quote2 = g_Coordinator.GetComponent<UIComponent>(P6);
				auto& threePiece = g_Coordinator.GetComponent<UIComponent>(P7);

				auto& Left8 = g_Coordinator.GetComponent<UIComponent>(P8);
				auto& Right9 = g_Coordinator.GetComponent<UIComponent>(P9);

				auto& FourPiece = g_Coordinator.GetComponent<UIComponent>(P10);
				auto& Left11 = g_Coordinator.GetComponent<UIComponent>(P11);
				auto& Right12 = g_Coordinator.GetComponent<UIComponent>(P12);

				twoPiece.set_position(glm::vec2(twoPiece.get_position().x, twoPiece.get_position().y + (0.2f * static_cast<float>(deltaTime))));

				if (twoPiece.get_opacity() < 1.0f)
				{
					twoPiece.set_opacity(twoPiece.get_opacity() + 0.3f * static_cast<float>(deltaTime));
				}

				if (twoPiece.get_position().y >= 0.0f)
				{
					quote2.set_position(glm::vec2(quote2.get_position().x, quote2.get_position().y + (0.2f * static_cast<float>(deltaTime))));
				
					if (twoPiece.get_opacity() >= 0.8f)
					{
						if (quote2.get_opacity() < 1.0f)
						{
							quote2.set_opacity(quote2.get_opacity() + 0.3f * static_cast<float>(deltaTime));
						}
					}
				}
				
				if (twoPiece.get_position().y >= 0.6f)
				{
					threePiece.set_position(glm::vec2(threePiece.get_position().x, threePiece.get_position().y + (0.2f * static_cast<float>(deltaTime))));

					if (threePiece.get_opacity() < 1.0f)
					{
						threePiece.set_opacity(threePiece.get_opacity() + 0.3f * static_cast<float>(deltaTime));
					}
				}

				if (threePiece.get_position().y >= 0.6f)
				{
					Left8.set_position(glm::vec2(Left8.get_position().x, Left8.get_position().y + (0.2f * static_cast<float>(deltaTime))));
					Right9.set_position(glm::vec2(Right9.get_position().x, Right9.get_position().y + (0.2f * static_cast<float>(deltaTime))));
					
					if (Left8.get_opacity() < 1.0f)
					{
						Left8.set_opacity(Left8.get_opacity() + 0.3f * static_cast<float>(deltaTime));
					}

					if (Left8.get_opacity() >= 0.8f)
					{
						if (Right9.get_opacity() < 1.0f)
						{
							Right9.set_opacity(Right9.get_opacity() + 0.3f * static_cast<float>(deltaTime));
						}
					}
				}

				if (Right9.get_position().y >= 0.6f)
				{
					FourPiece.set_position(glm::vec2(FourPiece.get_position().x, FourPiece.get_position().y + (0.2f * static_cast<float>(deltaTime))));
					
					if (FourPiece.get_opacity() < 1.0f)
					{
						FourPiece.set_opacity(FourPiece.get_opacity() + 0.3f * static_cast<float>(deltaTime));
					}
				}

				if (FourPiece.get_position().y >= 0.6f)
				{
					Left11.set_position(glm::vec2(Left11.get_position().x, Left11.get_position().y + (0.2f * static_cast<float>(deltaTime))));
					Right12.set_position(glm::vec2(Right12.get_position().x, Right12.get_position().y + (0.2f * static_cast<float>(deltaTime))));

					if (Left11.get_opacity() < 1.0f)
					{
						Left11.set_opacity(Left11.get_opacity() + 0.3f * static_cast<float>(deltaTime));
					}

					if (Left11.get_opacity() >= 0.8f)
					{
						if (Right12.get_opacity() < 1.0f)
						{
							Right12.set_opacity(Right12.get_opacity() + 0.3f * static_cast<float>(deltaTime));
						}
					}

					if (Right12.get_position().y >= 0.0f)
					{
						finishCutscene = true;
					}
				}
			}
		}

		if (finishCutscene) 
		{
			cutsceneTimer += deltaTime;

			if (cutsceneTimer >= timerLimit) 
			{
				g_LevelManager.SetNextLevel("StartingRoom");
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
	}

	void FreeLevel() { /* Empty by design */ }

	void UnloadLevel()
	{
		g_Coordinator.ResetEntities();
		cutsceneTimer = 0.0;
		finishCutscene = false;
		rightAppeared = false;
	}
};
