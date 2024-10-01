#define MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS 0
#include "ImGuiEditor.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <ImGuiFileDialog.h>

// this part must be included last. Want to add anything new, add before this line
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GLM_FORCE_SILENT_WARNINGS
#include <glm/glm.hpp>
#include <iostream>

namespace fs = std::filesystem;

//Helper function to locate save file directory
std::string GetScenesDir() {
	// Get current working directory (should be EditorPaws when running)
	std::filesystem::path currentPath = std::filesystem::current_path();

	// Go up one level (to reach BoofWoof)
	std::filesystem::path projectRoot = currentPath.parent_path();

	// Append "BoofWoof/Assets/Scenes" to the project root
	std::filesystem::path scenesPath = projectRoot / "BoofWoof" / "Assets" / "Scenes";

	return scenesPath.string();
}


ImGuiEditor& ImGuiEditor::GetInstance() {
	static ImGuiEditor instance{};
	return instance;
}

// parameter should have windows
void ImGuiEditor::ImGuiInit(Window* window)
{
	m_Window = window;

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
	// Start a new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Docking space
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

	//ImGui::ShowDemoWindow();

	ImGuiViewport();
	WorldHierarchy();
	InspectorWindow();
	AssetWindow();
	Settings();

	//// End the frame and render - this is in ImGuiRender()
	//ImGui::Render();
	//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

void ImGuiEditor::ImGuiViewport() {
	// Begin the ImGui Viewport window
	ImGui::Begin("Viewport");

	// Check if the ImGui window is docked and visible
	if (ImGui::IsWindowDocked())
	{
		// Get the size of the "Viewport" window
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

		// If the size changes, update the OpenGL viewport and framebuffer
		g_Coordinator.GetSystem<GraphicsSystem>()->SetEditorMode(true);

		g_Coordinator.GetSystem<GraphicsSystem>()->UpdateViewportSize(static_cast<int>(viewportPanelSize.x), static_cast<int>(viewportPanelSize.y));

		// Get framebuffer texture from GraphicsSystem
		GLuint texture = g_Coordinator.GetSystem<GraphicsSystem>()->GetFramebufferTexture();

		// Display the framebuffer texture in the ImGui viewport panel
		ImGui::Image((void*)(intptr_t)texture, viewportPanelSize, ImVec2(0, 1), ImVec2(1, 0));
	}

	ImGui::End();
}


void ImGuiEditor::WorldHierarchy()
{
	ImGui::Begin("World Hierarchy");
	{
		ImGui::Text(m_LastOpenedFile.c_str());
		ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

		if (g_Coordinator.GetTotalEntities() != MAX_ENTITIES)
		{
			if (ImGui::BeginPopupContextItem("GameObj"))
			{
				if (ImGui::Selectable("Empty GameObject"))
				{
					g_SelectedEntity = g_Coordinator.CreateEntity();

					// By default, add Transform and MetadataComponent (Identifier)
					if (!g_Coordinator.HaveComponent<MetadataComponent>(g_SelectedEntity)) 
					{
						g_Coordinator.AddComponent<MetadataComponent>(g_SelectedEntity, MetadataComponent("GameObject", g_SelectedEntity));
					}

					if (!g_Coordinator.HaveComponent<TransformComponent>(g_SelectedEntity)) 
					{
						g_Coordinator.AddComponent<TransformComponent>(g_SelectedEntity, TransformComponent());
					}
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

			ImGui::SameLine();

			if (ImGui::BeginPopupContextItem("Deletion"))
			{
				if (ImGui::Selectable("Delete Last Object"))
				{
					if (g_GettingDeletedEntity != MAX_ENTITIES)
					{
						g_Coordinator.DestroyEntity(g_GettingDeletedEntity);
					}
				}
				if (m_IsSelected)
				{
					if (ImGui::Selectable("Delete Selected Object"))
					{
						if (g_SelectedEntity != MAX_ENTITIES)
						{
							g_Coordinator.DestroyEntity(g_SelectedEntity);
							m_IsSelected = false;
						}
					}
				}
				ImGui::EndPopup();
			}

			if (ImGui::Button("Delete"))
			{
				ImGui::OpenPopup("Deletion");
			}
		}

		ImGui::Spacing(); ImGui::Spacing(); // to insert a gap so it looks visually nicer

		// Displaying hierarchy game objects - have to check for player next time
		const std::vector<Entity>& allEntities = g_Coordinator.GetAliveEntitiesSet();

		for (const auto& entity : allEntities)
		{
			// looping through to get EntityID, use the entityID to get the name of object		
			auto& name = g_Coordinator.GetComponent<MetadataComponent>(entity).GetName();

			ImGuiTreeNodeFlags nodeFlags = ((g_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | (ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);
			ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<intptr_t>(entity)), nodeFlags, name.c_str());

			if (ImGui::IsItemClicked())
			{
				m_IsSelected = true;
				g_SelectedEntity = entity;
			}

			g_GettingDeletedEntity = entity;
		}
	}
	ImGui::End();
}

void ImGuiEditor::InspectorWindow()
{
	static char fileNameBuffer[256] = "unnamed_scene"; // Default filename
	static bool showSavePopup = false;
	static bool showOverwritePopup = false;
	static std::string pendingFileName = ""; // File name being saved before overwrite check
	static std::string finalFileName = "";   // Final filename after checking
	static std::string filePath = "";        // Full file path
	static bool fileExists = false;          // To track if file exists

	ImGui::Begin("Inspector");
	{
		if (g_SelectedEntity < MAX_ENTITIES && g_SelectedEntity >= 0 && g_Coordinator.GetTotalEntities() != 0)
		{
			// Adding Components
			if (ImGui::BeginPopupContextItem("AComponents"))
			{
				if (ImGui::Selectable("TransformComponent"))
				{
					if (!g_Coordinator.HaveComponent<TransformComponent>(g_SelectedEntity))
					{
						g_Coordinator.AddComponent<TransformComponent>(g_SelectedEntity, TransformComponent());
					}
				}

				if (ImGui::Selectable("GraphicsComponent"))
				{
					if (!g_Coordinator.HaveComponent<GraphicsComponent>(g_SelectedEntity))
					{
						g_Coordinator.AddComponent<GraphicsComponent>(g_SelectedEntity, GraphicsComponent());
					}
				}

				ImGui::EndPopup();
			}

			if (ImGui::Button("Add Components"))
			{
				ImGui::OpenPopup("AComponents");
			}

			ImGui::SameLine();

			// Deleting Components
			if (ImGui::BeginPopupContextItem("DComponents"))
			{
				if (g_Coordinator.HaveComponent<TransformComponent>(g_SelectedEntity))
				{
					// in the future, when deleting transform component, physics component should also be deleted
					if (ImGui::Selectable("TransformComponent"))
					{
						g_Coordinator.RemoveComponent<TransformComponent>(g_SelectedEntity);
					}
				}

				if (g_Coordinator.HaveComponent<GraphicsComponent>(g_SelectedEntity))
				{
					if (ImGui::Selectable("GraphicsComponent"))
					{
						g_Coordinator.RemoveComponent<GraphicsComponent>(g_SelectedEntity);
					}
				}

				ImGui::EndPopup();
			}

			if (ImGui::Button("Delete Component"))
			{
				ImGui::OpenPopup("DComponents");
			}
		}

		// Metadata
		if (g_Coordinator.HaveComponent<MetadataComponent>(g_SelectedEntity))
		{
			if (ImGui::CollapsingHeader("Identifier", ImGuiTreeNodeFlags_None))
			{
				// Name
				auto& ObjName = g_Coordinator.GetComponent<MetadataComponent>(g_SelectedEntity).GetName();

				char entityNameBuffer[256];
				memset(entityNameBuffer, 0, sizeof(entityNameBuffer));
				strcpy_s(entityNameBuffer, sizeof(entityNameBuffer), ObjName.c_str());

				ImGui::Text("Name    "); ImGui::SameLine();
				ImGui::PushItemWidth(125.0f);
				if (ImGui::InputText("##ObjectName", entityNameBuffer, sizeof(entityNameBuffer)))
				{
					g_Coordinator.GetComponent<MetadataComponent>(g_SelectedEntity).SetName(std::string(entityNameBuffer));
				}
			}
		}

		// Transform
		if (g_Coordinator.HaveComponent<TransformComponent>(g_SelectedEntity))
		{
			if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_None))
			{
				//position
				auto& Position = g_Coordinator.GetComponent<TransformComponent>(g_SelectedEntity).GetPosition();
				ImGui::PushItemWidth(250.0f);
				ImGui::Text("Position"); ImGui::SameLine();

				if (ImGui::DragFloat3("##Position", static_cast<float*>(&Position.x), 0.5f))
				{
					g_Coordinator.GetComponent<TransformComponent>(g_SelectedEntity).SetPosition(Position);
				}

				//scale
				auto& Scale = g_Coordinator.GetComponent<TransformComponent>(g_SelectedEntity).GetScale();
				ImGui::Text("Scale   "); ImGui::SameLine();

				if (ImGui::DragFloat3("##Scale", static_cast<float*>(&Scale.x), 0.5f, 0.0f, FLT_MAX))
				{
					g_Coordinator.GetComponent<TransformComponent>(g_SelectedEntity).SetScale(Scale);
				}

				//rotation
				auto& rotation = g_Coordinator.GetComponent<TransformComponent>(g_SelectedEntity).GetRotation();
				ImGui::Text("Rotation"); ImGui::SameLine();

				if (ImGui::DragFloat3("##Rotation", static_cast<float*>(&rotation.x), 0.5f))
				{
					g_Coordinator.GetComponent<TransformComponent>(g_SelectedEntity).SetRotation(rotation);
				}
			}
		}

		if (g_Coordinator.HaveComponent<GraphicsComponent>(g_SelectedEntity))
		{
			if (ImGui::CollapsingHeader("Graphics", ImGuiTreeNodeFlags_None))
			{
				//	modelName - tempo
				auto modelName = g_Coordinator.GetComponent<GraphicsComponent>(g_SelectedEntity).getModel();
				const char* source = "";

				if (modelName == &g_AssetManager.ModelMap["sphere"])
				{
					source = "Sphere";
				}

				const char* modelNames[] = { "Sphere" };
				static int currentModel = 0;

				for (int i = 0; i < 6; ++i) {
					if (modelNames[i] == source) {
						currentModel = i;
					}
				}
				ImGui::PushItemWidth(123.0f);
				ImGui::Text("Model   "); ImGui::SameLine();
				if (ImGui::Combo("##ModelCombo", &currentModel, modelNames, 1))
				{
					if (currentModel == 0) modelName = &g_AssetManager.ModelMap["sphere"];
					g_Coordinator.GetComponent<GraphicsComponent>(g_SelectedEntity).SetModel(modelName);
				}

				// modelID
				auto modelID = g_Coordinator.GetComponent<GraphicsComponent>(g_SelectedEntity).getModelID();
				ImGui::Text("ModelID "); ImGui::SameLine();
				if (ImGui::DragInt("##ModelID", &modelID, 1))
				{
					g_Coordinator.GetComponent<GraphicsComponent>(g_SelectedEntity).SetModelID(modelID);
				}
			}
		}

		// Save & Load file portion	
		ImGui::Spacing();

		// Load button with file dialog
		if (ImGui::Button("Load"))
			ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".json", "../BoofWoof/Assets/Scenes/");

		if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
				size_t lastSlash = filePathName.find_last_of("/\\");
				m_LastOpenedFile = filePathName.substr(lastSlash + 1);

				g_SceneManager.LoadScene(filePathName);
			}
			ImGuiFileDialog::Instance()->Close();
		}

		ImGui::SameLine();

		if (ImGui::Button("Save Current World"))
		{
			showSavePopup = true;  // Show the popup when the button is pressed
		}

		// Create a popup for file naming and saving
		if (showSavePopup) {
			ImGui::OpenPopup("Save Scene As");
		}

		// Popup modal for saving the scene
		if (ImGui::BeginPopupModal("Save Scene As", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("Enter the name of the scene:");

			// Input text box for the user to enter the filename
			ImGui::InputText("##FileName", fileNameBuffer, IM_ARRAYSIZE(fileNameBuffer));

			if (ImGui::Button("Save")) {
				std::string fileName = fileNameBuffer;
				if (fileName.empty()) {
					fileName = "unnamed_scene"; // Default if empty
				}

				pendingFileName = fileName; // Save this file name to be processed

				// Check if the file exists
				filePath = GetScenesDir() + "/" + pendingFileName + ".json";
				fileExists = std::filesystem::exists(filePath);

				if (fileExists) {
					showOverwritePopup = true; // Show overwrite popup if file exists
				}
				else {
					// File doesn't exist, save directly
					finalFileName = pendingFileName;
					g_SceneManager.SaveScene(finalFileName + ".json");
					ImGui::OpenPopup("Saved");
				}

				showSavePopup = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel")) {
				showSavePopup = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		// Overwrite Confirmation Popup
		if (showOverwritePopup) {
			ImGui::OpenPopup("Overwrite Confirmation");
		}

		// Popup to confirm overwriting
		if (ImGui::BeginPopupModal("Overwrite Confirmation", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("File already exists. Do you want to overwrite it?");
			if (ImGui::Button("Yes")) {
				// Overwrite the file
				finalFileName = pendingFileName;
				g_SceneManager.SaveScene(finalFileName + ".json");
				ImGui::OpenPopup("Saved");

				showOverwritePopup = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button("No")) {
				// Append a number to the filename and save
				int counter = 1;
				finalFileName = pendingFileName;
				while (std::filesystem::exists(GetScenesDir() + "/" + finalFileName + ".json")) {
					finalFileName = pendingFileName + "_" + std::to_string(counter);
					counter++;
				}
				g_SceneManager.SaveScene(finalFileName + ".json");
				ImGui::OpenPopup("Saved");

				showOverwritePopup = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		// Confirmation popup when the scene is saved
		if (ImGui::BeginPopupModal("Saved", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("\nYour world has been saved!\n\n");
			ImGui::Separator();
			if (ImGui::Button("OK", ImVec2(50, 0))) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		// Failed popup if saving failed
		if (ImGui::BeginPopupModal("Failed", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("\nSaving failed! Try again.\n\n");
			ImGui::Separator();
			if (ImGui::Button("OK", ImVec2(50, 0))) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		ImGui::SameLine();
		/*if (ImGui::Button("Save New World"))
		{
		g_Json.m_SaveCounter++;
		std::stringstream ss;
		ss << "../BoofWoof/Saves/SaveWorld" << g_Json.m_SaveCounter << ".json";

		g_Json.JsonSerializeNew(ss.str());

		ImGui::OpenPopup("Saved");
		}*/
		ImGui::End();
	}

}


// Asset Window is incomplete cause no Asset Manager yet
void ImGuiEditor::AssetWindow()
{
	ImGui::Begin("Asset Browser");
	{
		std::string entireFilePath = m_BaseDir.relative_path().string();

		if (m_CurrDir != fs::path(m_BaseDir))
		{
			entireFilePath = m_CurrDir.relative_path().string();
			if (ImGui::Button("<-"))
			{
				m_CurrDir = m_CurrDir.parent_path();
			}
		}

		ImGui::SameLine();
		ImGui::Text(entireFilePath.c_str());

		// Calculate the space needed for the buttons
		float windowWidth = ImGui::GetWindowWidth();
		float buttonWidth = ImGui::CalcTextSize("Delete Asset").x + ImGui::GetStyle().FramePadding.x * 2;
		float availableSpace = windowWidth - ImGui::CalcTextSize(entireFilePath.c_str()).x - buttonWidth * 2 - ImGui::GetStyle().ItemSpacing.x * 3;

		// subtract the width of the buttons and spacing from the available space to align to the right
		ImGui::SameLine(availableSpace);
		if (ImGui::Button("Add Asset"))
		{
			ImGuiFileDialog::Instance()->OpenDialog("AddAsset", "Choose File", ".png,.mp3,.wav,.csv,.json,.ttf,.vert,.frag", "../BoofWoof/Assets/");
		}

		ImGui::SameLine();
		if (ImGui::Button("Delete Asset"))
		{
			ImGuiFileDialog::Instance()->OpenDialog("DeleteAsset", "Choose File", ".png,.mp3,.wav,.csv,.json,.ttf,.vert,.frag", "../BoofWoof/Assets/");
		}

		if (ImGuiFileDialog::Instance()->Display("AddAsset"))
		{
			// Check if the user made a selection
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				// Get the selected file path
				std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

				// Asset manager to add in the assets e.g. g_AssetManager.AddAssets(filePathName);
			}

			// close
			ImGuiFileDialog::Instance()->Close();
		}

		if (ImGuiFileDialog::Instance()->Display("DeleteAsset"))
		{
			// Check if the user made a selection
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				// Get the selected file path
				std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

				// find last / to get file name only
				size_t lastSlash = filePathName.find_last_of("/\\");
				std::string deleteFileName = filePathName.substr(lastSlash + 1);

				// Asset manager to delete the assets e.g. g_AssetManager.DeleteAssets(deleteFileName);
			}

			// close
			ImGuiFileDialog::Instance()->Close();
		}

		ImGui::Spacing();  ImGui::Separator(); ImGui::Spacing();

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int colCount = (int)(panelWidth / 100);
		if (colCount < 1) colCount = 1;	// to ensure that it will not go down to 0 when u push the column smaller than 1 size

		ImGui::Columns(colCount, 0, false);	// for resizing purposes

		for (auto& entry : fs::directory_iterator(m_CurrDir))
		{
			const auto& path = entry.path();
			auto relativePath = fs::relative(path, m_BaseDir);
			std::string fileNameExt = relativePath.filename().string();

			ImGui::PushID(fileNameExt.c_str()); // using filename as the ID so all are unique, the drag drop won't open same source
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0)); // for button background opacity to go 0

			size_t lastDot = fileNameExt.find_last_of(".");
			std::string fileName = fileNameExt.substr(0, lastDot);
			std::string fileExtension = fileNameExt.substr(lastDot + 1);
			std::string icon = entry.is_directory() ? "FolderIcon" : (fileExtension == "png" ? fileName : "TextIcon");

			//ImGui::ImageButton((ImTextureID)(uintptr_t)g_AssetManager.GetTexture(icon), { 60,60 }, { 0,1 }, { 1,0 });

			// drag from assets to components
			/*if (ImGui::BeginDragDropSource())
			{
				fs::path outerRelativePath = path.c_str();
				const wchar_t* itemPath = outerRelativePath.c_str();
				ImGui::SetDragDropPayload("Ass", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();
			}*/

			ImGui::PopStyleColor();

			// must double click to go next directory
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (entry.is_directory())
				{
					// showing folders
					m_CurrDir /= path.filename();
				}

			}

			ImGui::TextWrapped(fileName.c_str());

			ImGui::NextColumn();

			ImGui::PopID();
		}

		ImGui::Columns(1);
		ImGui::End();
	}
}

void ImGuiEditor::Settings()
{
	ImGui::Begin("Settings");
	{
		ImGui::SeparatorText("Configurations");

		// Window Size, Frame Rate, Frame Count, Camera Position (future)
		ImGui::Text("Window Size: %d x %d", g_Window->GetWindowWidth(), g_Window->GetWindowHeight());
		ImGui::Text("Frame Rate: %f", g_Window->GetFPS());
		ImGui::Text("Frame Count: %d", g_Core->m_CurrNumSteps);

		ImGui::Spacing();

		ImGui::SeparatorText("System DT (% of Total Game Loop)");

		//ImGui::Text("Graphics DT: %f", ((g_Core->m_GraphicsDT / g_Core->m_ElapsedDT) * 100));
		PlotSystemDT("Graphics DT", static_cast<float>((g_Core->m_GraphicsDT / g_Core->m_ElapsedDT) * 100), static_cast<float>(g_Core->m_ElapsedDT));
	}

	ImGui::End();
}

void ImGuiEditor::PlotSystemDT(const char* name, float dt, float totalDT)
{
	static_cast<void>(totalDT);

	// keep track of values array and offset for each system based on their names
	static std::map<std::string, std::pair<int, float[90]>> data;

	// Get the values and offset for this system
	auto& [values_offset, values] = data[name];

	// Append the current dt to the values array
	values[values_offset] = dt;
	values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);

	// Create overlay string
	char overlay[32];

	sprintf_s(overlay, "%f", dt);
	// Plot lines	
	ImGui::PlotLines(name, values, IM_ARRAYSIZE(values), values_offset, overlay, FLT_MAX, FLT_MAX, ImVec2(0, 40.0f));
}
