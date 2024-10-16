#pragma once
#include "Level Manager/Level.h"
#include "ECS/Coordinator.hpp"

class Splashscreen : public Level 
{
	void LoadLevel() 
	{
		g_SceneManager.LoadScene("../BoofWoof/Assets/Scenes/Sphere_2.json");
	}

	void InitLevel() { /* Empty by design */ }

	void UpdateLevel(double deltaTime) 
	{
		(void)deltaTime;

		// Press space to skip Splashscreen
		if (g_Input.GetKeyState(GLFW_KEY_SPACE) >= 1) 
		{
			//g_LevelManager.SetNextLevel("../BoofWoof/Assets/Scenes/2Circle.json");
		}
	}

	void FreeLevel() { /*Empty by design*/ }

	void UnloadLevel() 
	{
		g_Coordinator.ResetEntities();
	}
};