#pragma once

#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include "../ECS/pch.h"
#include "../ECS/System.h"
#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <sstream>

// Forward declare the GLFWwindow type so that everytime you spawn window, 
// it won't have the entire GLFW lib in the space
struct GLFWwindow;
extern int g_WindowX, g_WindowY;

class Window
{
public:

    Window() { }

    // constructing window class with width n height provided along with window title
    Window(int width, int height, const char* title) : m_Width(width), m_Height(height), m_Title(title), m_Window(nullptr), m_IsInitialized(true) { }

	// init GLFW window
	void OnInitialize();

	// This function tracks the fps and swap buffer
	void OnUpdate();

	// This terminate the glfw window
	void OnShutdown();

	// Setters
	void SetWindowTitle(const char* name);
	void SetWindowWidth(int width) { m_Width = width; }
	void SetWindowHeight(int height) { m_Height = height; }

	// Getters
	GLFWwindow* GetGLFWWindow();
	float GetAspectRatio();
	int GetWindowWidth() { return m_Width; }
	int GetWindowHeight() { return m_Height; }
	double GetFPS() { return m_FPS; }
	int GetFrameCount() { return m_FrameCount; }
	double GetDeltaTime() { return m_DeltaTime; }
	bool GetAltStat() { return m_AltTab; };

	// void initWindow(WindowComponent& windowComponent);
	static void OpenGLWindowResizeCallback(GLFWwindow* window, int width, int height);

private:
    static void KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);  // Example callback
    static void MouseCallBack(GLFWwindow* window, int button, int action, int mods);           // Example callback

	void OnUpdateFPS(double interval);

private:
	int m_Width{};
	int m_Height{};
	float m_AspectRatio{};
	const char* m_Title{};
	double m_FPS{};
	int m_FrameCount{};
	double m_DeltaTime{};
	GLFWwindow* m_Window{};
	bool m_IsInitialized{};
	bool m_FullScreen{};
	bool m_AltTab{};
};

extern Window* g_Window;

#endif