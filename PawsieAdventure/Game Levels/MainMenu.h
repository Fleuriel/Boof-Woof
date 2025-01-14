#pragma once

#include "Level Manager/Level.h"
#include "ECS/Coordinator.hpp"
#include "../BoofWoof/Core/AssetManager/FilePaths.h"

Entity BackCamera{};  // Entity for the back camera
Entity MenuMusic{}, MenuMusic1;




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

		// Play background music
		//g_Audio.PlayBGM(FILEPATH_ASSET_AUDIO+"/mainmenu music.wav");

		// Find the "BackCamera" entity
		std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();
		for (auto entity : entities)
		{
			if (g_Coordinator.HaveComponent<MetadataComponent>(entity))
			{
				auto& metadata = g_Coordinator.GetComponent<MetadataComponent>(entity);
				if (metadata.GetName() == "BackCamera")
				{
					BackCamera = entity;
				}
				else if (metadata.GetName() == "Music")
				{
					MenuMusic = entity;
					
				}
				else if (metadata.GetName() == "Music1")
				{
					MenuMusic1 = entity;
					break;
				}

				
			}
		}

		
		// Ensure the AudioComponent is linked to the AudioSystem
			auto& music = g_Coordinator.GetComponent<AudioComponent>(MenuMusic);

			// Set the AudioSystem for the AudioComponent
			music.SetAudioSystem(&g_Audio); // Or pass another instance of AudioSystem if preferred

			// Play the audio directly from the component
			
			 music.PlayAudio();
		
		

		

			auto& music1 = g_Coordinator.GetComponent<AudioComponent>(MenuMusic1);
			music1.SetAudioSystem(&g_Audio); // Or pass another instance of AudioSystem if preferred
			

			music1.PlayAudio();
		

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
				if (g_Coordinator.HaveComponent<AudioComponent>(MenuMusic))
				{
					auto& music = g_Coordinator.GetComponent<AudioComponent>(MenuMusic);
					music.StopAudio();
				}
				// Play the button click sound
				g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO+"/(MenuButtonClick).wav", false, "SFX");

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

		if (g_Input.GetKeyState(GLFW_KEY_1) >= 1) {
			if (g_Coordinator.HaveComponent<AudioComponent>(MenuMusic)) {
				auto& music = g_Coordinator.GetComponent<AudioComponent>(MenuMusic);

				// Decrease volume
				music.SetVolume(0.5f);
				std::cout << "Decreased volume for MenuMusic to: "  << std::endl;
			}
		}

		if (g_Input.GetKeyState(GLFW_KEY_2) >= 1) {
			if (g_Coordinator.HaveComponent<AudioComponent>(MenuMusic)) {
				auto& music = g_Coordinator.GetComponent<AudioComponent>(MenuMusic);
				music.StopAudio(); // Stop specific entity audio
				std::cout << "Stopped audio for MenuMusic." << std::endl;
			}
		}

		if (g_Input.GetKeyState(GLFW_KEY_3) >= 1) {
			if (g_Coordinator.HaveComponent<AudioComponent>(MenuMusic)) {
				auto& music = g_Coordinator.GetComponent<AudioComponent>(MenuMusic);
				music.PlayAudio(); // Stop specific entity audio
				std::cout << "paly audio for MenuMusic." << std::endl;
			}
		}
	}

	void FreeLevel() { /* Empty by design */ }

	void UnloadLevel()
	{
		if (g_Coordinator.HaveComponent<AudioComponent>(MenuMusic))
		{
			auto& music = g_Coordinator.GetComponent<AudioComponent>(MenuMusic);
			music.StopAudio();
		}

		// Check if MenuMusic1 has an AudioComponent before accessing it
		if (g_Coordinator.HaveComponent<AudioComponent>(MenuMusic1))
		{
			auto& music1 = g_Coordinator.GetComponent<AudioComponent>(MenuMusic1);
			music1.StopAudio();
		}
		g_Audio.StopBGM();
		// Reset all entities
		g_Coordinator.ResetEntities();
		MenuelapsedTime = 0.0;
		spacePressed = false;


	}
};
