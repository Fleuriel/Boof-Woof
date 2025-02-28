#pragma once
#include "Level Manager/Level.h"
#include "ResourceManager/ResourceManager.h"
#include "ECS/Coordinator.hpp"
#include "../BoofWoof/Core/AssetManager/FilePaths.h"

class Cutscene : public Level
{
	double cutsceneTimer = 0.0;
	double timerLimit = 4.0;
	double lastBarkTime = 0.0;

	Entity P1{}, P2{}, P3{}, P4{}, P5{}, P6{}, P7{}, P8{}, P9{}, P10{}, P11{}, P12{}, P13{};
	bool rightAppeared{ false }, finishCutscene{ false };

	void LoadLevel() override
	{
		// Use FILEPATH_ASSET_SCENES to construct the scene file path
		g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES + "/Cutscene.json");
		g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/CutsceneBGM3.wav", false, "BGM");
		g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/cutsceneSFX.wav", false, "SFX");

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
				if (P1 && P2 && P3 && P4 && P5 && P6 && P7 && P8 && P9 && P10 && P11 && P12 && P13)
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
			auto& onePiece = g_Coordinator.GetComponent<UIComponent>(P1);
			auto& leftHalf = g_Coordinator.GetComponent<UIComponent>(P2);
			auto& quote = g_Coordinator.GetComponent<UIComponent>(P3);
			auto& rightHalf = g_Coordinator.GetComponent<UIComponent>(P4);

			onePiece.set_position(glm::vec2(onePiece.get_position().x, onePiece.get_position().y + (0.2f * static_cast<float>(deltaTime))));

			if (onePiece.get_position().y >= 0.6f)
			{
				// Left half & Right will pan upwards tgt but Right will appear later

				leftHalf.set_position(glm::vec2(leftHalf.get_position().x, leftHalf.get_position().y + (0.2f * static_cast<float>(deltaTime))));
				quote.set_position(glm::vec2(quote.get_position().x, quote.get_position().y + (0.2f * static_cast<float>(deltaTime))));
				rightHalf.set_position(glm::vec2(rightHalf.get_position().x, rightHalf.get_position().y + (0.2f * static_cast<float>(deltaTime))));

				// Gradually increase opacity for LeftHalf
				if (leftHalf.get_opacity() < 1.0f)
				{
					leftHalf.set_opacity(leftHalf.get_opacity() + 0.3f * static_cast<float>(deltaTime));
				}

				if (leftHalf.get_opacity() >= 0.8f)
				{
					if (quote.get_opacity() < 1.0f)
					{
						quote.set_opacity(quote.get_opacity() + 0.3f * static_cast<float>(deltaTime));
					}
				}

				if (quote.get_opacity() >= 0.6f)
				{
					if (rightHalf.get_opacity() < 1.0f)
					{
						rightHalf.set_opacity(rightHalf.get_opacity() + 0.3f * static_cast<float>(deltaTime));
					}
				}

				if (rightHalf.get_position().y >= 0.6f && !rightAppeared)
				{
					rightAppeared = true;
				}
			}

			if (g_Coordinator.HaveComponent<UIComponent>(P5) && rightAppeared)
			{
				auto& twoPiece = g_Coordinator.GetComponent<UIComponent>(P5);
				auto& quote2 = g_Coordinator.GetComponent<UIComponent>(P6);
				auto& threePiece = g_Coordinator.GetComponent<UIComponent>(P7);

				auto& eight = g_Coordinator.GetComponent<UIComponent>(P8);

				auto& Left9 = g_Coordinator.GetComponent<UIComponent>(P9);
				auto& Right10 = g_Coordinator.GetComponent<UIComponent>(P10);

				auto& FourPiece = g_Coordinator.GetComponent<UIComponent>(P11);
				auto& Left12 = g_Coordinator.GetComponent<UIComponent>(P12);
				auto& Right13 = g_Coordinator.GetComponent<UIComponent>(P13);

				twoPiece.set_position(glm::vec2(twoPiece.get_position().x, twoPiece.get_position().y + (0.2f * static_cast<float>(deltaTime))));

				if (twoPiece.get_opacity() < 1.0f)
				{
					twoPiece.set_opacity(twoPiece.get_opacity() + 0.3f * static_cast<float>(deltaTime));
				}

				if (twoPiece.get_position().y >= 0.0f)
				{
					quote2.set_position(glm::vec2(quote2.get_position().x, quote2.get_position().y + (0.2f * static_cast<float>(deltaTime))));

					if (twoPiece.get_opacity() >= 0.8f)
					{
						if (quote2.get_opacity() < 1.0f)
						{
							quote2.set_opacity(quote2.get_opacity() + 0.3f * static_cast<float>(deltaTime));
						}
					}
				}

				if (twoPiece.get_position().y >= 0.6f)
				{
					threePiece.set_position(glm::vec2(threePiece.get_position().x, threePiece.get_position().y + (0.2f * static_cast<float>(deltaTime))));

					if (threePiece.get_opacity() < 1.0f)
					{
						threePiece.set_opacity(threePiece.get_opacity() + 0.3f * static_cast<float>(deltaTime));
					}
				}

				if (threePiece.get_position().y >= 0.6f)
				{
					eight.set_position(glm::vec2(eight.get_position().x, eight.get_position().y + (0.2f * static_cast<float>(deltaTime))));

					if (eight.get_opacity() < 1.0f)
					{
						eight.set_opacity(eight.get_opacity() + 0.3f * static_cast<float>(deltaTime));
					}
				}
				
				if (eight.get_position().y >= 0.6f) 
				{
					Left9.set_position(glm::vec2(Left9.get_position().x, Left9.get_position().y + (0.2f * static_cast<float>(deltaTime))));
					Right10.set_position(glm::vec2(Right10.get_position().x, Right10.get_position().y + (0.2f * static_cast<float>(deltaTime))));

					if (Left9.get_opacity() < 1.0f)
					{
						Left9.set_opacity(Left9.get_opacity() + 0.3f * static_cast<float>(deltaTime));
					}

					if (Left9.get_opacity() >= 0.8f)
					{
						if (Right10.get_opacity() < 1.0f)
						{
							Right10.set_opacity(Right10.get_opacity() + 0.3f * static_cast<float>(deltaTime));
						}
					}
				}

				if (Right10.get_position().y >= 0.6f)
				{
					FourPiece.set_position(glm::vec2(FourPiece.get_position().x, FourPiece.get_position().y + (0.2f * static_cast<float>(deltaTime))));

					if (FourPiece.get_opacity() < 1.0f)
					{
						FourPiece.set_opacity(FourPiece.get_opacity() + 0.3f * static_cast<float>(deltaTime));
					}
				}

				if (FourPiece.get_position().y >= 0.6f)
				{
					Left12.set_position(glm::vec2(Left12.get_position().x, Left12.get_position().y + (0.2f * static_cast<float>(deltaTime))));
					Right13.set_position(glm::vec2(Right13.get_position().x, Right13.get_position().y + (0.2f * static_cast<float>(deltaTime))));

					if (Left12.get_opacity() < 1.0f)
					{
						Left12.set_opacity(Left12.get_opacity() + 0.3f * static_cast<float>(deltaTime));
					}

					if (Left12.get_opacity() >= 0.8f)
					{
						if (Right13.get_opacity() < 1.0f)
						{
							Right13.set_opacity(Right13.get_opacity() + 0.3f * static_cast<float>(deltaTime));
						}
					}

					if (Right13.get_position().y >= 0.0f)
					{
						finishCutscene = true;
					}
				}
			}
		}

		if (finishCutscene)
		{
			cutsceneTimer += deltaTime;

			if (cutsceneTimer >= timerLimit)
			{
				g_LevelManager.SetNextLevel("StartingRoom");
			}
		}
		else
		{
			// Space to skip cutscene - zero feedback as of now.
			if (g_Input.GetKeyState(GLFW_KEY_SPACE) >= 1)
			{
				g_LevelManager.SetNextLevel("StartingRoom");
			}
		}
	}

	void FreeLevel() override { /* Empty by design */ }

	void UnloadLevel() override
	{
		g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO + "/CutsceneBGM3.wav");
		g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO + "/cutsceneSFX.wav");


		g_Coordinator.ResetEntities();
		cutsceneTimer = 0.0;
		finishCutscene = false;
		rightAppeared = false;
	}
};