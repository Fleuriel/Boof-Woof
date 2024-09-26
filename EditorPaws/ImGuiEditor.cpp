#define MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS 0
#include "ImGuiEditor.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

// this part must be included last. Want to add anything new, add before this line
#include <GL/glew.h>
#include <GLFW/glfw3.h>
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
void ImGuiEditor::ImGuiInit(Window* window)
{
	m_Window = window;

	//win = glfwCreateWindow(1280, 1024, "Dear ImGui Starter", NULL, NULL);
	//if (!win)
	//{
	//	std::cout << "Failed to create GLFW window!" << std::endl;
	//	glfwTerminate();
	//	return;
	//}

	//// Tell GLFW we are using OpenGL 4.5
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

	//// Tell GLFW that we are using the CORE Profile
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//// Create viewport of width and height.
	//glViewport(0, 0, 1920,1080);
	//glfwSwapInterval(1);

	//// Make the current window the current context
	//glfwMakeContextCurrent(win);

	//// Set input mode for the window with the cursor (Enables Cursor Input)
	//glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	//glEnable(GL_DEPTH_TEST);
	//glDepthRange(0.0f, 1.0f);

	//bool glewInitialized = false;
	//if (!glewInitialized)
	//{
	//	GLenum err = glewInit();
	//	if (err != GLEW_OK)
	//	{
	//		std::cerr << "Error initializing GLEW: " << glewGetErrorString(err) << std::endl;
	//		return;
	//	}
	//    glewInitialized = true;
	//}

	IMGUI_CHECKVERSION();

	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();

	io.WantCaptureKeyboard = false;
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

	ImGui_ImplGlfw_InitForOpenGL(window->GetGLFWWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 450");
}

void ImGuiEditor::ImGuiUpdate()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Dock the ImGui windows
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

	ImGui::ShowDemoWindow();

	// Panels
	WorldHierarchy();
	InspectorWindow();
}

void ImGuiEditor::ImGuiRender() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	ImGuiIO& io = ImGui::GetIO();
	io.WantCaptureKeyboard = false;

	////these two needs to be at the end.
	//glfwSwapBuffers(win);
	//glfwPollEvents();

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

void ImGuiEditor::WorldHierarchy()
{
	ImGui::Begin("World Hierarchy");
	{
		/*if (g_Coordinator.GetTotalEntities() != MAX_ENTITIES)
		{
			if (ImGui::BeginPopupContextItem("GameObj"))
			{
				if (ImGui::Selectable("Empty GameObject"))
				{
					g_SelectedEntity = g_Coordinator.CreateEntity();
				}
				ImGui::EndPopup();
			}

			if (ImGui::Button("Create"))
			{
				ImGui::OpenPopup("GameObj");
			}

			ImGui::SameLine();

			if (ImGui::Button("Clone Object") && g_Coordinator.GetTotalEntities() != 0) {

				if (g_SelectedEntity != MAX_ENTITIES)
				{
					Entity clone = g_Coordinator.CloneEntity(g_SelectedEntity);
					g_SelectedEntity = clone;
				}
			}
		}*/
	}
	ImGui::End();
}

void ImGuiEditor::InspectorWindow()
{
	ImGui::Begin("Inspector"); 
	{
		//if (g_SelectedEntity < MAX_ENTITIES && g_SelectedEntity >= 0 && g_Coordinator.GetTotalEntities() != 0) 
		//{
		//	// Adding Components
		//	if (ImGui::BeginPopupContextItem("AComponents"))
		//	{
		//		if (ImGui::Selectable("RenderTest"))
		//		{
		//			if (!g_Coordinator.HaveComponent<RenderTest>(g_SelectedEntity))
		//			{
		//				g_Coordinator.AddComponent<RenderTest>(g_SelectedEntity, RenderTest());
		//			}
		//		}

		//		ImGui::EndPopup();
		//	}

		//	if (ImGui::Button("Add Components"))
		//	{
		//		ImGui::OpenPopup("AComponents");
		//	}

		//	ImGui::SameLine();

		//	// Deleting Components
		//	if (ImGui::BeginPopupContextItem("DComponents"))
		//	{
		//		if (g_Coordinator.HaveComponent<RenderTest>(g_SelectedEntity))
		//		{
		//			if (ImGui::Selectable("RenderTest"))
		//			{
		//				g_Coordinator.RemoveComponent<RenderTest>(g_SelectedEntity);
		//			}
		//		}

		//		ImGui::EndPopup();
		//	}

		//	if (ImGui::Button("Delete Component"))
		//	{
		//		ImGui::OpenPopup("DComponents");
		//	}
		//}

		ImGui::End();
	}
}