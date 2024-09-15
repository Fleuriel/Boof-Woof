#pragma once

#include "../Core/ECS/Coordinator.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include "../Headers/TestComponent.h"
#include "../Headers/TestSystem.h"

class EngineCore 
{
public:
	void OnInit();
	void OnUpdate();
	void OnShutdown();
};

extern EngineCore* gCore;