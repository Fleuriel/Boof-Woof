#include "pch.h"
#include "../Core/Logic/Behaviour.h"
#include "../Core/ECS/Coordinator.hpp"

namespace Null {
	inline void Start(Entity entity) 
	{
		(void)entity;
		//std::cout << "Null Behaviour Started" << std::endl;

	}

	inline void Update(Entity entity) 
	{
		(void)entity;
		//std::cout << "Null Behaviour Updated" << std::endl;
	}

	inline void Destroy(Entity entity) 
	{
		(void)entity;
		//std::cout << "Null Behaviour Destroyed" << std::endl;
	}
}