#pragma once
#include "Level Manager/Level.h"
#include "ResourceManager/ResourceManager.h"
#include "ECS/Coordinator.hpp"

double cutsceneTimer = 0.0;
double timerLimit = 4.0;    // Set the time limit to 3 seconds
int textureIndex = 0;  // To track which texture we're currently showing
double lastBarkTime = 0.0;
Entity TextEnt{};

class Cutscene : public Level
{
	void LoadLevel()
	{
		g_SceneManager.LoadScene("../BoofWoof/Assets/Scenes/Cutscene.json");

		std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();

		for (auto entity : entities)
		{
			if (g_Coordinator.HaveComponent<MetadataComponent>(entity))
			{
				if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Text")
				{
					TextEnt = entity;
					break;
				}
			}
		}
	}

	void InitLevel() { /* Empty by design */ }

	void UpdateLevel(double deltaTime)
	{
		cutsceneTimer += deltaTime;		
		std::cout << "cutsceneTimer: " << cutsceneTimer << std::endl;

		if (!g_Coordinator.HaveComponent<GraphicsComponent>(TextEnt)) return;

		auto &text = g_Coordinator.GetComponent<GraphicsComponent>(TextEnt);
		std::string textureName = text.getTextureName();

		// If it's time to change the texture
		if (cutsceneTimer >= timerLimit && textureIndex < 4) // Only switch to CorgiText1, CorgiText2, CorgiText3, CorgiText4
		{
			std::string newTextureName = "CorgiText" + std::to_string(textureIndex + 1);

			int oldTextureId = g_ResourceManager.GetTextureDDS(textureName);	
			if (text.RemoveTexture(oldTextureId))
			{
				// remove old texture & add new one
				text.clearTextures();

				int textureId = g_ResourceManager.GetTextureDDS(newTextureName);	// getTextureID from name
				text.AddTexture(textureId);
				text.setTexture(newTextureName);
			}

			cutsceneTimer = 0.0;  // Reset the timer after each texture change
			textureIndex++;  // Move to the next texture
		}

		switch (textureIndex) 
		{
			case 0:
			{
				// Big dog telling you stay in the room
				g_Audio.PlayFile("../BoofWoof/Assets/Audio/AggressiveDogBarking.wav");
				break;
			}
			case 1:
			{
				// I'm scared
				if (cutsceneTimer <= 3.0) 
				{
					g_Audio.PlayFile("../BoofWoof/Assets/Audio/CorgiWhimper.wav");
				}
				break;
			}
			case 2:
			{
				// I can't keep living like this
				// I want to leave this place
				// No.. I WILL LEAVE THE CASTLE

				g_Audio.PlayFile("../BooFwoof/Assets/Audio/12sGrowlBarkCorgi.wav");
				
				break;
			}
			//case 3:
			//{
			//	// I want to leave this place
			//	if (static_cast<int>(cutsceneTimer) % 2 == 0)  
			//	{
			//		g_Audio.PlayFile("../BooFwoof/Assets/Audio/CorgiBark2.wav");
			//	}
			//	break;
			//}
			//case 4:
			//{
			//	if (static_cast<int>(cutsceneTimer) % 2 == 0)
			//	{
			//		g_Audio.PlayFile("../BooFwoof/Assets/Audio/CorgiBark1.wav");
			//	}
			//	break;
			//}
		}


		// After showing CorgiText4 for 3 seconds, transition to the next scene
		if (textureIndex >= 4 && cutsceneTimer >= timerLimit)
		{
			g_LevelManager.SetNextLevel("Level1");  // Transition to the next level after CorgiText4
		}
	}

	void FreeLevel() { /*Empty by design*/ }

	void UnloadLevel()
	{
		g_Coordinator.ResetEntities();
		cutsceneTimer = 0.0;
		textureIndex = 0;  
	}
};