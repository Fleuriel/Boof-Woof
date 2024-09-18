#pragma once

#include "../Core/ECS/Coordinator.h"
#include <iostream>
#include "../Core/ECS/Coordinator.h"
#include "../Utilities/Components/GraphicsComponent.h"
#include "Windows/WindowSystem.h"
#include "Graphics/GraphicsSystem.h"
#include "AssetManager/AssetManager.h"

#include <GLFW/glfw3.h>

class EngineCore 
{
public:
	void OnInit();
	void OnUpdate();
	void OnShutdown();
};

extern EngineCore* gCore;