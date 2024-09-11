#define MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS 0
#include <Windows.h>
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

	win = glfwCreateWindow(1280, 720, "Dear ImGui Starter", NULL, NULL);
	if (!win)
	{
		std::cout << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		return;
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