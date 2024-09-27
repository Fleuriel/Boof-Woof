#pragma once

#ifndef IMGUIEDITOR_H
#define IMGUIEDITOR_H
#define g_ImGuiEditor ImGuiEditor::GetInstance()

#include "../BoofWoof/Core/ECS/Coordinator.hpp"
#include "../BoofWoof/Core/Windows/WindowManager.h"
#include "../BoofWoof/Utilities/Components/TransformComponent.hpp"
#include "../BoofWoof/Utilities/Components/MetaData.hpp"
#include "Serialization/Serialization.h"

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
	void ImGuiViewport();
	void WorldHierarchy();
	void InspectorWindow();
	void AssetWindow();

	// Variables
	Entity g_SelectedEntity = 0;
	Entity g_GettingDeletedEntity = static_cast<Entity>(-1);		// will be deleted from the back
	
	std::string m_LastOpenedFile{};									// Saving & Loading new files
	
	std::filesystem::path m_BaseDir = "../BoofWoof/Assets";			// Asset Panel
	std::filesystem::path m_CurrDir = m_BaseDir;

	bool m_IsSelected{ false };

	static ImGuiEditor& GetInstance();

private:
	Window* m_Window = nullptr;
};

#endif  // IMGUIEDITOR_H