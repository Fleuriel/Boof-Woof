#pragma once

#ifndef IMGUIEDITOR_H
#define IMGUIEDITOR_H
#define g_ImGuiEditor ImGuiEditor::GetInstance()

#include "../BoofWoof/Core/ECS/Coordinator.hpp"
#include "../BoofWoof/Utilities/Components/RenderTest.hpp"

class RenderTest;

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
	void WorldHierarchy();
	void InspectorWindow();

	// Variables
	Entity g_SelectedEntity = 0;

	static ImGuiEditor& GetInstance();
};

#endif  // IMGUIEDITOR_H