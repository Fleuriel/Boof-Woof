#pragma once

#include "Level Manager/Level.h"
#include "ECS/Coordinator.hpp"

Entity BackCamera{};

class MainMenu : public Level
{
	void LoadLevel()
	{
		g_SceneManager.LoadScene("../BoofWoof/Assets/Scenes/MainMenuBack.json");
		g_SceneManager.LoadScene("../BoofWoof/Assets/Scenes/MainMenuFront.json");
		//g_Audio.PlayBGM("../BoofWoof/Assets/Audio/Test.wav");

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

		// Get the current yaw value
		float currentYaw = g_Coordinator.GetComponent<CameraComponent>(BackCamera).GetCameraYaw();

		// Increment the yaw by deltaTime (adjust speed as needed)
		currentYaw += static_cast<float>(deltaTime) * 10.0f; // 10.0f is the speed multiplier, adjust as needed

		// Check if yaw has reached or passed 270 degrees, and reset to -90 if true
		if (currentYaw >= 270.0f)
		{
			currentYaw = -90.0f;
		}

		// Set the updated yaw value
		g_Coordinator.GetComponent<CameraComponent>(BackCamera).SetCameraYaw(currentYaw);

		// Press space to start - now just go back splashscreen
		if (g_Input.GetKeyState(GLFW_KEY_SPACE) >= 1)
		{
			g_LevelManager.SetNextLevel("Splashscreen");
		}
	}

	void FreeLevel() { /*Empty by design*/ }

	void UnloadLevel()
	{
		//g_Audio.StopBGM();
		g_Coordinator.ResetEntities();
	}
};