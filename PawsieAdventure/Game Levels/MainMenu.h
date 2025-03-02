#pragma once

#include "Level Manager/Level.h"
#include "ECS/Coordinator.hpp"
#include "../BoofWoof/Core/AssetManager/FilePaths.h"

<<<<<<< HEAD
Entity BackCamera{};  // Entity for the back camera

double MenuelapsedTime = 0.0;  // Tracks the elapsed time
double delayAfterSpace = 0.5;  // Set the delay to 1 second
bool spacePressed = false;  // Tracks whether the space bar has been pressed
=======
Entity BackCamera{}, MenuMusic{}, MenuClick{}, StartGame{}, X{}, HTP{}, Cog{};
std::unique_ptr<PauseMenu> MenuPauser = CreatePausedMenu(PauseState::Paused);
float sfxVolume{ 1.0f }, bgmVolume{ 1.0f };
bool inSmth{ false };
std::unordered_map<Entity, glm::vec2> originalScales;
extern std::shared_ptr<GraphicsSystem> mGraphicsSys;
>>>>>>> main

class MainMenu : public Level
{
	void LoadLevel()
	{
		// Load the main menu scenes
		g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES+"/MainMenuBack.json");
		g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES+"/MainMenuFront.json");

		// Play background music
		g_Audio.PlayBGM(FILEPATH_ASSET_AUDIO+"/mainmenu music.wav");

		// Find the "BackCamera" entity
		std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();
		for (auto entity : entities)
		{
			if (g_Coordinator.HaveComponent<MetadataComponent>(entity))
			{
<<<<<<< HEAD
				if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "BackCamera")
=======
				const auto& metadata = g_Coordinator.GetComponent<MetadataComponent>(entity);
				auto it = nameToAction.find(metadata.GetName());

				if (it != nameToAction.end())
				{
					it->second(entity);

					if (g_Coordinator.HaveComponent<UIComponent>(entity))
					{
						// Store original scale before hiding
						auto& transform = g_Coordinator.GetComponent<UIComponent>(entity);
						if (originalScales.find(entity) == originalScales.end())
						{
							originalScales[entity] = transform.get_scale();
						}
					}
				}	

				if (g_Coordinator.HaveComponent<AudioComponent>(entity))
				{
					auto& music = g_Coordinator.GetComponent<AudioComponent>(entity);
					music.SetAudioSystem(&g_Audio);
				}

				// Exit early if all entities are found
				if (BackCamera && MenuMusic && MenuClick && StartGame && HTP && Cog && X)
>>>>>>> main
				{
					BackCamera = entity;
					break;
				}
			}
		}
	}

<<<<<<< HEAD
	void InitLevel() { /* Empty by design */ }

	void UpdateLevel(double deltaTime)
=======
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
>>>>>>> main
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
<<<<<<< HEAD
				//g_Audio.StopBGM();

				// Play the button click sound
				g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO+"/(MenuButtonClick).wav", false);
=======
				// need to add in audio feedback for pressing ESC
				g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/EscSFX.wav", false, "SFX");
				RestoreUI();
>>>>>>> main

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
<<<<<<< HEAD
=======

			if (g_Coordinator.HaveComponent<UIComponent>(HTP))
			{
				auto& UICompt = g_Coordinator.GetComponent<UIComponent>(HTP);
				if (UICompt.get_selected())
				{
					inSmth = true;

					g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/HowToPlaySFX.wav", false, "SFX");
					HideUI();
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

					HideUI();

					bgmVolume = g_Audio.GetBGMVolume();
					sfxVolume = g_Audio.GetSFXVolume();

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

					HideUI();

					MenuPauser = CreatePausedMenu(PauseState::QuitGame);
					MenuPauser->OnLoad();
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
				FontComponent& SFXFont = g_Coordinator.GetComponent<FontComponent>(MenuPauser->SFXVol);
				if (volDisplay >= 0 && volDisplay < 10)
				{
					std::stringstream ss;
					ss << std::setfill('0') << std::setw(2) << volDisplay;
					std::string text = ss.str();
					SFXFont.set_pos(glm::vec2(0.12f, 0.35f));
					SFXFont.set_text(text);
				}
				else
				{
					SFXFont.set_pos(glm::vec2(0.14f, 0.35f));
					SFXFont.set_text("10");
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
				FontComponent& BGMFont = g_Coordinator.GetComponent<FontComponent>(MenuPauser->BGMVol);
				if (volDisplay >= 0 && volDisplay < 10)
				{
					std::stringstream ss;
					ss << std::setfill('0') << std::setw(2) << volDisplay;
					std::string text = ss.str();
					BGMFont.set_pos(glm::vec2(0.12f, -0.05f));
					BGMFont.set_text(text);
				}
				else
				{
					BGMFont.set_pos(glm::vec2(0.14f, -0.05f));
					BGMFont.set_text("10");
				}
			}

			if (g_Coordinator.HaveComponent<UIComponent>(MenuPauser->GAMMALeft))
			{
				auto& UICompt = g_Coordinator.GetComponent<UIComponent>(MenuPauser->GAMMALeft);
				if (UICompt.get_selected())
				{
					mGraphicsSys->gammaValue -= 0.1f;
					if (mGraphicsSys->gammaValue < 1.f)
						mGraphicsSys->gammaValue = 1.f;
				}
			}

			if (g_Coordinator.HaveComponent<UIComponent>(MenuPauser->GAMMARight))
			{
				auto& UICompt = g_Coordinator.GetComponent<UIComponent>(MenuPauser->GAMMARight);
				if (UICompt.get_selected())
				{
					mGraphicsSys->gammaValue += 0.1f;
					if (mGraphicsSys->gammaValue > 3.f)
						mGraphicsSys->gammaValue = 3.f;
				}
			}

			if (g_Coordinator.HaveComponent<FontComponent>(MenuPauser->GAMMAValue))
			{
				std::stringstream ss;
				ss << std::fixed << std::setprecision(1) << mGraphicsSys->gammaValue; // Format to 1 decimal places
				std::string str = ss.str();

				FontComponent& gammaFont = g_Coordinator.GetComponent<FontComponent>(MenuPauser->GAMMAValue);
				gammaFont.set_pos(glm::vec2(0.12f, -0.45f));
				gammaFont.set_text(str);
			}

			if (g_Coordinator.HaveComponent<UIComponent>(MenuPauser->YesBtn))
			{
				auto& UICompt = g_Coordinator.GetComponent<UIComponent>(MenuPauser->YesBtn);
				if (UICompt.get_selected())
				{
					g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/(MenuButtonClick).wav", false, "SFX");
					exit(0);
				}
			}

			if (g_Coordinator.HaveComponent<UIComponent>(MenuPauser->NoBtn))
			{
				auto& UICompt = g_Coordinator.GetComponent<UIComponent>(MenuPauser->NoBtn);
				if (UICompt.get_selected())
				{
					g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/(MenuButtonClick).wav", false, "SFX");
					RestoreUI();

					inSmth = false;
					MenuPauser->OnExit();
				}
			}
		}
		else if (g_Input.GetMouseState(GLFW_MOUSE_BUTTON_LEFT) == 0)
		{
			// Reset the mouse press state when the mouse button is released
			wasMousePressed = false;
>>>>>>> main
		}
	}

	void FreeLevel() { /* Empty by design */ }

	void UnloadLevel()
	{
<<<<<<< HEAD
		g_Audio.StopBGM();
=======
		g_Audio.Stop(MenuMusic);

>>>>>>> main
		// Reset all entities
		g_Coordinator.ResetEntities();
		MenuelapsedTime = 0.0;
		spacePressed = false;
	}

	void HideUI()
	{
		for (auto& entry : originalScales)
		{
			Entity entity = entry.first;

			if (g_Coordinator.HaveComponent<UIComponent>(entity))
			{
				auto& transform = g_Coordinator.GetComponent<UIComponent>(entity);
				transform.set_scale(glm::vec2(0,0));
			}
		}
	}

	void RestoreUI() 
	{
		for (auto& entry : originalScales)
		{
			Entity entity = entry.first;
			glm::vec2 originalScale = entry.second;

			if (g_Coordinator.HaveComponent<UIComponent>(entity))
			{
				auto& transform = g_Coordinator.GetComponent<UIComponent>(entity);
				transform.set_scale(originalScale);
			}
		}
	}
};
