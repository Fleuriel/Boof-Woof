#pragma once

#include "Level Manager/Level.h"
#include "ECS/Coordinator.hpp"

class Level1 : public Level
{
	void LoadLevel()
	{
		g_SceneManager.LoadScene("../BoofWoof/Assets/Scenes/TestScene.json");
	}

	void InitLevel() { /* Empty by design */ }

	void UpdateLevel(double deltaTime)
	{
		std::cout << "Mouse Pos x " << g_Input.GetMousePosition().x << " ,y " << g_Input.GetMousePosition().y << std::endl;		

		// Space to go back mainmenu
		if (g_Input.GetKeyState(GLFW_KEY_SPACE) >= 1)
		{
			g_LevelManager.SetNextLevel("MainMenu");
		}
	}

	void FreeLevel() { /*Empty by design*/ }

	void UnloadLevel()
	{
		g_Coordinator.ResetEntities();
	}
};