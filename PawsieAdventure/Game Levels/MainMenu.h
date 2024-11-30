#pragma once

#include "Level Manager/Level.h"
#include "ECS/Coordinator.hpp"


Entity BackCamera{};  // Entity for the back camera

double MenuelapsedTime = 0.0;  // Tracks the elapsed time
double delayAfterSpace = 0.5;  // Set the delay to 1 second
bool spacePressed = false;  // Tracks whether the space bar has been pressed

class MainMenu : public Level
{
	void LoadLevel()
	{
		// Load the main menu scenes
		g_SceneManager.LoadScene("../BoofWoof/Assets/Scenes/MainMenuBack.json");
		g_SceneManager.LoadScene("../BoofWoof/Assets/Scenes/MainMenuFront.json");

		// Play background music
		g_Audio.PlayBGM("../BoofWoof/Assets/Audio/mainmenu music.wav"); 

		// Find the "BackCamera" entity
		std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();
		for (auto entity : entities)
		{
			if (g_Coordinator.HaveComponent<MetadataComponent>(entity))
			{
				if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "BackCamera")
				{
					BackCamera = entity;
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
				//g_Audio.StopBGM();

				// Play the button click sound
				g_Audio.PlayFileOnNewChannel("../BoofWoof/Assets/Audio/(MenuButtonClick).wav", false);

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
		g_Audio.StopBGM();
		// Reset all entities
		g_Coordinator.ResetEntities();
		MenuelapsedTime = 0.0;
		spacePressed = false;
	}
};
