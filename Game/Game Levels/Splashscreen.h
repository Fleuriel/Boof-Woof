#pragma once

#include "Level Manager/Level.h"
#include "ECS/Coordinator.hpp"

class Splashscreen : public Level 
{
	void LoadLevel() 
	{
		g_SceneManager.LoadScene("");
	}

	void InitLevel() { /* Empty by design */ }

	void UpdateLevel(double deltaTime) 
	{
		(void)deltaTime;

		// Press space to skip Splashscreen
		if (g_Input.GetKeyState(GLFW_KEY_SPACE) == 1) 
		{
			g_LevelManager.SetNextLevel("");
		}
	}

	void FreeLevel() { /*Empty by design*/ }

	void UnloadLevel() 
	{
		g_Coordinator.ResetEntities();
	}
};