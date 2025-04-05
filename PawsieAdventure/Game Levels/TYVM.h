#pragma once
#include "Level Manager/Level.h"
#include "ECS/Coordinator.hpp"
#include "../BoofWoof/Core/AssetManager/FilePaths.h"

class TYVM : public Level
{
	double elapsedTime = 0.0;
	double timeLimit = 10.0;    // Set the time limit to 10 seconds
	Entity exitEntitytext{}, creditspng{};
	bool reached{ false }, setText{ false };

	void LoadLevel() override
	{
		// Use FILEPATH_ASSET_SCENES to construct the scene file path
		g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES + "/TYVM.json");
		g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/ThankYouForPlaying.wav", false, "SFX");


		std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();

		for (auto entity : entities)
		{
			if (g_Coordinator.HaveComponent<MetadataComponent>(entity))
			{
				const auto& metadata = g_Coordinator.GetComponent<MetadataComponent>(entity);
				if (metadata.GetName() == "Exit")
				{
					exitEntitytext = entity;
				}

				if (metadata.GetName() == "TYVM")
				{
					creditspng = entity;
				}
			}
		}
	}

	void InitLevel() override { /* Empty by design */ }

	void UpdateLevel(double deltaTime) override
	{
		if (g_Coordinator.HaveComponent<UIComponent>(creditspng))
		{
			auto& credit = g_Coordinator.GetComponent<UIComponent>(creditspng);

			if (!reached) 
			{
				if (credit.get_position().y < 4.0f)
				{
					credit.set_position(glm::vec2(credit.get_position().x, credit.get_position().y + (0.2f * static_cast<float>(deltaTime))));
				}
				else if (credit.get_position().y >= 4.0f)
				{
					credit.set_position(glm::vec2(credit.get_position().x, 4.0f));
					reached = true;
				}
			}		
		}

		if (reached && !setText)
		{
			if (g_Coordinator.HaveComponent<FontComponent>(exitEntitytext)) 
			{
				auto& font = g_Coordinator.GetComponent<FontComponent>(exitEntitytext);
				font.set_text("ESC to CLOSE");
				setText = true;
			}
		}

		if (reached) 
		{
			elapsedTime += deltaTime;
		}

		if (elapsedTime >= timeLimit)
		{
			g_LevelManager.SetNextLevel("MainMenu");
			g_Window->ShowMouseCursor();
		}

		if (setText) 
		{
			if (g_Input.GetKeyState(GLFW_KEY_ESCAPE) == 1)
			{
				// Thank you scene
				g_LevelManager.SetNextLevel("MainMenu");
				g_Window->ShowMouseCursor();
			}
		}	
	}

	void FreeLevel() override { /* Empty by design */ }

	void UnloadLevel() override
	{
		g_Coordinator.ResetEntities();
		elapsedTime = 0.0;
	}
};
