#pragma once

#ifndef IMGUIEDITOR_H
#define IMGUIEDITOR_H
#define g_ImGuiEditor ImGuiEditor::GetInstance()

#ifdef MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS
#undef MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS
#endif

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
#include <ImGuizmo.h>
#include "AssetManager/Descriptor.h"

#include <filesystem>
#include <vector>

std::string GetScenesDir();

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


	void DrawEntityNode(Entity entity);
	void SetParent(Entity child, Entity parent);
	void RemoveParent(Entity child); 
	bool IsDescendantOf(Entity potentialParent, Entity child);
	void DeleteEntity(Entity entity);
	bool IsAncestorOf(Entity ancestor, Entity entity);



private:
	Window* m_Window = nullptr;
	std::vector<std::type_index> compTypes; // Member variable to hold selected component types
	std::filesystem::path m_SelectedFile;

	TextureDescriptor textureInfo;
	MaterialDescriptor materialInfo;

	glm::vec3 m_OldPosition;
	glm::vec3 m_OldRotationRadians;
	glm::vec3 m_OldScale;
	bool m_WasUsingGizmo = false;

};

#endif  // IMGUIEDITOR_H