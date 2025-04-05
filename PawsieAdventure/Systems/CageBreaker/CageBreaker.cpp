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


	Entity getEntity = 2147483647;

	if (CollidedCage1)
	{
		getEntity = cage1;
		g_Coordinator.GetComponent<AnimationComponent>(getEntity).cage1Hit = true;
	}
	else if (CollidedCage2)
	{
		getEntity = cage2;
	}
	else if (CollidedCage3)
	{
		getEntity = cage3;
	}





	if (BarSpawned)
	{
		g_BoneCatcher.OnUpdate(getEntity, deltaTime);

		// Only saves when you press ESC key
		SaveProgress();
	}
	else
	{

		auto allEntities = g_Coordinator.GetAliveEntitiesSet();
		for (auto& entity : allEntities)
		{
			if (entity == cage1 || entity == cage2 || entity == cage3)
			{

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
			
			cage1 = 2147483648;

			g_Coordinator.GetSystem<MyPhysicsSystem>()->RemoveEntityBody(cage1Collider);
			g_Coordinator.DestroyEntity(cage1Collider);
			CollidedCage1 = false;
			deletedCage1 = true;

			// Toggle isDynamic for the respective puppy
			//TogglePuppyDynamic(puppy1);
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


			cage2 = 2147483649;

			g_Coordinator.GetSystem<MyPhysicsSystem>()->RemoveEntityBody(cage2Collider);
			g_Coordinator.DestroyEntity(cage2Collider);
			CollidedCage2 = false;
			deletedCage2 = true;

			// Toggle isDynamic for the respective puppy
			//TogglePuppyDynamic(puppy2);
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

			cage1 = 2147483650;

			g_Coordinator.GetSystem<MyPhysicsSystem>()->RemoveEntityBody(cage3Collider);
			g_Coordinator.DestroyEntity(cage3Collider);
			CollidedCage3 = false;
			deletedCage3 = true;

			// Toggle isDynamic for the respective puppy
			//TogglePuppyDynamic(puppy3);
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

		static float start = 0, end = 0;

		if (CollidedCage1) 
		{
			g_Coordinator.GetComponent<CollisionComponent>(cage1Collider).SetLastCollidedObjectName("Floor");
			
			CageHitCounts[1] = g_BoneCatcher.m_HitCount;
			speedCage[1] = g_BoneCatcher.m_Speed;
			directionCage[1] = g_BoneCatcher.m_Direction;

			std::tuple<int, float, float> animationBreak = g_Coordinator.GetComponent<AnimationComponent>(cage1).animationVector[1];
			std::tuple<int, float, float> animationIdle = g_Coordinator.GetComponent<AnimationComponent>(cage1).animationVector[2];
			std::tuple<int, float, float> animationMoving = g_Coordinator.GetComponent<AnimationComponent>(cage1).animationVector[3];

			if (g_Coordinator.GetComponent<GraphicsComponent>(cage1).getModelName() == "Cage") 
			{
				if (g_Coordinator.HaveComponent<AnimationComponent>(cage1)) 
				{
					start = std::get<1>(animationIdle);
					end = std::get<2>(animationIdle);

					g_ResourceManager.AnimatorMap[g_Coordinator.GetComponent<GraphicsComponent>(cage1).getModelName()]->UpdateAnimation(g_Coordinator.GetComponent<AnimationComponent>(cage1), g_Coordinator.GetComponent<GraphicsComponent>(cage1).deltaTime, start, end);

					g_Coordinator.GetComponent<AnimationComponent>(cage1).pauseAnimation = true;
				}
			}

			CollidedCage1 = false;
		}

		if (CollidedCage2)
		{
			g_Coordinator.GetComponent<CollisionComponent>(cage2Collider).SetLastCollidedObjectName("Floor");
			CageHitCounts[2] = g_BoneCatcher.m_HitCount;
			speedCage[2] = g_BoneCatcher.m_Speed;
			directionCage[2] = g_BoneCatcher.m_Direction;

			std::tuple<int, float, float> animationBreak = g_Coordinator.GetComponent<AnimationComponent>(cage2).animationVector[1];
			std::tuple<int, float, float> animationIdle = g_Coordinator.GetComponent<AnimationComponent>(cage2).animationVector[2];
			std::tuple<int, float, float> animationMoving = g_Coordinator.GetComponent<AnimationComponent>(cage2).animationVector[3];

			if (g_Coordinator.GetComponent<GraphicsComponent>(cage2).getModelName() == "Cage1")
			{
				if (g_Coordinator.HaveComponent<AnimationComponent>(cage2))
				{
					start = std::get<1>(animationIdle);
					end = std::get<2>(animationIdle);

					g_ResourceManager.AnimatorMap[g_Coordinator.GetComponent<GraphicsComponent>(cage2).getModelName()]->UpdateAnimation(g_Coordinator.GetComponent<AnimationComponent>(cage2), g_Coordinator.GetComponent<GraphicsComponent>(cage2).deltaTime, start, end);

					g_Coordinator.GetComponent<AnimationComponent>(cage2).pauseAnimation = true;
				}
			}

			CollidedCage2 = false;
		}

		if (CollidedCage3)
		{
			g_Coordinator.GetComponent<CollisionComponent>(cage3Collider).SetLastCollidedObjectName("Floor");
			CageHitCounts[3] = g_BoneCatcher.m_HitCount;
			speedCage[3] = g_BoneCatcher.m_Speed;
			directionCage[3] = g_BoneCatcher.m_Direction;

			std::tuple<int, float, float> animationBreak = g_Coordinator.GetComponent<AnimationComponent>(cage3).animationVector[1];
			std::tuple<int, float, float> animationIdle = g_Coordinator.GetComponent<AnimationComponent>(cage3).animationVector[2];
			std::tuple<int, float, float> animationMoving = g_Coordinator.GetComponent<AnimationComponent>(cage3).animationVector[3];

			if (g_Coordinator.GetComponent<GraphicsComponent>(cage3).getModelName() == "Cage2")
			{
				if (g_Coordinator.HaveComponent<AnimationComponent>(cage3))
				{
					start = std::get<1>(animationIdle);
					end = std::get<2>(animationIdle);

					g_ResourceManager.AnimatorMap[g_Coordinator.GetComponent<GraphicsComponent>(cage3).getModelName()]->UpdateAnimation(g_Coordinator.GetComponent<AnimationComponent>(cage3), g_Coordinator.GetComponent<GraphicsComponent>(cage3).deltaTime, start, end);

					g_Coordinator.GetComponent<AnimationComponent>(cage3).pauseAnimation = true;
				}
			}

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

void CageBreaker::TogglePuppyDynamic(Entity puppy)
{
	if (g_Coordinator.HaveComponent<CollisionComponent>(puppy)) {
		auto& collisionComponent = g_Coordinator.GetComponent<CollisionComponent>(puppy);
		collisionComponent.SetIsDynamic(true);
		//g_Coordinator.GetSystem<MyPhysicsSystem>()->EnablePhysics(puppy);
	}
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
