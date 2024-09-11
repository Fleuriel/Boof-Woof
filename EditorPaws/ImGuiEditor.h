#pragma once

#include <memory>

#define g_ImGuiEditor ImGuiEditor::GetInstance()

class ImGuiEditor {
public:
	ImGuiEditor() = default;
	~ImGuiEditor() = default;

	// parameter should have windows initialized from windowmanager
	void ImGuiInit();
	void ImGuiUpdate();
	void ImGuiRender();
	void ImGuiEnd();

	static ImGuiEditor& GetInstance();
};