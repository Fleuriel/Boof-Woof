#pragma once

#ifndef IMGUIEDITOR_H
#define IMGUIEDITOR_H
#define g_ImGuiEditor ImGuiEditor::GetInstance()

#include "../BoofWoof/Core/ECS/Coordinator.hpp"
#include "../BoofWoof/Core/Windows/WindowManager.h"

class ImGuiEditor 
{
public:
	ImGuiEditor() = default;
	~ImGuiEditor() = default;

	void ImGuiInit(Window* window);
	void ImGuiUpdate();
	void ImGuiRender();
	void ImGuiEnd();

	// Panels
	void WorldHierarchy();
	void InspectorWindow();

	// Variables
	Entity g_SelectedEntity = 0;

	static ImGuiEditor& GetInstance();

private:
	Window* m_Window = nullptr;
};

#endif  // IMGUIEDITOR_H