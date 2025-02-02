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
	Entity playerEnt{}, RopeEnt{}, RopeEnt2{}, BridgeEnt{}, scentEntity1{}, scentEntity2{}, scentEntity3{}, puppy1{}, puppy2{}, puppy3{};
	Entity RexPee1{}, RexPee2{}, RexPee3{}, RexPee4{}, RexPee5{}, WaterBucket{}; // Smell Avoidance
	CameraController* cameraController = nullptr;

	double timer = 0.0; // Timer for smell avoidance
	double timerLimit = 20.0f; // Timer limit for smell avoidance

	bool sniffa{ false };
	bool collectedBall{ false }, collectedBone{ false }, chgChecklist{ false };
	bool playercollided{ false }, puppy1Collided{ false }, puppy2Collided{ false }, puppy3Collided{ false };
	bool puppy1Destroyed{ false }, puppy2Destroyed{ false }, puppy3Destroyed{ false };

	bool waterBucketcollided{ false }; // Smell Avoidance
	bool rexPee1collided{ false }, rexPee2collided{ false }, rexPee3collided{ false }, rexPee4collided{ false }, rexPee5collided{ false }; // Smell Avoidance
	bool peeMarked{ false }; // Smell Avoidance
	bool teb_last = false;

	bool peeSoundPlayed = false;
	bool waterSoundPlayed = false;

	void LoadLevel() override
	{
		g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES+"/MainHallM4.json");
		g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO+"/BedRoomMusicBGM.wav", true, "BGM");

		std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();

		// Use unordered_map to make it O(1) efficiency
		std::unordered_map<std::string, std::function<void(Entity)>> nameToAction =
		{
			{"Player", [&](Entity entity) { playerEnt = entity; }},
			{"Rope1", [&](Entity entity) { RopeEnt = entity; }},
			{"Rope2", [&](Entity entity) { RopeEnt2 = entity; }},
			{"DrawBridge", [&](Entity entity) { BridgeEnt = entity; }},
			{"Puppy1", [&](Entity entity) { puppy1 = entity; }},
			{"Puppy2", [&](Entity entity) { puppy2 = entity; }},
			{"Puppy3", [&](Entity entity) { puppy3 = entity; }},
			{"ScentTrail1", [&](Entity entity) { scentEntity1 = entity; }},
			{"ScentTrail2", [&](Entity entity) { scentEntity2 = entity; }},
			{"ScentTrail3", [&](Entity entity) { scentEntity3 = entity; }},
			{"Pee1", [&](Entity entity) { RexPee1 = entity; }},
			{"Pee2", [&](Entity entity) { RexPee2 = entity; }},
			{"Pee3", [&](Entity entity) { RexPee3 = entity; }},
			{"Pee4", [&](Entity entity) { RexPee4 = entity; }},
			{"Pee5", [&](Entity entity) { RexPee5 = entity; }},
			{"WaterBucket", [&](Entity entity) { WaterBucket = entity; }}
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
				if (playerEnt && RopeEnt && RopeEnt2 && BridgeEnt && puppy1 && puppy2 && puppy3 && scentEntity1 && scentEntity2 && scentEntity3
					&& RexPee1 && RexPee2 && RexPee3 && RexPee4 && RexPee5 && WaterBucket)
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
		g_Checklist.ChangeAsset(g_Checklist.Do3, glm::vec2(0.15f, 0.05f), "Do8");

		g_Checklist.ChangeAsset(g_Checklist.Do4, glm::vec2(0.0f, 0.0f), "");
		g_Checklist.ChangeAsset(g_Checklist.Box4, glm::vec2(0.0f, 0.0f), "");

		if (g_Coordinator.HaveComponent<UIComponent>(g_Checklist.Paper))
		{
			g_Coordinator.GetComponent<UIComponent>(g_Checklist.Paper).set_position(glm::vec2(-0.73f, 0.968f));
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

		if (g_Coordinator.HaveComponent<CollisionComponent>(puppy1))
		{
			puppy1Collided = g_Coordinator.GetComponent<CollisionComponent>(puppy1).GetIsColliding();
		}

		if (g_Coordinator.HaveComponent<CollisionComponent>(puppy2))
		{
			puppy2Collided = g_Coordinator.GetComponent<CollisionComponent>(puppy2).GetIsColliding();
		}

		if (g_Coordinator.HaveComponent<CollisionComponent>(puppy3))
		{
			puppy3Collided = g_Coordinator.GetComponent<CollisionComponent>(puppy3).GetIsColliding();
		}

		// Smell Avoidance
		if (g_Coordinator.HaveComponent<CollisionComponent>(RexPee1))
		{
			rexPee1collided = g_Coordinator.GetComponent<CollisionComponent>(RexPee1).GetIsColliding();
		}

		if (g_Coordinator.HaveComponent<CollisionComponent>(RexPee2))
		{
			rexPee2collided = g_Coordinator.GetComponent<CollisionComponent>(RexPee2).GetIsColliding();
		}

		if (g_Coordinator.HaveComponent<CollisionComponent>(RexPee3))
		{
			rexPee3collided = g_Coordinator.GetComponent<CollisionComponent>(RexPee3).GetIsColliding();
		}

		if (g_Coordinator.HaveComponent<CollisionComponent>(RexPee4))
		{
			rexPee4collided = g_Coordinator.GetComponent<CollisionComponent>(RexPee4).GetIsColliding();
		}

		if (g_Coordinator.HaveComponent<CollisionComponent>(RexPee5))
		{
			rexPee5collided = g_Coordinator.GetComponent<CollisionComponent>(RexPee5).GetIsColliding();
		}

		if (g_Coordinator.HaveComponent<CollisionComponent>(WaterBucket))
		{
			waterBucketcollided = g_Coordinator.GetComponent<CollisionComponent>(WaterBucket).GetIsColliding();
		}

		if (playercollided && (rexPee1collided || rexPee2collided || rexPee3collided || rexPee4collided || rexPee5collided) && !peeMarked && !peeSoundPlayed)
		{
			g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/PeePuddle.wav", false, "SFX");
			peeMarked = true;
			peeSoundPlayed = true;  // Ensure the sound plays only once

		}

		if (playercollided && waterBucketcollided && !waterSoundPlayed)
		{
			g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/WaterPuddle.wav", false, "SFX");
			peeMarked = false;
			timer = 0.0;

			// Reset sound state
			peeSoundPlayed = false;
			waterSoundPlayed = true; // Ensure water sound plays only once
		}
		//////////////////////////////////////////////////////////////////////////

		if (playercollided && puppy1Collided && !collectedBall)
		{
			collectedBall = true;
		}

		if (playercollided && puppy2Collided && !collectedBone)
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

			if (peeMarked)
			{
				timer += deltaTime;

				if (timer >= timerLimit)
				{
					peeMarked = false;
					timer = 0.0;
					auto* loading = dynamic_cast<LoadingLevel*>(g_LevelManager.GetLevel("LoadingLevel"));
					if (loading)
					{
						// Pass in the name of the real scene we want AFTER the loading screen
						loading->m_NextScene = "MainHall";
						g_LevelManager.SetNextLevel("LoadingLevel");
					}
				}
			}

			// If collected Tennis Ball
			if (collectedBall && !puppy2Destroyed)
			{
				g_Checklist.ChangeBoxChecked(g_Checklist.Box1);
				g_Coordinator.GetSystem<MyPhysicsSystem>()->RemoveEntityBody(puppy1);
				g_Coordinator.DestroyEntity(puppy1);
				puppy2Destroyed = true;
			}

			// If collected Bone
			if (collectedBone && !puppy1Destroyed)
			{
				g_Checklist.ChangeBoxChecked(g_Checklist.Box2);
				g_Coordinator.GetSystem<MyPhysicsSystem>()->RemoveEntityBody(puppy2);
				g_Coordinator.DestroyEntity(puppy2);
				puppy1Destroyed = true;
			}

			// Collect 3rd puppy
			if (collectedBone && !puppy1Destroyed)
			{
				g_Checklist.ChangeBoxChecked(g_Checklist.Box3);
				//g_Coordinator.GetSystem<MyPhysicsSystem>()->RemoveEntityBody(bone);
				//g_Coordinator.DestroyEntity(bone);
				//boneDestroyed = true;
			}

			if (collectedBall && collectedBone && !chgChecklist)
			{
				g_Checklist.ChangeAsset(g_Checklist.Do1, glm::vec2(0.15f, 0.05f), "Do9");
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
		g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO+"/BedRoomMusicBGM.wav");

		g_Audio.StopBGM();
		g_Coordinator.GetSystem<MyPhysicsSystem>()->ClearAllBodies();
		g_Coordinator.ResetEntities();

		// Ensure RESET for game to be replayable
		g_Checklist.shutted = false;
		sniffa = collectedBall = collectedBone = chgChecklist = false;
		playercollided = puppy1Collided = puppy2Collided = false;
		rexPee1collided = rexPee2collided = rexPee3collided = rexPee4collided = rexPee5collided = false; // Smell Avoidance
		waterBucketcollided = peeMarked = false; // Smell Avoidance
		puppy1Destroyed = puppy2Destroyed = false;
	}
};