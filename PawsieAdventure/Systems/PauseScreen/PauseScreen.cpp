#include "PauseScreen.h"
#include "../Core/AssetManager/FilePaths.h"
#include <Level Manager/LevelManager.h>
#include "../GSM/GameStateMachine.h" // for g_IsPaused

// Reset purposes
#include "../ChangeText/ChangeText.h" 
#include "../Checklist/Checklist.h" 
#include "../RopeBreaker/RopeBreaker.h" 
#include "../BoneCatcher/BoneCatcher.h" 

std::unique_ptr<PauseMenu> pauser = CreatePausedMenu(PauseState::Paused);
Serialization serialPause;
Entity ResumeGame{}, SettingsBtn{}, HTPBtn{}, ExitGame{};
bool inSmthAgain{ false };

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
		if (ResumeGame && HTPBtn && SettingsBtn && ExitGame)
		{
			break;
		}
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
	// might need to separate the setting json based 
	// on whether it's pausedscreen's or mainmenu's ?

	g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES + "/QuitGame.json");
	spawnedEntities = serialPause.GetStored();

	std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();
	for (auto entity : entities)
	{
		if (g_Coordinator.HaveComponent<MetadataComponent>(entity))
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
	// might need to separate the setting json based 
	// on whether it's pausedscreen's or mainmenu's ?

	g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES + "/SettingsScreen.json");
	spawnedEntities = serialPause.GetStored();

	std::unordered_map<std::string, std::function<void(Entity)>> nameToAction =
	{
		{"SFXLeft", [&](Entity entity) { SFXLeft = entity; }},
		{"SFXRight", [&](Entity entity) { SFXRight = entity; }},
		{"BGMLeft", [&](Entity entity) { BGMLeft = entity; }},
		{ "BGMRight", [&](Entity entity) { BGMRight = entity; } }
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
		if (SFXLeft && SFXRight && BGMLeft && BGMRight)
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
	// might need to separate the setting json based 
	// on whether it's pausedscreen's or mainmenu's ?

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
		if (!g_IsPaused && g_Input.GetKeyState(GLFW_KEY_ESCAPE))
		{
			g_IsPaused = true;
			pauser->OnLoad();
			g_Window->ShowMouseCursor();
		}

		static bool wasMousePressed = false;

		if (g_IsPaused)
		{
			if (g_Input.GetKeyState(GLFW_KEY_ESCAPE) && inSmthAgain)
			{
				// Add in audio feedback

				pauser->OnExit();
				pauser = CreatePausedMenu(PauseState::Paused);
				pauser->OnLoad();
				inSmthAgain = false;
			}

			if (g_Input.GetMouseState(GLFW_MOUSE_BUTTON_LEFT) && !inSmthAgain)
			{
				if (g_Coordinator.HaveComponent<UIComponent>(ResumeGame))
				{
					auto& UICompt = g_Coordinator.GetComponent<UIComponent>(ResumeGame);
					if (UICompt.get_selected())
					{
						// Add in audio feedback


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


						pauser->OnExit();
						pauser = CreatePausedMenu(PauseState::Settings);
						pauser->OnLoad();
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


						pauser->OnExit();
						ResetGame();

						g_LevelManager.SetNextLevel("MainMenu");

						/*pauser = CreatePausedMenu(PauseState::QuitGame);
						pauser->OnLoad();*/
					}
				}
			}

			if (g_Input.GetMouseState(GLFW_MOUSE_BUTTON_LEFT) && !wasMousePressed && inSmthAgain)
			{
				wasMousePressed = true;

				// For Settings Page
				if (g_Coordinator.HaveComponent<UIComponent>(pauser->SFXLeft))
				{
					auto& UICompt = g_Coordinator.GetComponent<UIComponent>(pauser->SFXLeft);
					if (UICompt.get_selected())
					{
						std::cout << "decrease SFX\n";
					}
				}

				if (g_Coordinator.HaveComponent<UIComponent>(pauser->SFXRight))
				{
					auto& UICompt = g_Coordinator.GetComponent<UIComponent>(pauser->SFXRight);
					if (UICompt.get_selected())
					{
						std::cout << "increase SFX\n";
					}
				}

				if (g_Coordinator.HaveComponent<UIComponent>(pauser->BGMLeft))
				{
					auto& UICompt = g_Coordinator.GetComponent<UIComponent>(pauser->BGMLeft);
					if (UICompt.get_selected())
					{
						std::cout << "decrease BGM\n";
					}
				}

				if (g_Coordinator.HaveComponent<UIComponent>(pauser->BGMRight))
				{
					auto& UICompt = g_Coordinator.GetComponent<UIComponent>(pauser->BGMRight);
					if (UICompt.get_selected())
					{
						std::cout << "increase BGM\n";
					}
				}
			}
			else if (!g_Input.GetMouseState(GLFW_MOUSE_BUTTON_LEFT))
			{
				// Reset the mouse press state when the mouse button is released
				wasMousePressed = false;
			}
		}
	}
	void ResetGame()
	{
		g_IsPaused = false;
		g_ChangeText.Reset();
		g_Checklist.Reset();
		g_RopeBreaker.ResetRB();
		g_BoneCatcher.ResetBC();
	}
}