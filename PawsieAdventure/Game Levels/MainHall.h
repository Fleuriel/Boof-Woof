#pragma once
#include "Level Manager/Level.h"
#include "ECS/Coordinator.hpp"
#include "../Systems/CameraController/CameraController.h"
#include "../Systems/BoneCatcher/BoneCatcher.h"
#include "../Systems/RopeBreaker/RopeBreaker.h"
#include "../Systems/ChangeText/ChangeText.h"
#include "../Systems/Checklist/Checklist.h"
#include "../BoofWoof/Core/AssetManager/FilePaths.h"

class MainHall : public Level
{
	Entity playerEnt{}, RopeEnt{}, RopeEnt2{}, BridgeEnt{}, scentEntity1{}, scentEntity2{}, tennisBall{}, bone{};
	CameraController* cameraController = nullptr;
	bool sniffa{ false };
	bool collectedBall{ false }, collectedBone{ false }, chgChecklist{ false };
	bool playercollided{ false }, tennisBallCollided{ false }, boneCollided{ false };
	bool boneDestroyed{ false }, tennisBallDestroyed{ false };
	bool teb_last = false;

	void LoadLevel() override
	{
		g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES+"/MainHall.json");
		g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO+"/BedRoomMusic.wav", true, "BGM");

		std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();

		// Use unordered_map to make it O(1) efficiency
		std::unordered_map<std::string, std::function<void(Entity)>> nameToAction =
		{
			{"Player", [&](Entity entity) { playerEnt = entity; }},
			{"Rope1", [&](Entity entity) { RopeEnt = entity; }},
			{"Rope2", [&](Entity entity) { RopeEnt2 = entity; }},
			{"DrawBridge", [&](Entity entity) { BridgeEnt = entity; }},
			{"Bone", [&](Entity entity) { bone = entity; }},
			{"TennisBall", [&](Entity entity) { tennisBall = entity; }},
			{"ScentTrail1", [&](Entity entity) { scentEntity1 = entity; }},
			{"ScentTrail2", [&](Entity entity) { scentEntity2 = entity; }}
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
				if (playerEnt && RopeEnt && RopeEnt2 && BridgeEnt && bone && tennisBall && scentEntity1 && scentEntity2)
				{
					break;
				}
			}
		}

		g_Window->HideMouseCursor();
	}

	void InitLevel() override
	{
		cameraController = new CameraController(playerEnt);
		g_RopeBreaker = RopeBreaker(playerEnt, RopeEnt, RopeEnt2, BridgeEnt);
		g_Checklist.OnInitialize();
		g_Checklist.ChangeAsset(g_Checklist.Do1, glm::vec2(0.15f, 0.05f), "Do6");
		g_Checklist.ChangeAsset(g_Checklist.Do2, glm::vec2(0.15f, 0.05f), "Do7");

		g_Checklist.ChangeAsset(g_Checklist.Do3, glm::vec2(0.0f, 0.0f), "");
		g_Checklist.ChangeAsset(g_Checklist.Do4, glm::vec2(0.0f, 0.0f), "");
		g_Checklist.ChangeAsset(g_Checklist.Box3, glm::vec2(0.0f, 0.0f), "");
		g_Checklist.ChangeAsset(g_Checklist.Box4, glm::vec2(0.0f, 0.0f), "");

		if (g_Coordinator.HaveComponent<UIComponent>(g_Checklist.Paper))
		{
			g_Coordinator.GetComponent<UIComponent>(g_Checklist.Paper).set_position(glm::vec2(-0.73f, 1.065f));
		}

		g_Audio.SetBGMVolume(g_Audio.GetBGMVolume());
		g_Audio.SetSFXVolume(g_Audio.GetSFXVolume());
	}

	void CheckCollision()
	{
		if (g_Coordinator.HaveComponent<CollisionComponent>(playerEnt))
		{
			playercollided = g_Coordinator.GetComponent<CollisionComponent>(playerEnt).GetIsColliding();
		}

		if (g_Coordinator.HaveComponent<CollisionComponent>(tennisBall))
		{
			tennisBallCollided = g_Coordinator.GetComponent<CollisionComponent>(tennisBall).GetIsColliding();
		}

		if (g_Coordinator.HaveComponent<CollisionComponent>(bone))
		{
			boneCollided = g_Coordinator.GetComponent<CollisionComponent>(bone).GetIsColliding();
		}

		if (playercollided && tennisBallCollided && !collectedBall)
		{
			collectedBall = true;
		}

		if (playercollided && boneCollided && !collectedBone)
		{
			collectedBone = true;
		}
	}

	void UpdateLevel(double deltaTime) override
	{
		pauseLogic::OnUpdate();

		if (!g_IsPaused)
		{
			cameraController->Update(static_cast<float>(deltaTime));

			auto& opacity1 = g_Coordinator.GetComponent<ParticleComponent>(scentEntity1);
			auto& opacity2 = g_Coordinator.GetComponent<ParticleComponent>(scentEntity2);

			if (!g_Checklist.shutted)
			{
				g_Checklist.OnUpdate(deltaTime);
			}

			if (!collectedBall || !collectedBone)
			{
				CheckCollision();
			}

			// If collected Tennis Ball
			if (collectedBall && !tennisBallDestroyed)
			{
				g_Checklist.ChangeBoxChecked(g_Checklist.Box1);
				g_Coordinator.GetSystem<MyPhysicsSystem>()->RemoveEntityBody(tennisBall);
				g_Coordinator.DestroyEntity(tennisBall);
				tennisBallDestroyed = true;
			}

			// If collected Bone
			if (collectedBone && !boneDestroyed)
			{
				g_Checklist.ChangeBoxChecked(g_Checklist.Box2);
				g_Coordinator.GetSystem<MyPhysicsSystem>()->RemoveEntityBody(bone);
				g_Coordinator.DestroyEntity(bone);
				boneDestroyed = true;
			}

			if (collectedBall && collectedBone && !chgChecklist)
			{
				g_Checklist.ChangeAsset(g_Checklist.Do1, glm::vec2(0.15f, 0.05f), "Do8");
				g_Checklist.ChangeAsset(g_Checklist.Box1, glm::vec2(0.04f, 0.06f), "Box");
				g_Checklist.ChangeAsset(g_Checklist.Do2, glm::vec2(0.0f, 0.0f), "");
				g_Checklist.ChangeAsset(g_Checklist.Box2, glm::vec2(0.0f, 0.0f), "");

				if (g_Coordinator.HaveComponent<UIComponent>(g_Checklist.Paper))
				{
					g_Coordinator.GetComponent<UIComponent>(g_Checklist.Paper).set_position(glm::vec2(-0.73f, 1.165f));
				}

				chgChecklist = true;
			}

			if (chgChecklist)
			{
				g_RopeBreaker.OnUpdate(deltaTime);
			}

			if (g_Input.GetKeyState(GLFW_KEY_TAB) >= 1)
			{
				if (!teb_last)
				{
					teb_last = true;
					cameraController->ShakePlayer(1.0f, glm::vec3(0.1f, 0.1f, 0.1f));
				}
			}
			else
			{
				teb_last = false;
			}

			if (g_Input.GetKeyState(GLFW_KEY_E) >= 1 && !sniffa)
			{
				g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/CorgiSniff.wav", false, "SFX");

				opacity1.setParticleColor(glm::vec4(0.0470588244497776f, 0.6627451181411743f, 0.95686274766922f, 1.0f));
				opacity2.setParticleColor(glm::vec4(0.7960784435272217f, 0.0470588244497776f, 0.95686274766922f, 1.0f));
				sniffa = true;
			}

			if (g_Input.GetKeyState(GLFW_KEY_E) == 0)
			{
				sniffa = false;
			}
		}
	}

	void FreeLevel() override
	{
		if (cameraController)
		{
			delete cameraController;
			cameraController = nullptr;
		}
	}

	void UnloadLevel() override
	{
		g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO+"/BedRoomMusic.wav");

		g_Audio.StopBGM();
		g_Coordinator.GetSystem<MyPhysicsSystem>()->ClearAllBodies();
		g_Coordinator.ResetEntities();
		g_Checklist.shutted = false;
	}
};