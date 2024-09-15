#include "../Headers/TestSystem.h"
#include <iostream>

//extern Coordinator gCoordinator;

void TestSystem::Update(float dt)
{
	int temp = 0;
	std::cout << "Test System Update: " << std::endl;

	// this part not entering
	for (auto const& entity : mEntities)
	{
		std::cout << "PRINTTTTTTTTTTTTTTTTT" << std::endl;
		auto& testcomponent = g_Coordinator.GetComponent<TestComponent>(entity);
		temp++;
		std::cout << temp << ": " << testcomponent.data << std::endl;
	}
}