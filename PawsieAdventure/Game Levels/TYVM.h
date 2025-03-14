#pragma once
#include "Level Manager/Level.h"
#include "ECS/Coordinator.hpp"
#include "../BoofWoof/Core/AssetManager/FilePaths.h"

class TYVM : public Level
{
	double elapsedTime = 0.0;
	double timeLimit = 10.0;    // Set the time limit to 10 seconds

	void LoadLevel() override
	{
		// Use FILEPATH_ASSET_SCENES to construct the scene file path
		g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES + "/TYVM.json");
	}

	void InitLevel() override { /* Empty by design */ }

	void UpdateLevel(double deltaTime) override
	{
		elapsedTime += deltaTime;

		if (elapsedTime >= timeLimit)
		{
			g_LevelManager.SetNextLevel("MainMenu");
			g_Window->ShowMouseCursor();
		}

		// ESC to exit tyvm - zero feedback as of now.
		if (g_Input.GetKeyState(GLFW_KEY_ESCAPE) == 1)
		{
			// Thank you scene
			g_LevelManager.SetNextLevel("MainMenu");
			g_Window->ShowMouseCursor();
		}
	}

	void FreeLevel() override { /* Empty by design */ }

	void UnloadLevel() override
	{
		g_Coordinator.ResetEntities();
		elapsedTime = 0.0;
	}
};
