#pragma once
#include "../ECS/pch.h"
#include "../ECS/System.h"
#include "../Utilities/Components/WindowComponent.h"
#include <iostream>

class WindowSystem : public System
{
public:
    WindowSystem();   // Constructor
    ~WindowSystem();  // Destructor

    void initWindow(WindowComponent& windowComponent);
    static void OpenGLWindowResizeCallback(GLFWwindow* window, int width, int height);

    // Add other callback registrations if needed
    GLFWwindow* GetGLFWWindow();


private:
    static void KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);  // Example callback
    static void MouseCallBack(GLFWwindow* window, int button, int action, int mods);           // Example callback
    GLFWwindow* m_Window{};
};

extern WindowSystem* g_Window;