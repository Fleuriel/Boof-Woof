#pragma once
#include "Level Manager/Level.h"
#include "ECS/Coordinator.hpp"
#include "../Systems/CameraController/CameraController.h"
#include "../Systems/BoneCatcher/BoneCatcher.h"
#include "../Systems/RopeBreaker/RopeBreaker.h"
#include "../Systems/ChangeText/ChangeText.h"
#include "../Systems/Checklist/Checklist.h"
#include "../Systems/SmellAvoidance/SmellAvoidance.h"
#include "../BoofWoof/Core/AssetManager/FilePaths.h"
#include "../GSM/GameStateMachine.h" // for g_IsPaused


class MainHall : public Level
{
	Entity playerEnt{}, RopeEnt{}, RopeEnt2{}, BridgeEnt{}, scentEntity1{}, scentEntity2{}, scentEntity3{}, puppy1{}, puppy2{}, puppy3{};
	Entity WaterBucket{}, WaterBucket2{}, WaterBucket3{}; // Smell Avoidance
	Entity FireSound{};

	Entity TestPee{}, TestCollider{}; // Smell Avoidance
	Entity pee1{}, pee2{}, pee3{}, pee4{}; // Smell Avoidance
	Entity pee1Collider{}, pee2Collider{}, pee3Collider{}, pee4Collider{}; // Smell Avoidance

	CameraController* cameraController = nullptr;
	bool savedcamdir{ false };
	glm::vec3 camdir{};

	double timer = 0.0; // Timer for smell avoidance
	double timerLimit = 10.0f; // Timer limit for smell avoidance
	double timesUp = 2.0;// Time limit for times up sound

	double colorChangeTimer = 0.0;
	double colorChangeDuration = 3.0; // Duration for which the color change lasts
	double cooldownTimer = 10.0;
	double cooldownDuration = 10.0; // Cooldown duration
	bool isColorChanged = false;

	bool sniffa{ false };
	bool collectedPuppy1{ false }, collectedPuppy2{ false }, collectedPuppy3{ false }, chgChecklist{ false };
	bool playercollided{ false }, puppy1Collided{ false }, puppy2Collided{ false }, puppy3Collided{ false };
	bool puppy1Destroyed{ false }, puppy2Destroyed{ false }, puppy3Destroyed{ false };
	bool teb_last = false;

	std::vector<Entity> particleEntities;
	void LoadLevel() override
	{
		g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES+"/MainHallM5.json");
		g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO+"/BedRoomMusicBGM.wav", true, "BGM");

		std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();
		std::unordered_map<std::string, std::function<void(Entity)>> nameToAction = GetNameToActionMap();

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
				if (AllEntitiesInitialized())
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
		g_SmellAvoidance = SmellAvoidance(playerEnt, pee1, pee2, pee3, pee4, pee1Collider, pee2Collider, pee3Collider, pee4Collider, WaterBucket, WaterBucket2, WaterBucket3, TestPee, TestCollider);
		g_Checklist.OnInitialize();
		InitializeChecklist();
		InitializeFireSound();
		g_SmellAvoidance.Initialize();

		g_Audio.SetBGMVolume(g_Audio.GetBGMVolume());
		g_Audio.SetSFXVolume(g_Audio.GetSFXVolume());

		g_DialogueText.OnInitialize();
		g_DialogueText.setDialogue(DialogueState::OUTOFLIBRARY);

		g_Coordinator.GetSystem<LogicSystem>()->ReInit();

		particleEntities = { scentEntity1, scentEntity2, scentEntity3 };
		g_UI.OnInitialize();
	}

	void CheckPuppyCollision()
	{
		playercollided = CheckEntityCollision(playerEnt);
		puppy1Collided = CheckEntityCollision(puppy1);
		puppy2Collided = CheckEntityCollision(puppy2);
		puppy3Collided = CheckEntityCollision(puppy3);

		if (playercollided && puppy1Collided && !collectedPuppy1)
		{
			collectedPuppy1 = true;
			g_DialogueText.OnInitialize();
			g_DialogueText.setDialogue(DialogueState::TWOMORETOGO);
		}

		if (playercollided && puppy2Collided && !collectedPuppy2)
		{
			collectedPuppy2 = true;
			g_DialogueText.OnInitialize();
			g_DialogueText.setDialogue(DialogueState::ONEMORETOGO);
		}

		if (playercollided && puppy3Collided && !collectedPuppy3)
		{
			collectedPuppy3 = true;
			g_DialogueText.OnInitialize();
			g_DialogueText.setDialogue(DialogueState::ALLPUPSFOUND);
		}
	}

	void UpdateLevel(double deltaTime) override
	{
		if (g_IsPaused && !savedcamdir) {
			camdir = cameraController->GetCameraDirection(g_Coordinator.GetComponent<CameraComponent>(playerEnt));
			savedcamdir = true;
		}

		if (!g_IsPaused && savedcamdir) {
			cameraController->SetCameraDirection(g_Coordinator.GetComponent<CameraComponent>(playerEnt), camdir);
			savedcamdir = false;
		}

		if (g_Coordinator.HaveComponent<TransformComponent>(playerEnt)) {
			auto& playerTransform = g_Coordinator.GetComponent<TransformComponent>(playerEnt);
			glm::vec3 playerPos = playerTransform.GetPosition();
			glm::vec3 playerRot = playerTransform.GetRotation();  // Get rotation from TransformComponent

			g_Audio.SetListenerPosition(playerPos, playerRot);
		}

		// Update the positions of all 3D sounds (including the fireplace)
		g_Audio.Update3DSoundPositions();

		if (!g_DialogueText.dialogueActive)
		{
			pauseLogic::OnUpdate();
		}

		if (!g_IsPaused)
		{
			cameraController->Update(static_cast<float>(deltaTime));
			cooldownTimer += deltaTime;

			g_UI.OnUpdate(static_cast<float>(deltaTime));
			g_UI.Sniff(particleEntities, static_cast<float>(deltaTime));
			g_DialogueText.OnUpdate(deltaTime);


			if (!g_Checklist.shutted)
			{
				g_Checklist.OnUpdate(deltaTime);
			}

			//CheckCollision();
			g_SmellAvoidance.Update(deltaTime);

			if (!collectedPuppy1 || !collectedPuppy2 || !collectedPuppy3)
			{
				CheckPuppyCollision();
			}

			if (g_SmellAvoidance.GetPeeMarked())
			{
				if (!g_SmellAvoidance.GetTimerInit()) {
					g_TimerTR.OnInitialize();
					g_TimerTR.timer = timerLimit;
					g_SmellAvoidance.SetTimerInit(true);
				}
				g_TimerTR.OnUpdate(deltaTime);

				if (g_TimerTR.timer == 0.0)
				{
					timesUp -= deltaTime;

					// Times up! sound
					g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/Timesup.wav", false, "SFX");

					if (timesUp < 0.0)
					{
						timesUp = 2.0;
						g_SmellAvoidance.SetPeeMarked(false);
						auto* loading = dynamic_cast<LoadingLevel*>(g_LevelManager.GetLevel("LoadingLevel"));
						if (loading)
						{
							// Pass in the name of the real scene we want AFTER the loading screen
							loading->m_NextScene = "MainHall";
							g_LevelManager.SetNextLevel("LoadingLevel");
							g_TimerTR.OnShutdown();
						}
					}
				}
			}

			// If collected 1st puppy
			if (collectedPuppy1 && !puppy1Destroyed)
			{
				g_Checklist.ChangeBoxChecked(g_Checklist.Box1);
				g_Coordinator.GetSystem<MyPhysicsSystem>()->RemoveEntityBody(puppy1);
				g_Coordinator.DestroyEntity(puppy1);
				puppy1Destroyed = true;
			}

			// If collected 2nd puppy
			if (collectedPuppy2 && !puppy2Destroyed)
			{
				g_Checklist.ChangeBoxChecked(g_Checklist.Box2);
				g_Coordinator.GetSystem<MyPhysicsSystem>()->RemoveEntityBody(puppy2);
				g_Coordinator.DestroyEntity(puppy2);
				puppy2Destroyed = true;
			}

			// If collected 3rd puppy
			if (collectedPuppy3 && !puppy3Destroyed)
			{
				g_Checklist.ChangeBoxChecked(g_Checklist.Box3);
				g_Coordinator.GetSystem<MyPhysicsSystem>()->RemoveEntityBody(puppy3);
				g_Coordinator.DestroyEntity(puppy3);
				puppy3Destroyed = true;
			}

			if (collectedPuppy1 && collectedPuppy2 && collectedPuppy3 && !chgChecklist)
			{
				g_Checklist.ChangeAsset(g_Checklist.Do1, glm::vec2(0.15f, 0.05f), "Do9");
				g_Checklist.ChangeAsset(g_Checklist.Box1, glm::vec2(0.04f, 0.06f), "Box");
				g_Checklist.ChangeAsset(g_Checklist.Do2, glm::vec2(0.0f, 0.0f), "");
				g_Checklist.ChangeAsset(g_Checklist.Box2, glm::vec2(0.0f, 0.0f), "");
				g_Checklist.ChangeAsset(g_Checklist.Do3, glm::vec2(0.0f, 0.0f), "");
				g_Checklist.ChangeAsset(g_Checklist.Box3, glm::vec2(0.0f, 0.0f), "");

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

			// Take this away once u shift to script & peecontrol file
			if (g_Input.GetKeyState(GLFW_KEY_E) >= 1 && !sniffa && cooldownTimer >= cooldownDuration)
			{
				g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/CorgiSniff.wav", false, "SFX");

				//SetDefaultPeePosition();
				g_SmellAvoidance.SetDefaultPeePosition();

				sniffa = true;
				isColorChanged = true;
				colorChangeTimer = 0.0;
				cooldownTimer = 0.0;
			}

			if (isColorChanged)
			{
				colorChangeTimer += deltaTime;
				if (colorChangeTimer >= colorChangeDuration)
				{
					//SetNewPeePosition();
					g_SmellAvoidance.SetNewPeePosition();

					isColorChanged = false;
				}
			}

			if (g_Input.GetKeyState(GLFW_KEY_E) == 0)
			{
				sniffa = false;
			}
			// until here
		}
	}

	void FreeLevel() override
	{
		if (cameraController)
		{
			delete cameraController;
			cameraController = nullptr;
		}

		g_UI.OnShutdown();
	}

	void UnloadLevel() override
	{
		g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO + "/BedRoomMusicBGM.wav");
		if (g_Coordinator.HaveComponent<AudioComponent>(FireSound)) {
			auto& music = g_Coordinator.GetComponent<AudioComponent>(FireSound);
			music.StopAudio();
		}
		g_Audio.StopBGM();
		g_Coordinator.GetSystem<MyPhysicsSystem>()->ClearAllBodies();
		g_Coordinator.ResetEntities();

		// Ensure RESET for game to be replayable
		g_Checklist.shutted = false;
		sniffa = collectedPuppy1 = collectedPuppy2 = collectedPuppy3 = chgChecklist = false;
		playercollided = puppy1Collided = puppy2Collided = puppy3Collided = false;
		puppy1Destroyed = puppy2Destroyed = puppy3Destroyed = false;

		g_TimerTR.OnShutdown();
	}

private:
	std::unordered_map<std::string, std::function<void(Entity)>> GetNameToActionMap()
	{
		return {
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
			{"Pee1", [&](Entity entity) { pee1 = entity; }},
			{"Pee2", [&](Entity entity) { pee2 = entity; }},
			{"Pee3", [&](Entity entity) { pee3 = entity; }},
			{"Pee4", [&](Entity entity) { pee4 = entity; }},
			{"Pee1Collision", [&](Entity entity) { pee1Collider = entity; }},
			{"Pee2Collision", [&](Entity entity) { pee2Collider = entity; }},
			{"Pee3Collision", [&](Entity entity) { pee3Collider = entity; }},
			{"Pee4Collision", [&](Entity entity) { pee4Collider = entity; }},
			{"WaterBucket", [&](Entity entity) { WaterBucket = entity; }},
			{"WaterBucket2", [&](Entity entity) { WaterBucket2 = entity; }},
			{"WaterBucket3", [&](Entity entity) { WaterBucket3 = entity; }},
			{"red particle", [&](Entity entity) { FireSound = entity; }},
			{"TestObject", [&](Entity entity) { TestPee = entity; }},
			{"TestCollision", [&](Entity entity) { TestCollider = entity; }}
		};
	}

	bool AllEntitiesInitialized() const
	{
		return playerEnt && RopeEnt && RopeEnt2 && BridgeEnt && puppy1 && puppy2 && puppy3 && scentEntity1 && scentEntity2 && scentEntity3
			&& pee1 && pee2 && pee3 && pee4 && pee1Collider && pee2Collider && pee3Collider && pee4Collider && WaterBucket && WaterBucket2 && WaterBucket3 && TestPee && TestCollider;
	}

	void InitializeChecklist()
	{
		g_Checklist.ChangeAsset(g_Checklist.Do1, glm::vec2(0.15f, 0.05f), "Do6");
		g_Checklist.ChangeAsset(g_Checklist.Do2, glm::vec2(0.15f, 0.05f), "Do7");
		g_Checklist.ChangeAsset(g_Checklist.Do3, glm::vec2(0.15f, 0.05f), "Do8");
		g_Checklist.ChangeAsset(g_Checklist.Do4, glm::vec2(0.0f, 0.0f), "");
		g_Checklist.ChangeAsset(g_Checklist.Box4, glm::vec2(0.0f, 0.0f), "");

		if (g_Coordinator.HaveComponent<UIComponent>(g_Checklist.Paper))
		{
			g_Coordinator.GetComponent<UIComponent>(g_Checklist.Paper).set_position(glm::vec2(-0.73f, 0.968f));
		}
	}

	void InitializeFireSound() const
	{
		if (g_Coordinator.HaveComponent<AudioComponent>(FireSound)) {
			auto& fireAudio = g_Coordinator.GetComponent<AudioComponent>(FireSound);
			fireAudio.SetAudioSystem(&g_Audio);
			g_Audio.PlayEntity3DAudio(FireSound, FILEPATH_ASSET_AUDIO + "/Fire.wav", true, "BGM");
			std::cout << " Fire Sound initialized in InitLevel for entity " << FireSound << std::endl;
		}
		else {
			std::cerr << " ERROR: FireSound entity has no AudioComponent in InitLevel!" << std::endl;
		}
	}

	bool CheckEntityCollision(Entity entity)
	{
		if (g_Coordinator.HaveComponent<CollisionComponent>(entity))
		{
			return g_Coordinator.GetComponent<CollisionComponent>(entity).GetIsColliding();
		}
		return false;
	}
};