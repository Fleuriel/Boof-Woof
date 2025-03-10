#include "CageBreaker.h"
#include "../Checklist/Checklist.h"
#include "../Core/AssetManager/FilePaths.h"
#include "../Utilities/ForGame/Dialogue/Dialogue.h"
#include <Level Manager/LevelManager.h>

CageBreaker g_CageBreaker;

void CageBreaker::OnUpdate(double deltaTime)
{
	if (!CollidedCage1 || !CollidedCage2 || !CollidedCage3)
	{
		CheckCageCollision();
	}

	// Only when collided and when dialogue isn't active
	SpawnBoneCatcher();

	if (BarSpawned)
	{
		g_BoneCatcher.OnUpdate(deltaTime);
	}
}

void CageBreaker::CheckCageCollision()
{
	Cage1Colliding = CheckEntityWithPlayerCollision(cage1Collider);
	Cage2Colliding = CheckEntityWithPlayerCollision(cage2Collider);
	Cage3Colliding = CheckEntityWithPlayerCollision(cage3Collider);

	if (!deletedCage1)
	{
		if (Cage1Colliding && !CollidedCage1)
		{
			if (!firstCageTouched)
			{
				g_DialogueText.OnInitialize();
				g_DialogueText.setDialogue(DialogueState::FOUNDPUP1);
				firstCageTouched = true;
			}
			CollidedCage1 = true;
		}
	}

	if (!deletedCage2)
	{
		if (Cage2Colliding && !CollidedCage2)
		{
			if (!secondCageTouched)
			{
				g_DialogueText.OnInitialize();
				g_DialogueText.setDialogue(DialogueState::FOUNDPUP2);
				secondCageTouched = true;
			}
			CollidedCage2 = true;
		}
	}

	if (!deletedCage3)
	{
		if (Cage3Colliding && !CollidedCage3)
		{
			if (!thirdCageTouched)
			{
				g_DialogueText.OnInitialize();
				g_DialogueText.setDialogue(DialogueState::FOUNDPUP3);
				thirdCageTouched = true;
			}
			CollidedCage3 = true;
		}
	}
}

void CageBreaker::SpawnBoneCatcher()
{
	if (CollidedCage1 || CollidedCage2 || CollidedCage3)
	{
		g_BoneCatcher.isCage = true;

		// Only if dialogue not active then can do rope breaker.
		if (!g_DialogueText.dialogueActive && !BarSpawned)
		{
			g_BoneCatcher.OnInitialize();
			BarSpawned = true;
		}
	}
}

void CageBreaker::DespawnCage()
{
	BarSpawned = false;

	if (CollidedCage1) 
	{
		if (!deletedCage1) 
		{
			if (!playedCageSound1)  
			{
				g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/CageBreak.wav", false, "SFX");
				playedCageSound1 = true;
			}
			g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO + "/MetalCage.wav");

			g_Coordinator.GetSystem<MyPhysicsSystem>()->RemoveEntityBody(cage1);
			g_Coordinator.DestroyEntity(cage1);

			g_Coordinator.GetSystem<MyPhysicsSystem>()->RemoveEntityBody(cage1Collider);
			g_Coordinator.DestroyEntity(cage1Collider);
			CollidedCage1 = false;
			deletedCage1 = true;
		}
	}

	if (CollidedCage2)
	{
		if (!deletedCage2)
		{
			if (!playedCageSound2)
			{
				g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/CageBreak.wav", false, "SFX");
				playedCageSound2 = true;
			}
			g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO + "/MetalCage.wav");

			g_Coordinator.GetSystem<MyPhysicsSystem>()->RemoveEntityBody(cage2);
			g_Coordinator.DestroyEntity(cage2);

			g_Coordinator.GetSystem<MyPhysicsSystem>()->RemoveEntityBody(cage2Collider);
			g_Coordinator.DestroyEntity(cage2Collider);
			CollidedCage2 = false;
			deletedCage2 = true;
		}
	}

	if (CollidedCage3)
	{
		if (!deletedCage3)
		{

			if (!playedCageSound3)
			{
				g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/CageBreak.wav", false, "SFX");
				playedCageSound3 = true;
			}
			g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO + "/MetalCage.wav");

			g_Coordinator.GetSystem<MyPhysicsSystem>()->RemoveEntityBody(cage3);
			g_Coordinator.DestroyEntity(cage3);

			g_Coordinator.GetSystem<MyPhysicsSystem>()->RemoveEntityBody(cage3Collider);
			g_Coordinator.DestroyEntity(cage3Collider);
			CollidedCage3 = false;
			deletedCage3 = true;
		}
	}
}

bool CageBreaker::CheckEntityWithPlayerCollision(Entity entity) const
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

void CageBreaker::ResetCB()
{
	CollidedCage1 = CollidedCage2 = CollidedCage3 = BarSpawned = false;
	Cage1Colliding = Cage2Colliding = Cage3Colliding = firstCageTouched = secondCageTouched =  thirdCageTouched = false;
	deletedCage1 = deletedCage2 = deletedCage3 = false;
}
