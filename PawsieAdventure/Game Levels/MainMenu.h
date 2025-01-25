#pragma once

#include "Level Manager/Level.h"
#include "ECS/Coordinator.hpp"
#include "../BoofWoof/Core/AssetManager/FilePaths.h"

Entity BackCamera{}, MenuMusic{}, MenuClick{};  // Entity for the back camera

double MenuelapsedTime = 0.0;  // Tracks the elapsed time
double delayAfterSpace = 0.5;  // Set the delay to 1 second
bool spacePressed = false;  // Tracks whether the space bar has been pressed

class MainMenu : public Level
{
	void LoadLevel()
	{
		// Load the main menu scenes
		g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES+"/MainMenuBack.json");
		g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES+"/MainMenuFront.json");


		// Find the "BackCamera" entity
		std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();

		// Use unordered_map to make it O(1) efficiency
		std::unordered_map<std::string, std::function<void(Entity)>> nameToAction =
		{
			{"BackCamera", [&](Entity entity) { BackCamera = entity; }},
			{"MainMenuBGM", [&](Entity entity) { MenuMusic = entity; }},
			{"MenuButtonClick", [&](Entity entity) { MenuClick = entity; }}
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

				if (g_Coordinator.HaveComponent<AudioComponent>(entity))
				{
					auto& music = g_Coordinator.GetComponent<AudioComponent>(entity);
					music.SetAudioSystem(&g_Audio);

					if (metadata.GetName() == "MainMenuBGM") 
					{
						music.PlayAudio();
					}
				}

				// Exit early if all entities are found
				if (BackCamera && MenuMusic && MenuClick)
				{
					break;
				}
			}
		}
	}

	void InitLevel() { /* Empty by design */ }

	void UpdateLevel(double deltaTime)
	{
		// Get the current yaw value for the camera
		float currentYaw = g_Coordinator.GetComponent<CameraComponent>(BackCamera).GetCameraYaw();

		// Increment the yaw by deltaTime (adjust speed as needed)
		currentYaw += static_cast<float>(deltaTime) * 10.0f; // Adjust speed multiplier if needed

		// Reset yaw if it exceeds 270 degrees
		if (currentYaw >= 270.0f)
		{
			currentYaw = -90.0f;
		}

		// Update the yaw value in the camera component
		g_Coordinator.GetComponent<CameraComponent>(BackCamera).SetCameraYaw(currentYaw);

		if (!spacePressed)
		{
			if (g_Input.GetKeyState(GLFW_KEY_SPACE) >= 1)
			{
				// Play the button click sound
				if (g_Coordinator.HaveComponent<AudioComponent>(MenuClick)) {
					auto& music1 = g_Coordinator.GetComponent<AudioComponent>(MenuClick);
					music1.PlayAudio();
				}
				
				// Mark space as pressed and reset elapsed time
				spacePressed = true;
				MenuelapsedTime = 0.0;
				g_Window->HideMouseCursor();
			}
		}
		else
		{
			MenuelapsedTime += deltaTime;
			if (MenuelapsedTime >= delayAfterSpace)
			{
				g_LevelManager.SetNextLevel("Cutscene");
			}
		}
	}

	void FreeLevel() { /* Empty by design */ }

	void UnloadLevel()
	{
		/*if (g_Coordinator.HaveComponent<AudioComponent>(MenuMusic)) {
			auto& music = g_Coordinator.GetComponent<AudioComponent>(MenuMusic);
			music.StopAudio();
		}*/

		g_Audio.Stop(MenuMusic);

		// Reset all entities
		g_Coordinator.ResetEntities();
		MenuelapsedTime = 0.0;
		spacePressed = false;
	}
};
