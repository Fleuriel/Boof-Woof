#pragma once

#include <memory>

#define g_ImGuiEditor ImGuiEditor::GetInstance()

class ImGuiEditor 
{
public:
	ImGuiEditor() = default;
	~ImGuiEditor() = default;

	// parameter should have windows initialized from windowmanager
	void ImGuiInit();
	void ImGuiUpdate();
	void ImGuiRender();
	void ImGuiEnd();

	// Panels
	void InspectorWindow();

	// Variables
	//Entity g_SelectedEntity = 0;

	static ImGuiEditor& GetInstance();
};