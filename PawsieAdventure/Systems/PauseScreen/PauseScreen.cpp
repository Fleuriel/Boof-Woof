#include "PauseScreen.h"
#include "../Core/AssetManager/FilePaths.h"
#include <Level Manager/LevelManager.h>
#include "../GSM/GameStateMachine.h" // for g_IsPaused

// Reset purposes
#include "../ChangeText/ChangeText.h" 
#include "../Checklist/Checklist.h" 
#include "../RopeBreaker/RopeBreaker.h" 
#include "../BoneCatcher/BoneCatcher.h" 
#include "../../Utilities/ForGame/TimerTR/TimerTR.h"

std::unique_ptr<PauseMenu> pauser = CreatePausedMenu(PauseState::Paused);
Serialization serialPause;
Entity ResumeGame{}, SettingsBtn{}, HTPBtn{}, ExitGame{};
bool inSmthAgain{ false };
float sfVolume{ 1.f }, bgVolume{ 1.f };

extern std::shared_ptr<GraphicsSystem> mGraphicsSys;

void PausedScreen::OnLoad()
{
	g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES + "/PausedScreen.json");
	spawnedEntities = serialPause.GetStored();

	std::unordered_map<std::string, std::function<void(Entity)>> nameToAction =
	{
		{"Resume", [&](Entity entity) { ResumeGame = entity; }},
		{"HTP", [&](Entity entity) { HTPBtn = entity; }},
		{"Settings", [&](Entity entity) { SettingsBtn = entity; }},
		{ "Exit", [&](Entity entity) { ExitGame = entity; } }
	};

	std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();
	for (auto entity : entities)
	{
		const auto& metadata = g_Coordinator.GetComponent<MetadataComponent>(entity);
		auto it = nameToAction.find(metadata.GetName());

		if (it != nameToAction.end())
		{
			it->second(entity);
		}

		// Exit early if all entities are found
		/*if (ResumeGame && HTPBtn && SettingsBtn && ExitGame)
		{
			break;
		}*/
	}
}

void PausedScreen::OnExit()
{
	std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();

	for (auto i = entities.begin(); i != entities.end(); i++)
	{
		for (auto k = spawnedEntities.begin(); k != spawnedEntities.end(); k++)
		{
			if (*k == *i)
			{
				g_Coordinator.DestroyEntity(*i);
			}
		}
	}
}

void QuitScreen::OnLoad()
{
	g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES + "/QuitGame.json");
	spawnedEntities = serialPause.GetStored();

	std::unordered_map<std::string, std::function<void(Entity)>> nameToAction =
	{
		{"YesBtn", [&](Entity entity) { YesBtn = entity; }},
		{"NoBtn", [&](Entity entity) { NoBtn = entity; }}
	};

	std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();
	for (auto entity : entities)
	{
		const auto& metadata = g_Coordinator.GetComponent<MetadataComponent>(entity);
		auto it = nameToAction.find(metadata.GetName());

		if (it != nameToAction.end())
		{
			it->second(entity);
		}

		// Exit early if all entities are found
		if (YesBtn && NoBtn)
		{
			break;
		}
	}
}

void QuitScreen::OnExit()
{
	std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();

	for (auto i = entities.begin(); i != entities.end(); i++)
	{
		for (auto k = spawnedEntities.begin(); k != spawnedEntities.end(); k++)
		{
			if (*k == *i)
			{
				g_Coordinator.DestroyEntity(*i);
			}
		}
	}
}

void Settings::OnLoad()
{
	g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES + "/SettingsScreen.json");
	spawnedEntities = serialPause.GetStored();

	std::unordered_map<std::string, std::function<void(Entity)>> nameToAction =
	{
		{"SFXLeft", [&](Entity entity) { SFXLeft = entity; }},
		{"SFXRight", [&](Entity entity) { SFXRight = entity; }},
		{"BGMLeft", [&](Entity entity) { BGMLeft = entity; }},
		{"BGMRight", [&](Entity entity) { BGMRight = entity; }},
		{"GAMMALeft", [&](Entity entity) { GAMMALeft = entity; }},
		{"GAMMARight", [&](Entity entity) { GAMMARight = entity; }},
		{"SFXVol", [&](Entity entity) { SFXVol = entity; }},
		{"BGMVol", [&](Entity entity) { BGMVol = entity; }},
		{"GAMMAValue", [&](Entity entity) { GAMMAValue = entity; }}
	};

	std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();
	for (auto entity : entities)
	{
		const auto& metadata = g_Coordinator.GetComponent<MetadataComponent>(entity);
		auto it = nameToAction.find(metadata.GetName());

		if (it != nameToAction.end())
		{
			it->second(entity);
		}

		// Exit early if all entities are found
		if (SFXLeft && SFXRight && BGMLeft && BGMRight && SFXVol && BGMVol && GAMMALeft && GAMMARight && GAMMAValue)
		{
			break;
		}
	}
}

void Settings::OnExit()
{
	std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();

	for (auto i = entities.begin(); i != entities.end(); i++)
	{
		for (auto k = spawnedEntities.begin(); k != spawnedEntities.end(); k++)
		{
			if (*k == *i)
			{
				g_Coordinator.DestroyEntity(*i);
			}
		}
	}
}

void HowToPlay::OnLoad()
{
	g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES + "/HTPScreen.json");
	spawnedEntities = serialPause.GetStored();
}

void HowToPlay::OnExit()
{
	std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();

	for (auto i = entities.begin(); i != entities.end(); i++)
	{
		for (auto k = spawnedEntities.begin(); k != spawnedEntities.end(); k++)
		{
			if (*k == *i)
			{
				g_Coordinator.DestroyEntity(*i);
			}
		}
	}
}

std::unique_ptr<PauseMenu> CreatePausedMenu(PauseState state)
{
	switch (state)
	{
	case PauseState::Paused:
		return std::make_unique<PausedScreen>();

	case PauseState::Settings:
		return std::make_unique<Settings>();

	case PauseState::HowToPlay:
		return std::make_unique<HowToPlay>();

	case PauseState::QuitGame:
		return std::make_unique<QuitScreen>();

	default:
		return std::make_unique<PausedScreen>();
	}
}

namespace pauseLogic 
{
	void OnUpdate()
	{
		if (!g_IsPaused && g_Input.GetKeyState(GLFW_KEY_ESCAPE) == 1)
		{
			g_IsPaused = true;
			pauser->OnLoad();
			g_Window->ShowMouseCursor();
		}

		static bool wasMousePressed = false;

		if (g_IsPaused)
		{
			if (g_Input.GetKeyState(GLFW_KEY_ESCAPE) == 1 && inSmthAgain)
			{
				// Add in audio feedback
				g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/PauseMenuButton.wav", false, "SFX");

				pauser->OnExit();
				pauser = CreatePausedMenu(PauseState::Paused);
				pauser->OnLoad();
				inSmthAgain = false;
			}

			if (g_Input.GetMouseState(GLFW_MOUSE_BUTTON_LEFT) == 1 && !inSmthAgain)
			{
				if (g_Coordinator.HaveComponent<UIComponent>(ResumeGame))
				{
					auto& UICompt = g_Coordinator.GetComponent<UIComponent>(ResumeGame);
					if (UICompt.get_selected())
					{
						// Add in audio feedback
						g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/PauseMenuButton.wav", false, "SFX");

						g_Window->HideMouseCursor();
						pauser->OnExit();
						g_IsPaused = false;
					}
				}

				if (g_Coordinator.HaveComponent<UIComponent>(SettingsBtn))
				{
					auto& UICompt = g_Coordinator.GetComponent<UIComponent>(SettingsBtn);
					if (UICompt.get_selected())
					{
						inSmthAgain = true;

						// Add in audio feedback
						g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/PauseMenuButton.wav", false, "SFX");

						pauser->OnExit();
						pauser = CreatePausedMenu(PauseState::Settings);
						pauser->OnLoad();

						bgVolume = g_Audio.GetBGMVolume();
						sfVolume = g_Audio.GetSFXVolume();

						g_Audio.SetBGMVolume(g_Audio.GetBGMVolume());
						g_Audio.SetSFXVolume(g_Audio.GetSFXVolume());
					}
				}

				// How To Play button
				if (g_Coordinator.HaveComponent<UIComponent>(HTPBtn))
				{
					auto& UICompt = g_Coordinator.GetComponent<UIComponent>(HTPBtn);
					if (UICompt.get_selected())
					{
						inSmthAgain = true;

						// Add in audio feedback
						g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/HowToPlaySFX.wav", false, "SFX");

						pauser->OnExit();
						pauser = CreatePausedMenu(PauseState::HowToPlay);
						pauser->OnLoad();
					}
				}

				// Exit game button
				if (g_Coordinator.HaveComponent<UIComponent>(ExitGame))
				{
					auto& UICompt = g_Coordinator.GetComponent<UIComponent>(ExitGame);
					if (UICompt.get_selected())
					{
						inSmthAgain = true;

						// Add in audio feedback
						g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/PauseMenuButton.wav", false, "SFX");

						pauser->OnExit();
						ResetGame();

						g_LevelManager.SetNextLevel("MainMenu");

						/*pauser = CreatePausedMenu(PauseState::QuitGame);
						pauser->OnLoad();*/
					}
				}
			}

			if (g_Input.GetMouseState(GLFW_MOUSE_BUTTON_LEFT) == 1 && !wasMousePressed && inSmthAgain)
			{
				wasMousePressed = true;
				const float volumeStep = 0.1f; // Step size for volume change


				// For Settings Page
				if (g_Coordinator.HaveComponent<UIComponent>(pauser->SFXLeft))
				{
					auto& UICompt = g_Coordinator.GetComponent<UIComponent>(pauser->SFXLeft);
					if (UICompt.get_selected())
					{
						sfVolume = std::max(0.0f, (float)(g_Audio.GetSFXVolume() - volumeStep));
						g_Audio.SetSFXVolume(sfVolume);
						g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/(MenuButtonClick).wav", false, "SFX");

					}
				}

				if (g_Coordinator.HaveComponent<UIComponent>(pauser->SFXRight))
				{
					auto& UICompt = g_Coordinator.GetComponent<UIComponent>(pauser->SFXRight);
					if (UICompt.get_selected())
					{
						sfVolume = std::min(1.0f, (float)(g_Audio.GetSFXVolume() + volumeStep));
						g_Audio.SetSFXVolume(sfVolume);
						g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/(MenuButtonClick).wav", false, "SFX");
					}
				}

				if (g_Coordinator.HaveComponent<FontComponent>(pauser->SFXVol))
				{
					int volDisplay = static_cast<int>(std::round(sfVolume * 10));
					FontComponent& SFXFont = g_Coordinator.GetComponent<FontComponent>(pauser->SFXVol);
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

				if (g_Coordinator.HaveComponent<UIComponent>(pauser->BGMLeft))
				{
					auto& UICompt = g_Coordinator.GetComponent<UIComponent>(pauser->BGMLeft);
					if (UICompt.get_selected())
					{
						bgVolume = std::max(0.0f, (float)(g_Audio.GetBGMVolume() - volumeStep));
						g_Audio.SetBGMVolume(bgVolume);
					}
				}

				if (g_Coordinator.HaveComponent<UIComponent>(pauser->BGMRight))
				{
					auto& UICompt = g_Coordinator.GetComponent<UIComponent>(pauser->BGMRight);
					if (UICompt.get_selected())
					{
						bgVolume = std::min(1.0f, (float)(g_Audio.GetBGMVolume() + volumeStep));
						g_Audio.SetBGMVolume(bgVolume);
					}
				}

				if (g_Coordinator.HaveComponent<FontComponent>(pauser->BGMVol))
				{
					int volDisplay = static_cast<int>(std::round(bgVolume * 10));
					FontComponent& BGMFont = g_Coordinator.GetComponent<FontComponent>(pauser->BGMVol);
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

				if (g_Coordinator.HaveComponent<UIComponent>(pauser->GAMMALeft))
				{
					auto& UICompt = g_Coordinator.GetComponent<UIComponent>(pauser->GAMMALeft);
					if (UICompt.get_selected())
					{
						mGraphicsSys->gammaValue -= 0.1f;
						if (mGraphicsSys->gammaValue < 1.f)
							mGraphicsSys->gammaValue = 1.f;
					}
				}

				if (g_Coordinator.HaveComponent<UIComponent>(pauser->GAMMARight))
				{
					auto& UICompt = g_Coordinator.GetComponent<UIComponent>(pauser->GAMMARight);
					if (UICompt.get_selected())
					{
						mGraphicsSys->gammaValue += 0.1f;
						if (mGraphicsSys->gammaValue > 3.f)
							mGraphicsSys->gammaValue = 3.f;
					}
				}

				if (g_Coordinator.HaveComponent<FontComponent>(pauser->GAMMAValue))
				{
					std::stringstream ss;
					ss << std::fixed << std::setprecision(1) << mGraphicsSys->gammaValue; // Format to 1 decimal places
					std::string str = ss.str();

					FontComponent& gammaFont = g_Coordinator.GetComponent<FontComponent>(pauser->GAMMAValue);
					gammaFont.set_pos(glm::vec2(0.12f, -0.45f));
					gammaFont.set_text(str);
				}
			}
			else if (g_Input.GetMouseState(GLFW_MOUSE_BUTTON_LEFT) == 0)
			{
				// Reset the mouse press state when the mouse button is released
				wasMousePressed = false;
			}
		}
	}
	void ResetGame()
	{
		g_IsPaused = false;
		inSmthAgain = false;
		g_ChangeText.Reset();
		g_Checklist.Reset();
		g_RopeBreaker.ResetRB();
		g_BoneCatcher.ResetBC();
		g_TimerTR.Reset();
	}
}