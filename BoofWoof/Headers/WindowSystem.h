#pragma once
#include "pch.h"
#include "System.h"
#include "WindowComponent.h"
#include <iostream>

class WindowSystem {
public:
    WindowSystem();   // Constructor
    ~WindowSystem();  // Destructor

    void initWindow(WindowComponent& windowComponent);
    static void OpenGLWindowResizeCallback(GLFWwindow* window, int width, int height);

    // Add other callback registrations if needed
private:
    static void KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);  // Example callback
    static void MouseCallBack(GLFWwindow* window, int button, int action, int mods);           // Example callback
};