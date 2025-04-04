#pragma once
#include "Level Manager/Level.h"
#include "ResourceManager/ResourceManager.h"
#include "ECS/Coordinator.hpp"
#include "../BoofWoof/Core/AssetManager/FilePaths.h"

class CutsceneEnd : public Level
{
	double cutsceneTimer = 0.0;
	double timerLimit = 4.0;
	double waitingTime = 0.0;

	Entity P1{}, P2{}, P3{}, P4{}, P5{}, P6{}, P7{}, P8{}, P9{}, P10{}, P11{}, P12{}, P13{}, P14{};
	bool rightAppeared{ false }, finishCutscene{ false };

	void LoadLevel() override
	{
		// Use FILEPATH_ASSET_SCENES to construct the scene file path
		g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES + "/CutsceneEnd.json");
		g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/EndCutsceneBGM3.wav", false, "BGM");
		//g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/cutsceneSFX.wav", false, "SFX");*

		std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();

		// Use unordered_map to make it O(1) efficiency
		std::unordered_map<std::string, std::function<void(Entity)>> nameToAction =
		{
			{"Panel1", [&](Entity entity) { P1 = entity; }},
			{"Panel2", [&](Entity entity) { P2 = entity; }},
			{"Panel3", [&](Entity entity) { P3 = entity; }},
			{"Panel4", [&](Entity entity) { P4 = entity; }},
			{"Panel5", [&](Entity entity) { P5 = entity; }},
			{"Panel6", [&](Entity entity) { P6 = entity; }},
			{"Panel7", [&](Entity entity) { P7 = entity; }},
			{"Panel8", [&](Entity entity) { P8 = entity; }},
			{"Panel9", [&](Entity entity) { P9 = entity; }},
			{"Panel10", [&](Entity entity) { P10 = entity; }},
			{"Panel11", [&](Entity entity) { P11 = entity; }},
			{"Panel12", [&](Entity entity) { P12 = entity; }},
			{"Panel13", [&](Entity entity) { P13 = entity; }},
			{"Panel14", [&](Entity entity) { P14 = entity; }},
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

				// Exit early if all entities are found
				if (P1 && P2 && P3 && P4 && P5 && P6 && P7 && P8 && P9 && P10 && P11 && P12 && P13 && P14)
				{
					break;
				}
			}
		}
	}

	void InitLevel() override { /* Empty by design */
		g_Audio.SetBGMVolume(g_Audio.GetBGMVolume());
		g_Audio.SetSFXVolume(g_Audio.GetSFXVolume());
	}

	void UpdateLevel(double deltaTime) override
	{
		// Whole panel panning upwards
		if (g_Coordinator.HaveComponent<UIComponent>(P1) && !finishCutscene)
		{
			auto& one = g_Coordinator.GetComponent<UIComponent>(P1);
			auto& two = g_Coordinator.GetComponent<UIComponent>(P2);
			auto& three = g_Coordinator.GetComponent<UIComponent>(P3);

			auto& four = g_Coordinator.GetComponent<UIComponent>(P4);
			auto& five = g_Coordinator.GetComponent<UIComponent>(P5);
			auto& six = g_Coordinator.GetComponent<UIComponent>(P6);
			auto& seven = g_Coordinator.GetComponent<UIComponent>(P7);

			auto& eight = g_Coordinator.GetComponent<UIComponent>(P8);
			auto& nine = g_Coordinator.GetComponent<UIComponent>(P9);
			auto& ten = g_Coordinator.GetComponent<UIComponent>(P10);

			auto& eleven = g_Coordinator.GetComponent<UIComponent>(P11);
			auto& twelve = g_Coordinator.GetComponent<UIComponent>(P12);
			auto& thirteen = g_Coordinator.GetComponent<UIComponent>(P13);
			auto& fourteen = g_Coordinator.GetComponent<UIComponent>(P14);

			if (one.get_opacity() < 1.0f)
			{
				one.set_opacity(one.get_opacity() + 0.3f * static_cast<float>(deltaTime));
			}

			if (one.get_opacity() >= 0.4f)
			{
				if (two.get_opacity() < 1.0f)
				{
					two.set_opacity(two.get_opacity() + 0.3f * static_cast<float>(deltaTime));
				}
			}

			if (two.get_opacity() >= 0.4f)
			{
				one.set_position(glm::vec2(one.get_position().x, one.get_position().y + (0.2f * static_cast<float>(deltaTime))));
				two.set_position(glm::vec2(two.get_position().x, two.get_position().y + (0.2f * static_cast<float>(deltaTime))));
				three.set_position(glm::vec2(three.get_position().x, three.get_position().y + (0.2f * static_cast<float>(deltaTime))));

				if (three.get_opacity() < 1.0f)
				{
					three.set_opacity(three.get_opacity() + 0.3f * static_cast<float>(deltaTime));
				}
			}

			if (three.get_position().y >= 0.4f)
			{
				four.set_position(glm::vec2(four.get_position().x, four.get_position().y + (0.2f * static_cast<float>(deltaTime))));
				five.set_position(glm::vec2(five.get_position().x, five.get_position().y + (0.2f * static_cast<float>(deltaTime))));
				six.set_position(glm::vec2(six.get_position().x, six.get_position().y + (0.2f * static_cast<float>(deltaTime))));
				seven.set_position(glm::vec2(seven.get_position().x, seven.get_position().y + (0.2f * static_cast<float>(deltaTime))));

				if (four.get_opacity() < 1.0f)
				{
					four.set_opacity(four.get_opacity() + 0.3f * static_cast<float>(deltaTime));
				}

				if (four.get_opacity() >= 0.5f)
				{
					if (five.get_opacity() < 1.0f)
					{
						five.set_opacity(five.get_opacity() + 0.3f * static_cast<float>(deltaTime));
					}
				}

				if (five.get_opacity() >= 0.6f)
				{
					if (six.get_opacity() < 1.0f)
					{
						six.set_opacity(six.get_opacity() + 0.3f * static_cast<float>(deltaTime));
					}
				}

				if (six.get_opacity() >= 0.6f)
				{
					if (seven.get_opacity() < 1.0f)
					{
						seven.set_opacity(seven.get_opacity() + 0.3f * static_cast<float>(deltaTime));
					}
				}
			}

			if (seven.get_position().y >= 0.4f)
			{
				eight.set_position(glm::vec2(eight.get_position().x, eight.get_position().y + (0.2f * static_cast<float>(deltaTime))));
				nine.set_position(glm::vec2(nine.get_position().x, nine.get_position().y + (0.2f * static_cast<float>(deltaTime))));
				ten.set_position(glm::vec2(ten.get_position().x, ten.get_position().y + (0.2f * static_cast<float>(deltaTime))));

				if (eight.get_opacity() < 1.0f)
				{
					eight.set_opacity(eight.get_opacity() + 0.3f * static_cast<float>(deltaTime));
				}
			}
			
			if (eight.get_position().y >= -0.6f)
			{
				if (eight.get_opacity() >= 0.6f)
				{
					if (nine.get_opacity() < 1.0f)
					{
						nine.set_opacity(nine.get_opacity() + 0.3f * static_cast<float>(deltaTime));
					}
				}

				if (nine.get_opacity() >= 0.6f)
				{
					if (ten.get_opacity() < 1.0f)
					{
						ten.set_opacity(ten.get_opacity() + 0.3f * static_cast<float>(deltaTime));
					}
				}
			}

			if (ten.get_position().y >= 0.4f)
			{
				if (eleven.get_position().y <= 0.0f)
				{
					eleven.set_position(glm::vec2(eleven.get_position().x, eleven.get_position().y + (0.2f * static_cast<float>(deltaTime))));
					twelve.set_position(glm::vec2(twelve.get_position().x, twelve.get_position().y + (0.2f * static_cast<float>(deltaTime))));
					thirteen.set_position(glm::vec2(thirteen.get_position().x, thirteen.get_position().y + (0.2f * static_cast<float>(deltaTime))));
					fourteen.set_position(glm::vec2(fourteen.get_position().x, fourteen.get_position().y + (0.2f * static_cast<float>(deltaTime))));
				}

				if (eleven.get_opacity() < 1.0f)
				{
					eleven.set_opacity(eleven.get_opacity() + 0.3f * static_cast<float>(deltaTime));
				}

				if (eleven.get_opacity() >= 0.6f)
				{
					if (twelve.get_opacity() < 1.0f)
					{
						twelve.set_opacity(twelve.get_opacity() + 0.3f * static_cast<float>(deltaTime));
					}
				}

				if (twelve.get_opacity() >= 0.6f)
				{
					if (thirteen.get_opacity() < 1.0f)
					{
						thirteen.set_opacity(thirteen.get_opacity() + 0.3f * static_cast<float>(deltaTime));
					}
				}

				if (thirteen.get_opacity() >= 1.0f)
				{
					waitingTime += deltaTime;

					if (waitingTime >= 3.0f) 
					{
						if (fourteen.get_opacity() < 1.0f)
						{
							fourteen.set_opacity(fourteen.get_opacity() + 0.3f * static_cast<float>(deltaTime));
						}
					}	
				}
				else 
				{
					waitingTime = 0.0;
				}

				if (fourteen.get_opacity() >= 1.0f)
				{
					finishCutscene = true;
				}
			}
		}

		if (finishCutscene)
		{
			cutsceneTimer += deltaTime;

			if (cutsceneTimer >= timerLimit)
			{
				// Thank you scene
				g_LevelManager.SetNextLevel("TYVM");
			}
		}
		else
		{
			// Space to skip cutscene - zero feedback as of now.
			if (g_Input.GetKeyState(GLFW_KEY_SPACE) >= 1)
			{
				// Thank you scene
				g_LevelManager.SetNextLevel("TYVM");
			}
		}
	}

	void FreeLevel() override { /* Empty by design */ }

	void UnloadLevel() override
	{
		g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO + "/EndCutsceneBGM3.wav");

		g_Coordinator.ResetEntities();
		cutsceneTimer = 0.0;
		waitingTime = 0.0;
		finishCutscene = false;
		rightAppeared = false;
	}
};
