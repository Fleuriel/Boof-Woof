#include "RopeBreaker.h"

void RopeBreaker::OnUpdate(double deltaTime)
{
	if (g_Input.GetKeyState(GLFW_KEY_V) >= 1)
	{
		// INTERACT ???
	}

	if (!PlayerCollidedRope)
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

	if (g_Coordinator.HaveComponent<CollisionComponent>(rope)) 
	{
		RopeColliding = g_Coordinator.GetComponent<CollisionComponent>(rope).GetIsColliding();
	}

	if (PlayerColliding && RopeColliding) 
	{
		PlayerCollidedRope = true;
		SpawnBoneCatcher();
	}
}

void RopeBreaker::SpawnBoneCatcher()
{
	// If collide with rope then load
	if (PlayerCollidedRope) 
	{
		g_BoneCatcher.OnInitialize();
	}
}
