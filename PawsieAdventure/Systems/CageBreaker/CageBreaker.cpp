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


	Entity currentCage = 2147483647;

	if (CollidedCage1)
		currentCage = cage1;
	else if (CollidedCage2)
		currentCage = cage2;
	else if (CollidedCage3)
		currentCage = cage3;

	std::cout << "Currentcage\t" << currentCage << '\n';


	if (BarSpawned)
	{

		g_BoneCatcher.OnUpdate(deltaTime, currentCage);

		// Only saves when you press ESC key
		SaveProgress();
	}
	else
	{
		auto allEntities = g_Coordinator.GetAliveEntitiesSet();
		for (auto& entity : allEntities)
		{
			if (entity == cage1)
			{
				std::tuple<int, float, float> animationIdle1 = g_Coordinator.GetComponent<AnimationComponent>(entity).animationVector[g_BoneCatcher.ANIM_IDLE];

				g_ResourceManager.AnimatorMap[g_Coordinator.GetComponent<GraphicsComponent>(entity).getModelName()]->SetPlaybackRange(std::get<1>(animationIdle1), std::get<2>(animationIdle1));

			}
			if (entity == cage2)
			{
				std::tuple<int, float, float> animationIdle2 = g_Coordinator.GetComponent<AnimationComponent>(entity).animationVector[g_BoneCatcher.ANIM_IDLE];

				g_ResourceManager.AnimatorMap[g_Coordinator.GetComponent<GraphicsComponent>(entity).getModelName()]->SetPlaybackRange(std::get<1>(animationIdle2), std::get<2>(animationIdle2));
			}
			if (entity == cage3)
			{
				std::tuple<int, float, float> animationIdle3 = g_Coordinator.GetComponent<AnimationComponent>(entity).animationVector[g_BoneCatcher.ANIM_IDLE];

				g_ResourceManager.AnimatorMap[g_Coordinator.GetComponent<GraphicsComponent>(entity).getModelName()]->SetPlaybackRange(std::get<1>(animationIdle3), std::get<2>(animationIdle3));

			}
		}
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

void CageBreaker::DespawnCage(Entity entity)
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
			g_Coordinator.GetSystem<MyPhysicsSystem>()->RemoveEntityBody(cage1);
			g_Coordinator.DestroyEntity(cage1);
			cage1 = 2147483647;

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
			cage2 = 2147483648;
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
			cage3 = 2147483649;
			g_Coordinator.GetSystem<MyPhysicsSystem>()->RemoveEntityBody(cage3Collider);
			g_Coordinator.DestroyEntity(cage3Collider);
			CollidedCage3 = false;
			deletedCage3 = true;
		}
	}

	std::vector<Entity> checklistEnt = { g_Checklist.Paper, g_Checklist.Do1, g_Checklist.Do2, g_Checklist.Do3, g_Checklist.Box1, g_Checklist.Box2, g_Checklist.Box3 };
	g_Checklist.HideChecklistUI(checklistEnt, false);

	g_BoneCatcher.isCage = false;
}

void CageBreaker::SaveProgress()
{
	if (g_Input.GetKeyState(GLFW_KEY_ESCAPE) == 1 && !g_BoneCatcher.savePawgress)
	{
		std::vector<Entity> checklistEnt = { g_Checklist.Paper, g_Checklist.Do1, g_Checklist.Do2, g_Checklist.Do3, g_Checklist.Box1, g_Checklist.Box2, g_Checklist.Box3 };
		g_Checklist.HideChecklistUI(checklistEnt, false);

		if (CollidedCage1) 
		{
			g_Coordinator.GetComponent<CollisionComponent>(cage1Collider).SetLastCollidedObjectName("Floor");
			
			CageHitCounts[1] = g_BoneCatcher.m_HitCount;
			speedCage[1] = g_BoneCatcher.m_Speed;
			directionCage[1] = g_BoneCatcher.m_Direction;

			CollidedCage1 = false;
		}

		if (CollidedCage2)
		{
			g_Coordinator.GetComponent<CollisionComponent>(cage2Collider).SetLastCollidedObjectName("Floor");
			CageHitCounts[2] = g_BoneCatcher.m_HitCount;
			speedCage[2] = g_BoneCatcher.m_Speed;
			directionCage[2] = g_BoneCatcher.m_Direction;

			CollidedCage2 = false;
		}

		if (CollidedCage3)
		{
			g_Coordinator.GetComponent<CollisionComponent>(cage3Collider).SetLastCollidedObjectName("Floor");
			CageHitCounts[3] = g_BoneCatcher.m_HitCount;
			speedCage[3] = g_BoneCatcher.m_Speed;
			directionCage[3] = g_BoneCatcher.m_Direction;

			CollidedCage3 = false;
		}

		g_BoneCatcher.ClearBoneCatcher();

		BarSpawned = false;
		g_BoneCatcher.isCage = false;
		g_BoneCatcher.savePawgress = true;
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

	CageHitCounts.clear();
	speedCage.clear();
	directionCage.clear();
}
