#pragma once
#include "Level Manager/Level.h"
#include "ECS/Coordinator.hpp"

double elapsedTimee = 0.0;
double timeLimitt = 3.0;    // Set the time limit to 3 seconds

class Cutscene : public Level
{
	void LoadLevel()
	{
		g_SceneManager.LoadScene("../BoofWoof/Assets/Scenes/Cutscene.json");
	}

	void InitLevel() { /* Empty by design */ }

	void UpdateLevel(double deltaTime)
	{
		elapsedTimee += deltaTime;

		if (elapsedTimee >= timeLimitt)
		{
			g_LevelManager.SetNextLevel("Level1");
		}
	}

	void FreeLevel() { /*Empty by design*/ }

	void UnloadLevel()
	{
		g_Coordinator.ResetEntities();
		elapsedTimee = 0.0;
	}
};