#include "RopeBreaker.h"
#include "../Checklist/Checklist.h"
#include "../CageBreaker/CageBreaker.h"
#include "../Core/AssetManager/FilePaths.h"
#include "../Utilities/ForGame/Dialogue/Dialogue.h"
#include <Level Manager/LevelManager.h>
#include "../ChangeText/ChangeText.h"


RopeBreaker g_RopeBreaker;

void RopeBreaker::OnUpdate(double deltaTime)
{
	if (!PlayerCollidedRope1 || !PlayerCollidedRope2) 
	{
		CheckCollision();
	}

	// Only when collided and when dialogue isn't active
	SpawnBoneCatcher();

	if (BoneSpawned)
	{
		g_BoneCatcher.OnUpdate(deltaTime);

		// Only saves when you press ESC key
		SaveRopeProgress();

		/*if (!g_ChangeText.startingRoomOnly) 
		{
			SaveRopeProgress();
		}*/
	}

	if (RopeDespawned >= 2 && !isFalling)
	{
		g_Checklist.ChangeBoxChecked(g_Checklist.Box1);
		g_Checklist.finishRB = true;
		DropBridge();
	}

	// Drawbridge
	if (!isFalling)
	{
		return; // do nothing

		// For quick testing
		//PlayerCollidedRope1 = PlayerCollidedRope2 = true;
		//RopeDespawned = 2;
	}
	else
	{
		ElapsedTime += static_cast<float>(deltaTime);

		if (!g_Coordinator.HaveComponent<TransformComponent>(bridge)) return;
		auto& transform = g_Coordinator.GetComponent<TransformComponent>(bridge);

		// Calculate smooth interpolation factor
		float t = glm::min(ElapsedTime / FallDuration, 1.0f);

		// Calculate current position with offset
		glm::vec3 currentPos = initialPos + TargetPos;

		// Calculate current rotation (only changing X rotation from initial to -90)
		glm::vec3 currentRotation = initialRotation;
		currentRotation.x = glm::mix(initialRotation.x, -90.0f, t);

		// Apply transforms
		transform.SetPosition(currentPos);

		//transform.SetPosition(initialPos);
		transform.SetRotation(glm::radians(currentRotation));

		// Stop animation when complete
		if (ElapsedTime >= FallDuration)
		{
			isFalling = false;

			// Enter Ending Cutscene
			g_LevelManager.SetNextLevel("CutsceneEnd");
		}
	}

}

void RopeBreaker::CheckCollision()
{
	Rope1Colliding = CheckEntityWithPlayerCollision(rope1);
	Rope2Colliding = CheckEntityWithPlayerCollision(rope2);

	if (!deletedRope1) 
	{
		if (Rope1Colliding && !PlayerCollidedRope1)
		{
			if (!firstRopeTouched && !g_ChangeText.startingRoomOnly)
			{
				g_DialogueText.OnInitialize();
				g_DialogueText.setDialogue(DialogueState::BREAKROPES);
				firstRopeTouched = true;
			}
			PlayerCollidedRope1 = true;
		}
	}	

	if (!deletedRope2)
	{
		if (Rope2Colliding && !PlayerCollidedRope2)
		{
			if (!firstRopeTouched)
			{
				g_DialogueText.OnInitialize();
				g_DialogueText.setDialogue(DialogueState::BREAKROPES);
				firstRopeTouched = true;
			}
			PlayerCollidedRope2 = true;
		}
	}
}

void RopeBreaker::DropBridge()
{
	if (!bridgeAudio)
	{
		g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO +"/BridgeCreak.wav", false, "SFX");
		bridgeAudio = true;
	}

	if (!g_Coordinator.HaveComponent<TransformComponent>(bridge)) return;

	auto& transform = g_Coordinator.GetComponent<TransformComponent>(bridge);

	// Store initial state
	initialPos = transform.GetPosition();
	initialRotation = glm::degrees(transform.GetRotation());

	// Start animation
	isFalling = true;
	ElapsedTime = 0.0f;

	g_DialogueText.OnInitialize();
	g_DialogueText.setDialogue(DialogueState::FREED);
}

void RopeBreaker::ResetRB()
{
	PlayerCollidedRope1 = PlayerCollidedRope2 = BoneSpawned = false;
	RopeCount = 2;

	Rope1Colliding = Rope2Colliding = false;
	RopeDespawned = 0;
	bridgeAudio = deletedRope1 = deletedRope2 = false;

	ElapsedTime = 0.0f;
	FallDuration = 2.0f;
	isFalling = false;
	firstRopeTouched = false;
}


void RopeBreaker::SpawnBoneCatcher()
{
	// If collide with rope then load
	if (PlayerCollidedRope1 || PlayerCollidedRope2)
	{
		g_BoneCatcher.isRope = true;

		// Only if dialogue not active then can do rope breaker.
		if (!g_DialogueText.dialogueActive && !BoneSpawned)
		{
			g_BoneCatcher.OnInitialize();
			BoneSpawned = true;
		}
	}
}

void RopeBreaker::DespawnRope()
{
	BoneSpawned = false;

	if (PlayerCollidedRope1 && !deletedRope1)
	{
		if (!playedRopeSnap1)
		{
			g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/RopeSnap.wav", false, "SFX");
			playedRopeSnap1 = true;
		}
		g_Coordinator.GetSystem<MyPhysicsSystem>()->RemoveEntityBody(rope1);
		g_Coordinator.DestroyEntity(rope1);
		g_Coordinator.GetComponent<CollisionComponent>(player).SetLastCollidedObjectName("Floor");
		RopeDespawned++;
		deletedRope1 = true;
		PlayerCollidedRope1 = false;
	}

	if (PlayerCollidedRope2 && !deletedRope2)
	{
		if (!playedRopeSnap2)
		{
			g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/RopeSnap.wav", false, "SFX");
			playedRopeSnap2 = true;
		}

		g_Coordinator.GetSystem<MyPhysicsSystem>()->RemoveEntityBody(rope2);
		g_Coordinator.DestroyEntity(rope2);
		g_Coordinator.GetComponent<CollisionComponent>(player).SetLastCollidedObjectName("Floor");
		RopeDespawned++;
		deletedRope2 = true;
		PlayerCollidedRope2 = false;
	}

	if (RopeDespawned == 1)
	{
		g_DialogueText.OnInitialize();
		g_DialogueText.setDialogue(DialogueState::BROKEROPE1);
	}

	g_BoneCatcher.isRope = false;
}

void RopeBreaker::DespawnRopeInStartingRoom()
{
	if (g_ChangeText.startingRoomOnly) 
	{
		if (PlayerCollidedRope1 && !deletedRope1)
		{
			if (!playedRopeSnap1)
			{
				g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/RopeSnap.wav", false, "SFX");
				playedRopeSnap1 = true;
			}
			g_Coordinator.GetSystem<MyPhysicsSystem>()->RemoveEntityBody(rope1);
			g_Coordinator.DestroyEntity(rope1);
			g_Coordinator.GetComponent<CollisionComponent>(player).SetLastCollidedObjectName("Floor");

			g_Checklist.ChangeBoxChecked(g_Checklist.Box3);
			g_Checklist.Check3 = true;

			deletedRope1 = true;
			PlayerCollidedRope1 = false;
		}
	}
}

void RopeBreaker::SaveRopeProgress()
{
	if (g_Input.GetKeyState(GLFW_KEY_ESCAPE) >= 1 && !g_BoneCatcher.savePawgress)
	{
		if (PlayerCollidedRope1)
		{
			g_Coordinator.GetComponent<CollisionComponent>(rope1).SetLastCollidedObjectName("Floor");
			g_Coordinator.GetComponent<CollisionComponent>(player).SetLastCollidedObjectName("Floor");
			RopeHitCounts[1] = g_BoneCatcher.m_HitCount;
			Rope1Colliding = false;
			PlayerCollidedRope1 = false;
		}

		if (PlayerCollidedRope2)
		{
			g_Coordinator.GetComponent<CollisionComponent>(rope2).SetLastCollidedObjectName("Floor");
			g_Coordinator.GetComponent<CollisionComponent>(player).SetLastCollidedObjectName("Floor");
			RopeHitCounts[2] = g_BoneCatcher.m_HitCount;
			Rope2Colliding = false;
			PlayerCollidedRope2 = false;
		}

		g_BoneCatcher.ClearBoneCatcher();

		BoneSpawned = false;
		g_BoneCatcher.isRope = false;
		g_BoneCatcher.savePawgress = true;
	}
}

bool RopeBreaker::CheckEntityWithPlayerCollision(Entity entity) const
{
	//Check Entity Collision with Player
	if (g_Coordinator.HaveComponent<CollisionComponent>(entity) && g_Coordinator.HaveComponent<CollisionComponent>(player))
	{
		auto& collider1 = g_Coordinator.GetComponent<CollisionComponent>(entity);
		if (collider1.GetIsColliding() && std::strcmp(collider1.GetLastCollidedObjectName().c_str(), "Player") == 0)
			return true;
	}
	return false;
}
