#pragma once
#include "Level Manager/Level.h"
#include "ECS/Coordinator.hpp"
#include "../BoofWoof/Core/AssetManager/FilePaths.h"

double elapsedTime = 0.0;
double timeLimit = 3.0;    // Set the time limit to 3 seconds

class TYVM : public Level
{
	void LoadLevel()
	{
		// Use FILEPATH_ASSET_SCENES to construct the scene file path
		g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES + "/TYVM.json");
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

	void FreeLevel() { /* Empty by design */ }

	void UnloadLevel()
	{
		g_Coordinator.ResetEntities();
		elapsedTime = 0.0;
	}
};
