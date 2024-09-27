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

	ImGui::ShowDemoWindow();

	ImGuiViewport();
	WorldHierarchy();
	InspectorWindow();


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
		ImGui::Text("FutureFileNameWillBeHere");
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
					if (ImGui::Selectable("TransformComponent"))
					{
						g_Coordinator.RemoveComponent<TransformComponent>(g_SelectedEntity);
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

				if (modelName == &g_AssetManager.Models[0])
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
					if (currentModel == 0) modelName = &g_AssetManager.Models[0];
					g_Coordinator.GetComponent<GraphicsComponent>(g_SelectedEntity).SetModel(modelName);
				}

				// modelID
				auto modelID = g_Coordinator.GetComponent<GraphicsComponent>(g_SelectedEntity).getModelID();
				ImGui::Text("ModelID "); ImGui::SameLine();
				/*ImGui::InputInt*/
				if (ImGui::DragInt("##ModelID", &modelID, 1)) 
				{
					g_Coordinator.GetComponent<GraphicsComponent>(g_SelectedEntity).SetModelID(modelID);
				}
			}
		}

		ImGui::End();
	}
}