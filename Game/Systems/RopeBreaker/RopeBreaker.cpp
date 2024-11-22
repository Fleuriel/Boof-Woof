#include "RopeBreaker.h"

RopeBreaker g_RopeBreaker;

void RopeBreaker::OnUpdate(double deltaTime)
{
	if (g_Input.GetKeyState(GLFW_KEY_V) >= 1)
	{
		// INTERACT ???

	}

	if (!PlayerCollidedRope1 || !PlayerCollidedRope2)
	{
		CheckCollision();
	}

	g_BoneCatcher.OnUpdate(deltaTime);	
}

void RopeBreaker::CheckCollision()
{
	if (g_Coordinator.HaveComponent<CollisionComponent>(player)) 
	{
		PlayerColliding = g_Coordinator.GetComponent<CollisionComponent>(player).GetIsColliding();
	}

	if (g_Coordinator.HaveComponent<CollisionComponent>(rope1)) 
	{
		Rope1Colliding = g_Coordinator.GetComponent<CollisionComponent>(rope1).GetIsColliding();
	}

	if (g_Coordinator.HaveComponent<CollisionComponent>(rope2))
	{
		Rope2Colliding = g_Coordinator.GetComponent<CollisionComponent>(rope2).GetIsColliding();
	}

	if (PlayerColliding && Rope1Colliding && !PlayerCollidedRope1)
	{
		PlayerCollidedRope1 = true;
		SpawnBoneCatcher();
	}

	if (PlayerColliding && Rope2Colliding && !PlayerCollidedRope2)
	{
		PlayerCollidedRope2 = true;
		SpawnBoneCatcher();
	}
}

void RopeBreaker::SpawnBoneCatcher()
{
	// If collide with rope then load
	if (PlayerCollidedRope1 || PlayerCollidedRope2)
	{
		g_BoneCatcher.OnInitialize();
	}
}

void RopeBreaker::DespawnRope()
{
	std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();

	for (auto entity : entities)
	{
		if (g_Coordinator.HaveComponent<MetadataComponent>(entity))
		{
			if (PlayerCollidedRope1) 
			{
				if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Rope1")
				{
					g_Coordinator.DestroyEntity(entity);
				}
			}
			
			if (PlayerCollidedRope2)
			{
				if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Rope2")
				{
					g_Coordinator.DestroyEntity(entity);
				}
			}
		}		
	}
}
