#pragma once

#include "Level Manager/Level.h"
#include "ECS/Coordinator.hpp"
#include "../BoofWoof/Core/AssetManager/FilePaths.h"
#include "../Systems/PauseScreen/PauseScreen.h"

Entity BackCamera{}, MenuMusic{}, MenuClick{}, StartGame{}, X{}, HTP{}, Cog{};
std::unique_ptr<PauseMenu> MenuPauser = CreatePausedMenu(PauseState::Paused);
float sfxVolume{ 1.0f }, bgmVolume{ 1.0f };
bool inSmth{ false };

class MainMenu : public Level
{
	void LoadLevel() override
	{
		// Load the main menu scenes
		g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES + "/MainMenuBack.json");
		g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES + "/MainMenuFront.json");

		// Find the "BackCamera" entity
		std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();

		// Use unordered_map to make it O(1) efficiency
		std::unordered_map<std::string, std::function<void(Entity)>> nameToAction =
		{
			{"BackCamera", [&](Entity entity) { BackCamera = entity; }},
			{"MainMenuBGM", [&](Entity entity) { MenuMusic = entity; }},
			{"MenuButtonClick", [&](Entity entity) { MenuClick = entity; }},
			{ "StartGame", [&](Entity entity) { StartGame = entity; } },
			{ "HTP", [&](Entity entity) { HTP = entity; } },
			{ "Cog", [&](Entity entity) { Cog = entity; } },
			{ "X", [&](Entity entity) { X = entity; } }
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
				}

				// Exit early if all entities are found
				if (BackCamera && MenuMusic && MenuClick && StartGame && HTP && Cog && X)
				{
					break;
				}
			}
		}
	}

	void InitLevel() override
	{
		auto& music = g_Coordinator.GetComponent<AudioComponent>(MenuMusic);
		music.SetAudioSystem(&g_Audio);

		music.PlayAudio();

		if (g_Coordinator.HaveComponent<AudioComponent>(MenuClick)) {
			auto& music1 = g_Coordinator.GetComponent<AudioComponent>(MenuClick);
			music1.SetAudioSystem(&g_Audio);
		}
	}

	void UpdateLevel(double deltaTime) override
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

		if (g_Input.GetKeyState(GLFW_KEY_ESCAPE) >= 1)
		{
			if (inSmth)
			{
				// need to add in audio feedback for pressing ESC
				g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/EscSFX.wav", false, "SFX");


				inSmth = false;
				MenuPauser->OnExit();
			}
		}

		if (g_Input.GetMouseState(GLFW_MOUSE_BUTTON_LEFT) == 1 && !inSmth)
		{
			if (g_Coordinator.HaveComponent<UIComponent>(StartGame))
			{
				auto& UICompt = g_Coordinator.GetComponent<UIComponent>(StartGame);
				if (UICompt.get_selected())
				{
					inSmth = true;

					// Play the button click sound
					if (g_Coordinator.HaveComponent<AudioComponent>(MenuClick)) {
						auto& music1 = g_Coordinator.GetComponent<AudioComponent>(MenuClick);
						music1.PlayAudio();
					}

					g_Window->HideMouseCursor();
					g_LevelManager.SetNextLevel("Cutscene");
				}
			}

			if (g_Coordinator.HaveComponent<UIComponent>(HTP))
			{
				auto& UICompt = g_Coordinator.GetComponent<UIComponent>(HTP);
				if (UICompt.get_selected())
				{
					inSmth = true;

					g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/HowToPlaySFX.wav", false, "SFX");
					MenuPauser = CreatePausedMenu(PauseState::HowToPlay);
					MenuPauser->OnLoad();
				}
			}

			if (g_Coordinator.HaveComponent<UIComponent>(Cog))
			{
				auto& UICompt = g_Coordinator.GetComponent<UIComponent>(Cog);
				if (UICompt.get_selected())
				{
					inSmth = true;

					// Play the button click sound
					if (g_Coordinator.HaveComponent<AudioComponent>(MenuClick)) {
						auto& music1 = g_Coordinator.GetComponent<AudioComponent>(MenuClick);
						music1.PlayAudio();
					}
					MenuPauser = CreatePausedMenu(PauseState::Settings);
					MenuPauser->OnLoad();
				}
			}

			if (g_Coordinator.HaveComponent<UIComponent>(X))
			{
				auto& UICompt = g_Coordinator.GetComponent<UIComponent>(X);
				if (UICompt.get_selected())
				{
					inSmth = true;

					// Play the button click sound
					if (g_Coordinator.HaveComponent<AudioComponent>(MenuClick)) {
						auto& music1 = g_Coordinator.GetComponent<AudioComponent>(MenuClick);
						music1.PlayAudio();
					}

					MenuPauser->OnExit();
					exit(0);
				}
			}
		}

		static bool wasMousePressed = false;

		if (g_Input.GetMouseState(GLFW_MOUSE_BUTTON_LEFT) == 1 && !wasMousePressed && inSmth)
		{
			wasMousePressed = true;
			const float volumeStep = 0.1f; // Step size for volume change

			// For Settings Page
			if (g_Coordinator.HaveComponent<UIComponent>(MenuPauser->SFXLeft))
			{
				auto& UICompt = g_Coordinator.GetComponent<UIComponent>(MenuPauser->SFXLeft);
				if (UICompt.get_selected())
				{
					sfxVolume = std::max(0.0f, (float)(g_Audio.GetSFXVolume() - volumeStep));
					g_Audio.SetSFXVolume(sfxVolume);
					g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/(MenuButtonClick).wav", false, "SFX");
				}
			}

			if (g_Coordinator.HaveComponent<UIComponent>(MenuPauser->SFXRight))
			{
				auto& UICompt = g_Coordinator.GetComponent<UIComponent>(MenuPauser->SFXRight);
				if (UICompt.get_selected())
				{
					sfxVolume = std::min(1.0f, (float)(g_Audio.GetSFXVolume() + volumeStep));
					g_Audio.SetSFXVolume(sfxVolume);
					g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/(MenuButtonClick).wav", false, "SFX");
				}
			}

			if (g_Coordinator.HaveComponent<FontComponent>(MenuPauser->SFXVol))
			{
				int volDisplay = static_cast<int>(std::round(sfxVolume * 10));
				if (volDisplay >= 0 && volDisplay < 10)
				{
					std::stringstream ss;
					ss << std::setfill('0') << std::setw(2) << volDisplay;
					std::string text = ss.str();
					g_Coordinator.GetComponent<FontComponent>(MenuPauser->SFXVol).set_pos(glm::vec2(0.12f, 0.25f));
					g_Coordinator.GetComponent<FontComponent>(MenuPauser->SFXVol).set_text(text);
				}
				else
				{
					g_Coordinator.GetComponent<FontComponent>(MenuPauser->SFXVol).set_pos(glm::vec2(0.14f, 0.25f));
					g_Coordinator.GetComponent<FontComponent>(MenuPauser->SFXVol).set_text("10");
				}
			}

			if (g_Coordinator.HaveComponent<UIComponent>(MenuPauser->BGMLeft))
			{
				auto& UICompt = g_Coordinator.GetComponent<UIComponent>(MenuPauser->BGMLeft);
				if (UICompt.get_selected())
				{
					bgmVolume = std::max(0.0f, (float)(g_Audio.GetBGMVolume() - volumeStep));
					g_Audio.SetBGMVolume(bgmVolume);
				}
			}

			if (g_Coordinator.HaveComponent<UIComponent>(MenuPauser->BGMRight))
			{
				auto& UICompt = g_Coordinator.GetComponent<UIComponent>(MenuPauser->BGMRight);
				if (UICompt.get_selected())
				{
					bgmVolume = std::min(1.0f, (float)(g_Audio.GetBGMVolume() + volumeStep));
					g_Audio.SetBGMVolume(bgmVolume);
				}
			}

			if (g_Coordinator.HaveComponent<FontComponent>(MenuPauser->BGMVol))
			{
				int volDisplay = static_cast<int>(std::round(bgmVolume * 10));
				if (volDisplay >= 0 && volDisplay < 10)
				{
					std::stringstream ss;
					ss << std::setfill('0') << std::setw(2) << volDisplay;
					std::string text = ss.str();
					g_Coordinator.GetComponent<FontComponent>(MenuPauser->BGMVol).set_pos(glm::vec2(0.12f, -0.25f));
					g_Coordinator.GetComponent<FontComponent>(MenuPauser->BGMVol).set_text(text);
				}
				else
				{
					g_Coordinator.GetComponent<FontComponent>(MenuPauser->BGMVol).set_pos(glm::vec2(0.14f, -0.25f));
					g_Coordinator.GetComponent<FontComponent>(MenuPauser->BGMVol).set_text("10");
				}
			}
		}
		else if (g_Input.GetMouseState(GLFW_MOUSE_BUTTON_LEFT) == 0)
		{
			// Reset the mouse press state when the mouse button is released
			wasMousePressed = false;
		}
	}

	void FreeLevel() override { /* Empty by design */ }

	void UnloadLevel() override
	{
		g_Audio.Stop(MenuMusic);

		// Reset all entities
		g_Coordinator.ResetEntities();

		inSmth = false;
	}
};
