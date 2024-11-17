#pragma once

#ifndef IMGUIEDITOR_H
#define IMGUIEDITOR_H
#define g_ImGuiEditor ImGuiEditor::GetInstance()

#include "../BoofWoof/Core/ECS/Coordinator.hpp"
#include "../BoofWoof/Core/Windows/WindowManager.h"
#include "../BoofWoof/Utilities/Components/TransformComponent.hpp"
#include "../BoofWoof/Utilities/Components/MetaData.hpp"
#include "../BoofWoof/Core/SceneManager/SceneManager.h"
#include "EngineCore.h"
#include "../BoofWoof/Utilities/Components/AudioComponent.hpp"
#include "../BoofWoof/Utilities/Components/BehaviourComponent.hpp"
#include "UndoRedoManager.hpp"
#include "ArchetypeManager.hpp"

#include <filesystem>

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
	void Settings();
	void Scenes();
	void Audio();
	void PlayStopRunBtn();
	void ArchetypeTest();
	void ShowPickingDebugWindow();

	// PlayStopRun Panel
	enum class States
	{
		Play,
		Stop,
		Pause
	};
	States m_State = States::Stop;

	// Audio Panel
	std::string m_AudioName{};
	bool m_ShowAudio = false;

	// For fun
	void PlotSystemDT(const char* name, float dt, float totalDT);

	// Variables
	Entity g_SelectedEntity = 0;
	Entity g_GettingDeletedEntity = static_cast<Entity>(-1);		// will be deleted from the back
		
	std::filesystem::path m_BaseDir = "../BoofWoof/Assets";			// Asset Panel
	std::filesystem::path m_CurrDir = m_BaseDir;
	std::string m_LastOpenedFile{};

	// For saving files
	std::string m_FileName{};
	std::string m_FinalFileName{};
	std::string m_FilePath{};

	bool m_IsSelected{ false };
	bool m_PlayerExist{ false };

	static ImGuiEditor& GetInstance();

private:
	Window* m_Window = nullptr;
	std::vector<std::type_index> compTypes; // Member variable to hold selected component types

};

#endif  // IMGUIEDITOR_H