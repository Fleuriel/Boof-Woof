#pragma once
#include "Level Manager/Level.h"
#include "ECS/Coordinator.hpp"
#include "../Systems/CameraController/CameraController.h"
#include "../Systems/BoneCatcher/BoneCatcher.h"
#include "../Systems/RopeBreaker/RopeBreaker.h"
#include "../Systems/ChangeText/ChangeText.h"
#include "../Systems/Checklist/Checklist.h"
#include "../BoofWoof/Core/AssetManager/FilePaths.h"
#include "../GSM/GameStateMachine.h" // for g_IsPaused


class MainHall : public Level
{
	Entity playerEnt{}, RopeEnt{}, RopeEnt2{}, BridgeEnt{}, scentEntity1{}, scentEntity2{}, scentEntity3{}, puppy1{}, puppy2{}, puppy3{};
	Entity RexPee1{}, RexPee2{}, RexPee3{}, RexPee4{}, RexPee5{}, WaterBucket{}, WaterBucket2{}, WaterBucket3{}; // Smell Avoidance
	Entity FireSound{};
	Entity TestPee{}, TestCollider{};
	glm::vec3 TestPos{}, NewPos{};

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

	bool TimerInit = false;
	bool sniffa{ false };
	bool collectedPuppy1{ false }, collectedPuppy2{ false }, collectedPuppy3{ false }, chgChecklist{ false };
	bool playercollided{ false }, puppy1Collided{ false }, puppy2Collided{ false }, puppy3Collided{ false };
	bool puppy1Destroyed{ false }, puppy2Destroyed{ false }, puppy3Destroyed{ false };

	bool waterBucketcollided{ false }, waterBucket2collided{ false }, waterBucket3collided{ false }; // Smell Avoidance
	bool rexPee1collided{ false }, rexPee2collided{ false }, rexPee3collided{ false }, rexPee4collided{ false }, rexPee5collided{ false }; // Smell Avoidance
	bool peeMarked{ false }; // Smell Avoidance
	bool teb_last = false;

	bool peeSoundPlayed = false;
	bool waterSoundPlayed = false;
	bool testCollided = false;

	std::vector<Entity> particleEntities;

	void LoadLevel() override
	{
		g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES+"/MainHallM5.json");
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
			{"WaterBucket", [&](Entity entity) { WaterBucket = entity; }},
			{"WaterBucket2", [&](Entity entity) { WaterBucket2 = entity; }},
			{"WaterBucket3", [&](Entity entity) { WaterBucket3 = entity; }},
			{"red particle", [&](Entity entity) { FireSound = entity; }},
			{"TestObject", [&](Entity entity) { TestPee = entity; }},
			{"TestCollision", [&](Entity entity) { TestCollider = entity; }}

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
					&& RexPee1 && RexPee2 && RexPee3 && RexPee4 && RexPee5 && WaterBucket && WaterBucket2 && WaterBucket3 && TestPee && TestCollider)
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

		if (g_Coordinator.HaveComponent<AudioComponent>(FireSound)) {
			auto& fireAudio = g_Coordinator.GetComponent<AudioComponent>(FireSound);
			fireAudio.SetAudioSystem(&g_Audio);

			// Play Fire Audio (3D BGM)
			g_Audio.PlayEntity3DAudio(FireSound, FILEPATH_ASSET_AUDIO + "/Fire.wav", true, "BGM");
			std::cout << " Fire Sound initialized in InitLevel for entity " << FireSound << std::endl;
		}
		else {
			std::cerr << " ERROR: FireSound entity has no AudioComponent in InitLevel!" << std::endl;
		}

		if (g_Coordinator.HaveComponent<TransformComponent>(TestPee))
		{
			auto& testPeeTransform = g_Coordinator.GetComponent<TransformComponent>(TestPee);
			TestPos = testPeeTransform.GetPosition();

			NewPos = TestPos - glm::vec3(0.0f, 20.0f, 0.0f);
			testPeeTransform.SetPosition(NewPos);
		}

		g_Audio.SetBGMVolume(g_Audio.GetBGMVolume());
		g_Audio.SetSFXVolume(g_Audio.GetSFXVolume());

		g_DialogueText.OnInitialize();
		g_DialogueText.setDialogue(DialogueState::OUTOFLIBRARY);

		g_Coordinator.GetSystem<LogicSystem>()->ReInit();

		particleEntities = { scentEntity1, scentEntity2, scentEntity3 };
		g_UI.OnInitialize();
	}

	void CheckCollision()
	{
		if (g_Coordinator.HaveComponent<CollisionComponent>(playerEnt))
		{
			playercollided = g_Coordinator.GetComponent<CollisionComponent>(playerEnt).GetIsColliding();
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

		if (g_Coordinator.HaveComponent<CollisionComponent>(TestCollider))
		{
			testCollided = g_Coordinator.GetComponent<CollisionComponent>(TestCollider).GetIsColliding();
		}

		if (g_Coordinator.HaveComponent<CollisionComponent>(WaterBucket))
		{
			waterBucketcollided = g_Coordinator.GetComponent<CollisionComponent>(WaterBucket).GetIsColliding();
		}

		if (g_Coordinator.HaveComponent<CollisionComponent>(WaterBucket2))
		{
			waterBucket2collided = g_Coordinator.GetComponent<CollisionComponent>(WaterBucket2).GetIsColliding();
		}

		if (g_Coordinator.HaveComponent<CollisionComponent>(WaterBucket3))
		{
			waterBucket3collided = g_Coordinator.GetComponent<CollisionComponent>(WaterBucket3).GetIsColliding();
		}

		if (playercollided && (rexPee1collided || rexPee2collided || rexPee3collided || rexPee4collided || rexPee5collided || testCollided) && !peeMarked && !peeSoundPlayed)
		{
			g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/PeePuddle.wav", false, "SFX");
			peeMarked = true;
			peeSoundPlayed = true;  // Ensure the sound plays only once
			waterSoundPlayed = false; // Reset water sound state

		}

		if (playercollided && (waterBucketcollided || waterBucket2collided || waterBucket3collided) && !waterSoundPlayed)
		{
			g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/WaterPuddle.wav", false, "SFX");
			peeMarked = false;
			timer = 0.0;

			// Reset sound state
			peeSoundPlayed = false;
			waterSoundPlayed = true; // Ensure water sound plays only once
			if (TimerInit) {
				g_TimerTR.OnShutdown();
				TimerInit = false;
			}
		}
	}

	void CheckPuppyCollision()
	{
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

		if (playercollided && puppy1Collided && !collectedPuppy1)
		{
			collectedPuppy1 = true;
		}

		if (playercollided && puppy2Collided && !collectedPuppy2)
		{
			collectedPuppy2 = true;
		}

		if (playercollided && puppy3Collided && !collectedPuppy3)
		{
			collectedPuppy3 = true;
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


		// ?? Update the positions of all 3D sounds (including the fireplace)
		g_Audio.Update3DSoundPositions();

		if (!g_DialogueText.dialogueActive)
		{
			pauseLogic::OnUpdate();
		}

		if (!g_IsPaused)
		{
			cameraController->Update(static_cast<float>(deltaTime));
			cooldownTimer += deltaTime;

			g_UI.Sniff(particleEntities, static_cast<float>(deltaTime));
			g_DialogueText.OnUpdate(deltaTime);


			if (!g_Checklist.shutted)
			{
				g_Checklist.OnUpdate(deltaTime);
			}

			CheckCollision();

			if (!collectedPuppy1 || !collectedPuppy2 || !collectedPuppy3)
			{
				CheckPuppyCollision();
			}

			if (peeMarked)
			{
				if (!TimerInit) {
					g_TimerTR.OnInitialize();
					g_TimerTR.timer = timerLimit;
					TimerInit = true;
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
						peeMarked = false;
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

				if (g_Coordinator.HaveComponent<TransformComponent>(TestPee))
				{
					auto& testPeeTransform = g_Coordinator.GetComponent<TransformComponent>(TestPee);
					testPeeTransform.SetPosition(TestPos);
				}

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
					if (g_Coordinator.HaveComponent<TransformComponent>(TestPee))
					{
						auto& testPeeTransform = g_Coordinator.GetComponent<TransformComponent>(TestPee);
						testPeeTransform.SetPosition(NewPos);
					}

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
		g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO+"/BedRoomMusicBGM.wav");
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
		rexPee1collided = rexPee2collided = rexPee3collided = rexPee4collided = rexPee5collided = false; // Smell Avoidance
		waterBucketcollided = waterBucket2collided = waterBucket3collided = peeMarked = false; // Smell Avoidance
		puppy1Destroyed = puppy2Destroyed = puppy3Destroyed = false;
		peeSoundPlayed = waterSoundPlayed = false;
		TimerInit = false;
		testCollided = false;

		g_TimerTR.OnShutdown();
	}
};