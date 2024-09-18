// WindowComponent.h
#pragma once
#include "../Core/ECS/pch.h"
#include <GLFW/glfw3.h>
#include <utility> // For std::pair

class WindowComponent {

public:
    GLFWwindow* window;                    // GLFW window handle
    std::pair<int, int> windowSize;        // Window size (width, height)

    WindowComponent();
};