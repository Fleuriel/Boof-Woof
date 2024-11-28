#include "RopeBreaker.h"
#include "../Checklist/Checklist.h"

RopeBreaker g_RopeBreaker;

void RopeBreaker::OnUpdate(double deltaTime)
{
	if (!PlayerCollidedRope1 || !PlayerCollidedRope2)
	{
		CheckCollision();
	}

	if (BoneSpawned)
	{
		g_BoneCatcher.OnUpdate(deltaTime);
	}

	if (PlayerCollidedRope1 && PlayerCollidedRope2 && RopeDespawned >= 2 && !isFalling)
	{
		g_Checklist.ChangeBoxChecked(g_Checklist.Box1);
		g_Checklist.finishRB = true;
		DropBridge();
	}

	// Drawbridge
	if (!isFalling) 
	{
		//return; // do nothing

		// For quick testing
		PlayerCollidedRope1 = PlayerCollidedRope2 = true;
		RopeDespawned = 2;
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
			//// Ensure final position
			//currentRotation.x = -90.0f;
			//transform.SetRotation(glm::radians(currentRotation));
			//transform.SetPosition(initialPos + TargetPos);
		}
	}

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

void RopeBreaker::DropBridge()
{
	if (!bridgeAudio) 
	{
		g_Audio.PlayFileOnNewChannel("../BoofWoof/Assets/Audio/WoodenBridgeDropping.wav");
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
}


void RopeBreaker::SpawnBoneCatcher()
{
	// If collide with rope then load
	if (PlayerCollidedRope1 || PlayerCollidedRope2)
	{
		g_BoneCatcher.OnInitialize();
		BoneSpawned = true;
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
			if (PlayerCollidedRope1) 
			{
				if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Rope1")
				{
					g_Audio.PlayFileOnNewChannel("../BoofWoof/Assets/Audio/RopeSnap.wav");
					g_Coordinator.GetSystem<MyPhysicsSystem>()->RemoveEntityBody(entity);
					g_Coordinator.DestroyEntity(entity);
					RopeDespawned++;
				}
			}
			
			if (PlayerCollidedRope2)
			{
				if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Rope2")
				{
					g_Audio.PlayFileOnNewChannel("../BoofWoof/Assets/Audio/RopeSnap.wav");
					g_Coordinator.GetSystem<MyPhysicsSystem>()->RemoveEntityBody(entity);
					g_Coordinator.DestroyEntity(entity);
					RopeDespawned++;
				}
			}
		}		
	}
}
