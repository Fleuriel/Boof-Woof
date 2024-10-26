#include "pch.h"
#include "../Core/Logic/Behaviour.h"
#include "../Core/ECS/Coordinator.hpp"
#include "../Core/Input/Input.h"


namespace Collectables
{
	inline void Start(Entity entity) 
	{
		(void)entity;
		std::cout << "Collectables Behaviour Started" << std::endl;
	}

	inline void Update(Entity entity) {
		// Check if player is colliding with collectable

		// If player is colliding with collectable, destroy collectable

		// Add score to player

		(void)entity;
	}

	inline void Destroy(Entity entity) 
	{
		(void)entity;
		std::cout << "Collectables Behaviour Destroyed" << std::endl;
	}

}
