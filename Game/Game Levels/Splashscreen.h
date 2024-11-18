#pragma once
#include "Level Manager/Level.h"
#include "ECS/Coordinator.hpp"

double elapsedTime = 0.0;  
double timeLimit = 3.0;    // Set the time limit to 3 seconds

class Splashscreen : public Level 
{
	void LoadLevel() 
	{
		g_SceneManager.LoadScene("../BoofWoof/Assets/Scenes/SplashScreen.json");
	}

	void InitLevel() { /* Empty by design */ }

	void UpdateLevel(double deltaTime) 
	{
		elapsedTime += deltaTime;

		if (elapsedTime >= timeLimit)
		{
			g_LevelManager.SetNextLevel("MainMenu");
		}
	}

	void FreeLevel() { /*Empty by design*/ }

	void UnloadLevel() 
	{
		g_Coordinator.ResetEntities();
		elapsedTime = 0.0;
	}
};