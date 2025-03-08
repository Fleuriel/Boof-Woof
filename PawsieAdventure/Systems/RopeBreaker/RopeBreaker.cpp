#include "RopeBreaker.h"
#include "../Checklist/Checklist.h"
#include "../CageBreaker/CageBreaker.h"
#include "../Core/AssetManager/FilePaths.h"
#include "../Utilities/ForGame/Dialogue/Dialogue.h"
#include <Level Manager/LevelManager.h>


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
	if (g_Coordinator.HaveComponent<CollisionComponent>(player))
	{
		PlayerColliding = g_Coordinator.GetComponent<CollisionComponent>(player).GetIsColliding();
	}

	Rope1Colliding = g_CageBreaker.CheckEntityWithPlayerCollision(rope1);
	Rope2Colliding = g_CageBreaker.CheckEntityWithPlayerCollision(rope2);

	//if (g_Coordinator.HaveComponent<CollisionComponent>(rope1))
	//{
	//	Rope1Colliding = g_Coordinator.GetComponent<CollisionComponent>(rope1).GetIsColliding();
	//}

	//if (g_Coordinator.HaveComponent<CollisionComponent>(rope2))
	//{
	//	Rope2Colliding = g_Coordinator.GetComponent<CollisionComponent>(rope2).GetIsColliding();
	//}

	if (!deletedRope1) 
	{
		if (PlayerColliding && Rope1Colliding && !PlayerCollidedRope1)
		{
			if (!firstRopeTouched)
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
		if (PlayerColliding && Rope2Colliding && !PlayerCollidedRope2)
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

	PlayerColliding = Rope1Colliding = Rope2Colliding = false;
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

	std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();

	for (auto entity : entities)
	{
		if (g_Coordinator.HaveComponent<MetadataComponent>(entity))
		{
			if (PlayerCollidedRope1 && !deletedRope1)
			{
				if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Rope1")
				{
					g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO+"/RopeSnap.wav", false, "SFX");

					g_Coordinator.GetSystem<MyPhysicsSystem>()->RemoveEntityBody(entity);
					g_Coordinator.DestroyEntity(entity);
					g_Coordinator.GetComponent<CollisionComponent>(player).SetLastCollidedObjectName("Floor");
					RopeDespawned++;
					deletedRope1 = true;
					PlayerCollidedRope1 = false;
				}
			}

			if (PlayerCollidedRope2 && !deletedRope2)
			{
				if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Rope2")
				{
					g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO+ "/RopeSnap.wav", false, "SFX");

					g_Coordinator.GetSystem<MyPhysicsSystem>()->RemoveEntityBody(entity);
					g_Coordinator.DestroyEntity(entity);
					g_Coordinator.GetComponent<CollisionComponent>(player).SetLastCollidedObjectName("Floor");
					RopeDespawned++;
					deletedRope2 = true;
					PlayerCollidedRope2 = false;
				}
			}
		}
	}

	if (RopeDespawned == 1)
	{
		g_DialogueText.OnInitialize();
		g_DialogueText.setDialogue(DialogueState::BROKEROPE1);
	}
}
