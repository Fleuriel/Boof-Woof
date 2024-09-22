#pragma once

#ifndef ENGINE_CORE_H
#define ENGINE_CORE_H

#include "../Core/ECS/Coordinator.h"
#include <iostream>
#include "../Core/ECS/Coordinator.h"
#include "../Utilities/Components/GraphicsComponent.h"
#include "Windows/WindowManager.h"
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

#endif