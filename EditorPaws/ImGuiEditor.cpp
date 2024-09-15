#define MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS 0
#include "ImGuiEditor.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

// this part must be included last. Want to add anything new, add before this line
#include <GL/glew.h>
#include <glfw3.h>
#define GLM_FORCE_SILENT_WARNINGS
#include <glm/glm.hpp>
#include <iostream>

// Declare the window as extern (it is defined in main.cpp)
extern GLFWwindow* win;

ImGuiEditor& ImGuiEditor::GetInstance() {
	static ImGuiEditor instance{};
	return instance;
}

// parameter should have windows
void ImGuiEditor::ImGuiInit() {


	win = glfwCreateWindow(1920, 1080, "Dear ImGui Starter", NULL, NULL);
	if (!win)
	{
		std::cout << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		return;
	}


	// Tell GLFW we are using OpenGL 4.5
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);


	// Tell GLFW that we are using the CORE Profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create viewport of width and height.
	glViewport(0, 0, 1920,1080);

	glfwSwapInterval(1);

	// Receives Key input/output [Checks for Key Presses]
	//glfwSetKeyCallback(win, KeyCallBack);
	//
	//// Receives Mouse input/output [Checks for Mouse Clicks]
	//glfwSetMouseButtonCallback(win, MouseCallBack);
	//
	////glfwSetScrollCallback(newWindow, ScrollCallBack);
	////
	////glfwSetWindowFocusCallback(newWindow, windowFocusCallback);
	//
	//
	//glfwSetWindowSizeCallback(win, OpenGLWindowResizeCallback);

	// Make the current window the current context
	glfwMakeContextCurrent(win);

	// Set input mode for the window with the cursor (Enables Cursor Input)
	glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);


	glEnable(GL_DEPTH_TEST);

	glDepthRange(0.0f, 1.0f);

	bool glewInitialized = false;
	if (!glewInitialized)
	{
		GLenum err = glewInit();
		if (err != GLEW_OK)
		{
			std::cerr << "Error initializing GLEW: " << glewGetErrorString(err) << std::endl;
			return;
		}
	    glewInitialized = true;
	}

	IMGUI_CHECKVERSION();

	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();

	io.WantCaptureKeyboard = false;
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

	// ImGui::StyleColorsClassic(); // if you want it to be purple imGui

	ImGui_ImplGlfw_InitForOpenGL(win, true);

	ImGui_ImplOpenGL3_Init("#version 450");
}

void ImGuiEditor::ImGuiUpdate()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDemoWindow();
}

void ImGuiEditor::ImGuiRender() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	ImGuiIO& io = ImGui::GetIO();
	io.WantCaptureKeyboard = false;

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		auto* Window = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(Window);
	}
}

void ImGuiEditor::ImGuiEnd() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext(ImGui::GetCurrentContext());
}