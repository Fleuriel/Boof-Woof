#pragma once

#ifndef ENGINE_CORE_H
#define ENGINE_CORE_H

#include "../Core/ECS/Coordinator.hpp"
#include <iostream>
#include "../Utilities/Components/GraphicsComponent.h"
#include "Windows/WindowManager.h"
#include "Graphics/GraphicsSystem.h"
#include "AssetManager/AssetManager.h"
#include "../Utilities/Components/RenderTest.hpp"

#include <GLFW/glfw3.h>

class EngineCore 
{
public:
	void OnInit();
	void OnUpdate();
	void OnShutdown();
};

extern EngineCore* g_Core;

#endif