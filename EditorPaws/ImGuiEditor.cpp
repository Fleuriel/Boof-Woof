﻿#define MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS 0
#include <Windows.h>
#include "ImGuiEditor.h"
#ifdef MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS
#undef MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS
#endif
#include <Windows.h>
#include "ImGuiEditor.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <ImGuiFileDialog.h>
#include "ResourceManager/ResourceManager.h"
#include "AssetManager/FilePaths.h"
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include "../Utilities/Components/AnimationComponent.hpp"
#include "../Utilities/Components/MaterialComponent.hpp"
#include <glm/gtx/matrix_decompose.hpp>



enum s_shader {

	S_FONT,
	S_INST


};


namespace fs = std::filesystem;

constexpr float EPSILON = 1e-6f;

//AnimationManager g_AnimationManager;

//Helper function to locate save file directory
std::string GetScenesDir()
{
	// Get current working directory (should be EditorPaws when running)
	fs::path currentPath = fs::current_path();

	// Go up one level (to reach BoofWoof)
	fs::path projectRoot = currentPath.parent_path();

	// Append "BoofWoof/Assets/Scenes" to the project root
	fs::path scenesPath = projectRoot / "BoofWoof" / "Assets" / "Scenes";

	return scenesPath.string();
}


bool AreVectorsEqual(const glm::vec3& v1, const glm::vec3& v2, float epsilon = EPSILON)
{
	return glm::all(glm::epsilonEqual(v1, v2, epsilon));
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


	g_Coordinator.GetSystem<GraphicsSystem>()->clearAllEntityTextures();
}

void ImGuiEditor::ImGuiUpdate()
{
	// Start a new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Initialize ImGuizmo
	ImGuizmo::BeginFrame();

	// Docking space
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

	//ImGui::ShowDemoWindow();
	ImGuiViewport();

	ArchetypeTest();

	WorldHierarchy();
	InspectorWindow();
	AssetWindow();
	Settings();
	Scenes();
	PlayStopRunBtn();
	//ShowPickingDebugWindow();



	if (m_ShowAudio)
	{
		Audio();
	}

}


void ImGuiEditor::ImGuiRender() {
//	glDisable(GL_FRAMEBUFFER_SRGB);
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//	glEnable(GL_FRAMEBUFFER_SRGB);
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

		// Update the OpenGL viewport and framebuffer
		g_Coordinator.GetSystem<GraphicsSystem>()->SetEditorMode(true);
		g_Coordinator.GetSystem<GraphicsSystem>()->UpdateViewportSize(static_cast<int>(viewportPanelSize.x), static_cast<int>(viewportPanelSize.y));

		// Get the position where the image will be drawn
		ImVec2 viewportPanelPos = ImGui::GetCursorScreenPos();

		// Get framebuffer texture from GraphicsSystem
		GLuint texture = g_Coordinator.GetSystem<GraphicsSystem>()->GetFramebufferTexture();

		// Display the framebuffer texture in the ImGui viewport panel
		ImGui::Image((void*)(intptr_t)texture, viewportPanelSize, ImVec2(0, 1), ImVec2(1, 0));

		// Set up ImGuizmo
		ImGuizmo::SetOrthographic(false); // Assuming you are using perspective projection
		ImGuizmo::SetDrawlist();

		// Set the ImGuizmo rect to match the viewport
		ImGuizmo::SetRect(viewportPanelPos.x, viewportPanelPos.y, viewportPanelSize.x, viewportPanelSize.y);

		glm::mat4 view = g_Coordinator.GetSystem<GraphicsSystem>()->GetCamera().GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), viewportPanelSize.x / viewportPanelSize.y, 0.1f, 100.0f);

		// Get the transform matrix of the selected entity
		if (g_SelectedEntity != MAX_ENTITIES && g_Coordinator.HaveComponent<TransformComponent>(g_SelectedEntity))
		{
			auto& transformComp = g_Coordinator.GetComponent<TransformComponent>(g_SelectedEntity);

			if (g_Coordinator.HaveComponent<CollisionComponent>(g_SelectedEntity)) 
			{
				auto& collisionComp = g_Coordinator.GetComponent<CollisionComponent>(g_SelectedEntity);
			}

			// Get the TransformSystem instance
			std::shared_ptr<TransformSystem> transformSystem = g_Coordinator.GetSystem<TransformSystem>();

			// Get the object's world matrix from TransformSystem
			glm::mat4 modelMatrix = transformSystem->GetWorldMatrix(g_SelectedEntity);

			// Define the operation and mode
			static ImGuizmo::OPERATION mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
			static ImGuizmo::MODE mCurrentGizmoMode = ImGuizmo::LOCAL;

			// Handle ImGuizmo manipulation
			ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection),
				mCurrentGizmoOperation, mCurrentGizmoMode, glm::value_ptr(modelMatrix));

			// Get the current ImGuizmo usage state
			bool isUsingGizmo = ImGuizmo::IsUsing();

			if (isUsingGizmo)
			{
				// Disable physics override
				transformComp.SetEditing(true);

				// Get the parent's world matrix
				glm::mat4 parentWorldMatrix = glm::mat4(1.0f);
				if (g_Coordinator.HaveComponent<HierarchyComponent>(g_SelectedEntity))
				{
					Entity parent = g_Coordinator.GetComponent<HierarchyComponent>(g_SelectedEntity).parent;
					if (parent != MAX_ENTITIES)
					{
						parentWorldMatrix = transformSystem->GetWorldMatrix(parent);
					}
				}

				// Compute the new local matrix
				glm::mat4 newLocalMatrix = glm::inverse(parentWorldMatrix) * modelMatrix;

				// Decompose the new local matrix to get local position, rotation, scale
				glm::vec3 newLocalPosition, newLocalRotationDegrees, newLocalScale;
				ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(newLocalMatrix),
					glm::value_ptr(newLocalPosition), glm::value_ptr(newLocalRotationDegrees), glm::value_ptr(newLocalScale));

				glm::vec3 newLocalRotationRadians = glm::radians(newLocalRotationDegrees);

				// If manipulation has just started
				if (!m_WasUsingGizmo)
				{

					// Store old local transform values
					m_OldPosition = transformComp.GetPosition();
					m_OldRotationRadians = transformComp.GetRotation();
					m_OldScale = transformComp.GetScale();
					
				}

				// Update the TransformComponent with new local values
				transformComp.SetPosition(newLocalPosition);
				transformComp.SetRotation(newLocalRotationRadians);
				transformComp.SetScale(newLocalScale);
			}
			else if (m_WasUsingGizmo) // Manipulation has just ended
			{
				// Re-enable physics control
				transformComp.SetEditing(false);

				// Retrieve the new local transform values
				glm::vec3 newPosition = transformComp.GetPosition();
				glm::vec3 newRotationRadians = transformComp.GetRotation();
				glm::vec3 newScale = transformComp.GetScale();

				Entity entity = g_SelectedEntity;

				// Store copies of the old values
				glm::vec3 oldPosition = m_OldPosition;
				glm::vec3 oldRotationRadians = m_OldRotationRadians;
				glm::vec3 oldScale = m_OldScale;

				// Check if any transform component has changed
				bool positionChanged = !AreVectorsEqual(oldPosition, newPosition);
				bool rotationChanged = !AreVectorsEqual(oldRotationRadians, newRotationRadians);
				bool scaleChanged = !AreVectorsEqual(oldScale, newScale);

				if (positionChanged || rotationChanged || scaleChanged)
				{
					// Push the action into the UndoRedoManager
					g_UndoRedoManager.ExecuteCommand(
						[entity, newPosition, newRotationRadians, newScale]() {
							auto& component = g_Coordinator.GetComponent<TransformComponent>(entity);
							component.SetPosition(newPosition);
							component.SetRotation(newRotationRadians);
							component.SetScale(newScale);
						},
						[entity, oldPosition, oldRotationRadians, oldScale]() {
							auto& component = g_Coordinator.GetComponent<TransformComponent>(entity);
							component.SetPosition(oldPosition);
							component.SetRotation(oldRotationRadians);
							component.SetScale(oldScale);
						}
					);

					// **Check if any children have a NodeComponent**
					bool needsPathfindingUpdate = false;

					if (g_Coordinator.HaveComponent<HierarchyComponent>(entity))
					{
						auto& hierarchyComp = g_Coordinator.GetComponent<HierarchyComponent>(entity);
						for (Entity child : hierarchyComp.children)
						{
							if (g_Coordinator.HaveComponent<NodeComponent>(child))
							{
								needsPathfindingUpdate = true;
								break; // Stop checking after finding the first match
							}
						}
					}

					// **Rebuild the pathfinding graph if necessary**
					if (needsPathfindingUpdate)
					{
						auto pathfindingSystem = g_Coordinator.GetSystem<PathfindingSystem>();
						if (pathfindingSystem)
						{
							pathfindingSystem->ResetPathfinding();
						}
					}
				}

				g_Coordinator.GetSystem<MyPhysicsSystem>()->UpdateEntityBody(g_SelectedEntity, 0.0f);
			}

			// Update m_WasUsingGizmo for the next frame
			m_WasUsingGizmo = isUsingGizmo;

			// Handle changing the gizmo operation via keyboard shortcuts
			if (ImGui::IsKeyPressed(ImGuiKey_T))
				mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
			if (ImGui::IsKeyPressed(ImGuiKey_R))
				mCurrentGizmoOperation = ImGuizmo::ROTATE;
			if (ImGui::IsKeyPressed(ImGuiKey_E))
				mCurrentGizmoOperation = ImGuizmo::SCALE;
		}

		glm::vec2 scaling = { g_WindowX / viewportPanelSize.x, g_WindowY / viewportPanelSize.y };

		glm::vec2 mouse_pos_ = { (ImGui::GetMousePos().x - viewportPanelPos.x) *scaling.x
			,(ImGui::GetMousePos().y - viewportPanelPos.y)*scaling.y };
		
		g_Input.SetMousePositionUI({ mouse_pos_.x , mouse_pos_.y });
		
		// Object picking
		if (!ImGuizmo::IsUsing() && ImGui::IsWindowFocused() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			ImVec2 mousePos = ImGui::GetMousePos();

			float mouseX = mousePos.x - viewportPanelPos.x;
			float mouseY = mousePos.y - viewportPanelPos.y;
			mouseY = viewportPanelSize.y - mouseY;

			if (mouseX >= 0 && mouseY >= 0 && mouseX < viewportPanelSize.x && mouseY < viewportPanelSize.y)
			{
				
				// Request picking render
				g_Coordinator.GetSystem<GraphicsSystem>()->SetPickingRenderer(true);

				// Read the pixel from the picking framebuffer
				glBindFramebuffer(GL_FRAMEBUFFER, g_Coordinator.GetSystem<GraphicsSystem>()->GetPickingFBO());
				glReadBuffer(GL_COLOR_ATTACHMENT0);

				unsigned char data[3];
				glReadPixels(static_cast<int>(mouseX), static_cast<int>(mouseY), 1, 1, GL_RGB, GL_UNSIGNED_BYTE, data);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				// Decode the color to get the entity ID
				Entity pickedEntity = g_Coordinator.GetSystem<GraphicsSystem>()->DecodeColorToID(data);

				if (pickedEntity != MAX_ENTITIES)
				{
					g_SelectedEntity = pickedEntity;
					m_IsSelected = true;
				}
				else
				{
					g_SelectedEntity = MAX_ENTITIES;
					m_IsSelected = false;
				}
			}
		}
	}

	ImGui::End();
}



void ImGuiEditor::WorldHierarchy()
{
	ImGui::Begin("World Hierarchy");
	{
		// Context menu for creating entities
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

		if (ImGui::Button("Clone Object") && g_Coordinator.GetTotalEntities() != 0)
		{
			if (g_SelectedEntity != MAX_ENTITIES)
			{
				Entity clone = g_Coordinator.CloneEntity(g_SelectedEntity);

				// Ensure the cloned entity gets a new physics body
				if (g_Coordinator.HaveComponent<CollisionComponent>(clone))
				{
					auto& clonedCollisionComp = g_Coordinator.GetComponent<CollisionComponent>(clone);

					// Reset Physics Body to prevent it from being the same as the original entity
					clonedCollisionComp.SetPhysicsBody(nullptr);
					clonedCollisionComp.SetHasBodyAdded(false);  // Mark as not added to physics system

					// Add a new physics body for the cloned entity
					g_Coordinator.GetSystem<MyPhysicsSystem>()->AddEntityBody(clone, 0.0f);
					clonedCollisionComp.SetHasBodyAdded(true);  // Mark as added
				}

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
					g_Coordinator.GetSystem<MyPhysicsSystem>()->RemoveEntityBody(g_GettingDeletedEntity);
					DeleteEntity(g_GettingDeletedEntity);
				}
			}
			if (m_IsSelected)
			{
				if (ImGui::Selectable("Delete Selected Object"))
				{
					if (g_SelectedEntity != MAX_ENTITIES)
					{
						g_Coordinator.GetSystem<MyPhysicsSystem>()->RemoveEntityBody(g_SelectedEntity);
						DeleteEntity(g_SelectedEntity);
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

		if (ImGui::Button("Clear all entities"))
		{
			g_Coordinator.GetSystem<MyPhysicsSystem>()->ClearAllBodies();
			g_Coordinator.ResetEntities();
			g_SceneManager.ClearSceneList();
			g_UndoRedoManager.Clear();
		}

		ImGui::Spacing(); ImGui::Spacing(); // to insert a gap so it looks visually nicer

		const ImGuiPayload* payload = ImGui::GetDragDropPayload();
		if (payload && payload->IsDataType("DND_ENTITY"))
		{
			Entity draggedEntity = *(Entity*)payload->Data;

			// Check if the dragged entity has a parent
			Entity parent = g_Coordinator.HaveComponent<HierarchyComponent>(draggedEntity)
				? g_Coordinator.GetComponent<HierarchyComponent>(draggedEntity).parent
				: MAX_ENTITIES;

			if (parent != MAX_ENTITIES)
			{
				// Display the "Drag here to remove parent" text and drag target
				ImGui::Text("Drag here to remove parent");

				// Make this area a drag-and-drop target
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* dropPayload = ImGui::AcceptDragDropPayload("DND_ENTITY"))
					{
						Entity droppedEntity = *(Entity*)dropPayload->Data;

						// Capture old and new parents for undo/redo
						Entity oldParent = parent;
						Entity newParent = MAX_ENTITIES;

						// Create doAction and undoAction lambdas
						auto doAction = [this, droppedEntity]() {
							RemoveParent(droppedEntity);
							};

						auto undoAction = [this, droppedEntity, oldParent]() {
							SetParent(droppedEntity, oldParent);
							};

						// Execute the action and add it to the UndoRedoManager
						g_UndoRedoManager.ExecuteCommand(doAction, undoAction);
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::Separator(); // Visual separator between root drag target and entity list
			}
		}

		// Displaying hierarchy game objects
		const std::vector<Entity>& allEntities = g_Coordinator.GetAliveEntitiesSet();
		m_PlayerExist = false;

		// Display root entities (entities without a parent)
		for (const auto& entity : allEntities)
		{
			bool isRoot = true;

			if (g_Coordinator.HaveComponent<HierarchyComponent>(entity))
			{
				auto& hierarchyComp = g_Coordinator.GetComponent<HierarchyComponent>(entity);
				if (hierarchyComp.parent != MAX_ENTITIES)
				{
					// Entity has a parent, so it's not a root entity
					isRoot = false;
				}
			}

			if (isRoot)
			{
				DrawEntityNode(entity);
			}


			// Need this part to have selected entity working properly.
			auto& name = g_Coordinator.GetComponent<MetadataComponent>(entity).GetName();
			if (name == "Player") m_PlayerExist = true;

			ImGuiTreeNodeFlags nodeFlags = ((g_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | (ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);

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


bool DecomposeTransform(const glm::mat4& transform, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale)
{
	using namespace glm;

	vec3 skew;
	vec4 perspective;
	quat orientation;

	if (!glm::decompose(transform, scale, orientation, position, skew, perspective))
	{
		return false;
	}

	// Correct the signs of the scale components if necessary
	if (determinant(transform) < 0)
	{
		scale = -scale;
	}

	// Convert quaternion to Euler angles (in radians)
	rotation = eulerAngles(orientation);

	return true;
}




void ImGuiEditor::InspectorWindow()
{
	static std::unordered_map<std::string, glm::vec3> oldVec3Values;
	static std::unordered_map<std::string, float> oldFloatValues;
	static std::unordered_map<std::string, std::string> oldStringValues;
	static std::unordered_map<std::string, bool> oldBoolValues;


	ImGui::Begin("Inspector");
	{
		if (m_SelectedFile.empty())
		{
			if (g_SelectedEntity < MAX_ENTITIES && g_SelectedEntity >= 0 && g_Coordinator.GetTotalEntities() != 0)
			{

				bool colorPushed = false;

				// Change color if the undo stack is empty
				if (!g_UndoRedoManager.CanUndo()) {
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f)); // Greyed-out color
					colorPushed = true;
				}
				if (ImGui::Button("Undo")) {
					if (g_UndoRedoManager.CanUndo()) {
						g_UndoRedoManager.Undo();
					}
				}
				if (colorPushed) {
					ImGui::PopStyleColor();
					colorPushed = false;
				}

				ImGui::SameLine();

				// Change color if the redo stack is empty
				if (!g_UndoRedoManager.CanRedo()) {
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f)); // Greyed-out color
					colorPushed = true;
				}
				if (ImGui::Button("Redo")) {
					if (g_UndoRedoManager.CanRedo()) {
						g_UndoRedoManager.Redo();
					}
				}
				if (colorPushed) {
					ImGui::PopStyleColor();
				}


				// Adding Components
				if (ImGui::BeginPopupContextItem("AComponents"))
				{
					if (ImGui::Selectable("Transform Component"))
					{
						if (!g_Coordinator.HaveComponent<TransformComponent>(g_SelectedEntity))
						{
							g_Coordinator.AddComponent<TransformComponent>(g_SelectedEntity, TransformComponent());
							g_UndoRedoManager.ExecuteCommand(
								[this]() {
									if (!g_Coordinator.HaveComponent<TransformComponent>(g_SelectedEntity))
										g_Coordinator.AddComponent<TransformComponent>(g_SelectedEntity, TransformComponent());
								},
								[this]() {
									if (g_Coordinator.HaveComponent<TransformComponent>(g_SelectedEntity))
										g_Coordinator.RemoveComponent<TransformComponent>(g_SelectedEntity);
								}
							);

						}
					}
					if (ImGui::Selectable("Graphics Component"))
					{
						if (!g_Coordinator.HaveComponent<GraphicsComponent>(g_SelectedEntity))
						{
							g_Coordinator.AddComponent<GraphicsComponent>(g_SelectedEntity, GraphicsComponent());
							g_UndoRedoManager.ExecuteCommand(
								[this]() {
									if (!g_Coordinator.HaveComponent<GraphicsComponent>(g_SelectedEntity))
										g_Coordinator.AddComponent<GraphicsComponent>(g_SelectedEntity, GraphicsComponent());
								},
								[this]() {
									if (g_Coordinator.HaveComponent<GraphicsComponent>(g_SelectedEntity))
										g_Coordinator.RemoveComponent<GraphicsComponent>(g_SelectedEntity);
								}
							);

						}

					//	if (!g_Coordinator.HaveComponent<AnimationComponent>(g_SelectedEntity)) {
					//
					//
					//
					//
					//		g_Coordinator.AddComponent<AnimationComponent>(g_SelectedEntity, AnimationComponent());
					//
					//
					//
					//
					//
					//		g_UndoRedoManager.ExecuteCommand(
					//			[this]() {
					//				if (!g_Coordinator.HaveComponent<AnimationComponent>(g_SelectedEntity)) {
					//					g_Coordinator.AddComponent<AnimationComponent>(g_SelectedEntity, AnimationComponent());
					//				}
					//			},
					//			[this]() {
					//				if (g_Coordinator.HaveComponent<AnimationComponent>(g_SelectedEntity)) {
					//					g_Coordinator.RemoveComponent<AnimationComponent>(g_SelectedEntity);
					//				}
					//			}
					//		);
					//	}


						 if (!g_Coordinator.HaveComponent<MaterialComponent>(g_SelectedEntity))
						 {
						 	auto& graphicsComponent = g_Coordinator.GetComponent<GraphicsComponent>(g_SelectedEntity);
						 	g_Coordinator.AddComponent<MaterialComponent>(g_SelectedEntity, MaterialComponent());
						 
						 
						 
						 	// Creates a Material File to get the data.
						 	if (g_Coordinator.HaveComponent<MetadataComponent>(g_SelectedEntity))
						 	{
						 		std::string objectName = g_Coordinator.GetComponent<MetadataComponent>(g_SelectedEntity).GetName();
						 
						 
						 		static int materialCount = 0;
						 		std::string entireFilePath = m_BaseDir.relative_path().string();
						 
						 
						 		// Handle action for "Create Material Instances"
						 		std::string materialName;
						 		// Generate the material name based on the current count
						 		if (materialCount > 0)
						 			materialName = objectName + " (" + std::to_string(materialCount) + ")";
						 		else
						 			materialName = objectName;
						 
						 		// Write the JSON string to a file
						 		std::string filePath = entireFilePath.c_str();  // Convert path to string
						 		filePath += "/Material/" + materialName + ".mat";  // Append the new file name (customize this if needed)
						 
						 		std::ifstream existingFile(filePath);
						 		if (existingFile.good()) {
						 			std::cout << "Material already exists: " << filePath << std::endl;
						 
						 		}
						 		else
						 		{
						 			rapidjson::Document document;
						 			document.SetObject();
						 			rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
						 
						 			// Add name and type (similar to your previous structure)
						 			document.AddMember("name", rapidjson::Value(materialName.c_str(), allocator), allocator);
						 			document.AddMember("shader", rapidjson::Value("Direction_obj_render", allocator), allocator);
						 
						 			// Add material properties (color, shininess, textureID, etc.)
						 			rapidjson::Value properties(rapidjson::kObjectType);
						 
						 			// Adding color as a JSON array for RGBA values
						 			rapidjson::Value color(rapidjson::kArrayType);
						 			color.PushBack(1.0f, allocator);  // R
						 			color.PushBack(1.0f, allocator);  // G
						 			color.PushBack(1.0f, allocator);  // B
						 			color.PushBack(1.0f, allocator);  // A (Alpha)
						 			properties.AddMember("color", color, allocator);
						 
						 
						 			rapidjson::Value diffuseValue("NothingTexture", allocator);
						 			properties.AddMember("Diffuse", diffuseValue, allocator);
						 
						 			rapidjson::Value NormalValue("NothingNormal", allocator);
						 			properties.AddMember("Normal", NormalValue, allocator);
						 
						 			rapidjson::Value HeightValue("NothingHeight", allocator);
						 			properties.AddMember("Height", HeightValue, allocator);
						 
						 
						 			// Add shininess
						 
						 			properties.AddMember("metallic", 0, allocator);
						 			properties.AddMember("shininess", 0, allocator);
						 
						 
						 
						 			// Add texture ID
						 			properties.AddMember("textureID", 0, allocator);  // Modify this if you have a specific texture ID
						 
						 			// Add reflectivity
						 			// properties.AddMember("reflectivity", 0.8f, allocator);
						 
						 			// Adding the properties object to the document
						 			document.AddMember("properties", properties, allocator);
						 
						 			// Serialize the JSON document to a string with pretty printing
						 			rapidjson::StringBuffer buffer;
						 			rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
						 			document.Accept(writer);
						 
						 
						 
						 			// Write the JSON string to the file
						 			std::ofstream file(filePath);
						 			if (file.is_open()) {
						 				file << buffer.GetString();  // Write the serialized JSON
						 				file.close();
						 				std::cout << "File created: " << filePath << std::endl;
						 			}
						 			else {
						 				std::cerr << "Error: Could not create the file." << std::endl;
						 			}
						 			materialCount++;
						 		}
						 	}
						 
						 
						 
						 	g_UndoRedoManager.ExecuteCommand(
						 		[this]() {
						 			if (!g_Coordinator.HaveComponent<MaterialComponent>(g_SelectedEntity))
						 				g_Coordinator.AddComponent<MaterialComponent>(g_SelectedEntity, MaterialComponent());
						 		},
						 		[this]() {
						 			if (g_Coordinator.HaveComponent<MaterialComponent>(g_SelectedEntity))
						 				g_Coordinator.RemoveComponent<MaterialComponent>(g_SelectedEntity);
						 		}
						 	);
						 
						 }

					}
					if (ImGui::Selectable("Animation Component")) {
						
						
						if (!g_Coordinator.HaveComponent<GraphicsComponent>(g_SelectedEntity))
						{
							g_Coordinator.AddComponent<GraphicsComponent>(g_SelectedEntity, GraphicsComponent());
							g_UndoRedoManager.ExecuteCommand(
								[this]() {
									if (!g_Coordinator.HaveComponent<GraphicsComponent>(g_SelectedEntity))
										g_Coordinator.AddComponent<GraphicsComponent>(g_SelectedEntity, GraphicsComponent());
								},
								[this]() {
									if (g_Coordinator.HaveComponent<GraphicsComponent>(g_SelectedEntity))
										g_Coordinator.RemoveComponent<GraphicsComponent>(g_SelectedEntity);
								}
							);

						}
						
						if (!g_Coordinator.HaveComponent<MaterialComponent>(g_SelectedEntity))
						{
							auto& graphicsComponent = g_Coordinator.GetComponent<GraphicsComponent>(g_SelectedEntity);
							g_Coordinator.AddComponent<MaterialComponent>(g_SelectedEntity, MaterialComponent());



							// Creates a Material File to get the data.
							if (g_Coordinator.HaveComponent<MetadataComponent>(g_SelectedEntity))
							{
								std::string objectName = g_Coordinator.GetComponent<MetadataComponent>(g_SelectedEntity).GetName();


								static int materialCount = 0;
								std::string entireFilePath = m_BaseDir.relative_path().string();


								// Handle action for "Create Material Instances"
								std::string materialName;
								// Generate the material name based on the current count
								if (materialCount > 0)
									materialName = objectName + " (" + std::to_string(materialCount) + ")";
								else
									materialName = objectName;

								// Write the JSON string to a file
								std::string filePath = entireFilePath.c_str();  // Convert path to string
								filePath += "/Material/" + materialName + ".mat";  // Append the new file name (customize this if needed)

								std::ifstream existingFile(filePath);
								if (existingFile.good()) {
									std::cout << "Material already exists: " << filePath << std::endl;

								}
								else
								{
									rapidjson::Document document;
									document.SetObject();
									rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

									// Add name and type (similar to your previous structure)
									document.AddMember("name", rapidjson::Value(materialName.c_str(), allocator), allocator);
									document.AddMember("shader", rapidjson::Value("Shader3D", allocator), allocator);

									// Add material properties (color, shininess, textureID, etc.)
									rapidjson::Value properties(rapidjson::kObjectType);

									// Adding color as a JSON array for RGBA values
									rapidjson::Value color(rapidjson::kArrayType);
									color.PushBack(1.0f, allocator);  // R
									color.PushBack(1.0f, allocator);  // G
									color.PushBack(1.0f, allocator);  // B
									color.PushBack(1.0f, allocator);  // A (Alpha)
									properties.AddMember("color", color, allocator);


									rapidjson::Value diffuseValue("NothingTexture", allocator);
									properties.AddMember("Diffuse", diffuseValue, allocator);

									rapidjson::Value NormalValue("NothingNormal", allocator);
									properties.AddMember("Normal", NormalValue, allocator);

									rapidjson::Value HeightValue("NothingHeight", allocator);
									properties.AddMember("Height", HeightValue, allocator);


									// Add shininess

									properties.AddMember("metallic", 0, allocator);
									properties.AddMember("shininess", 0, allocator);



									// Add texture ID
									properties.AddMember("textureID", 0, allocator);  // Modify this if you have a specific texture ID

									// Add reflectivity
									// properties.AddMember("reflectivity", 0.8f, allocator);

									// Adding the properties object to the document
									document.AddMember("properties", properties, allocator);

									// Serialize the JSON document to a string with pretty printing
									rapidjson::StringBuffer buffer;
									rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
									document.Accept(writer);



									// Write the JSON string to the file
									std::ofstream file(filePath);
									if (file.is_open()) {
										file << buffer.GetString();  // Write the serialized JSON
										file.close();
										std::cout << "File created: " << filePath << std::endl;
									}
									else {
										std::cerr << "Error: Could not create the file." << std::endl;
									}
									materialCount++;
								}
							}

						}
						if (!g_Coordinator.HaveComponent<AnimationComponent>(g_SelectedEntity)) {




							g_Coordinator.AddComponent<AnimationComponent>(g_SelectedEntity, AnimationComponent());
				
				
							g_UndoRedoManager.ExecuteCommand(
								[this]() {
									if (!g_Coordinator.HaveComponent<AnimationComponent>(g_SelectedEntity)) {
										g_Coordinator.AddComponent<AnimationComponent>(g_SelectedEntity, AnimationComponent());
									}
								},
								[this]() {
									if (g_Coordinator.HaveComponent<AnimationComponent>(g_SelectedEntity)) {
										g_Coordinator.RemoveComponent<AnimationComponent>(g_SelectedEntity);
									}
								}
							);
						}
					}

					if (ImGui::Selectable("Audio Component"))
					{
						if (!g_Coordinator.HaveComponent<AudioComponent>(g_SelectedEntity))
						{
							g_Coordinator.AddComponent<AudioComponent>(g_SelectedEntity, AudioComponent());
							g_UndoRedoManager.ExecuteCommand(
								[this]() {
									if (!g_Coordinator.HaveComponent<AudioComponent>(g_SelectedEntity))
										g_Coordinator.AddComponent<AudioComponent>(g_SelectedEntity, AudioComponent());
								},
								[this]() {
									if (g_Coordinator.HaveComponent<AudioComponent>(g_SelectedEntity))
										g_Coordinator.RemoveComponent<AudioComponent>(g_SelectedEntity);
								}
							);

						}
					}
					if (ImGui::Selectable("Behaviour Component"))
					{
						if (!g_Coordinator.HaveComponent<BehaviourComponent>(g_SelectedEntity))
						{
							g_Coordinator.AddComponent<BehaviourComponent>(g_SelectedEntity, BehaviourComponent());
							g_UndoRedoManager.ExecuteCommand(
								[this]() {
									if (!g_Coordinator.HaveComponent<BehaviourComponent>(g_SelectedEntity))
										g_Coordinator.AddComponent<BehaviourComponent>(g_SelectedEntity, BehaviourComponent());
								},
								[this]() {
									if (g_Coordinator.HaveComponent<BehaviourComponent>(g_SelectedEntity))
										g_Coordinator.RemoveComponent<BehaviourComponent>(g_SelectedEntity);
								}
							);

						}
					}
					if (ImGui::Selectable("Collision Component"))
					{
						if (!g_Coordinator.HaveComponent<CollisionComponent>(g_SelectedEntity))
						{
							g_Coordinator.AddComponent<CollisionComponent>(g_SelectedEntity, CollisionComponent());
							g_UndoRedoManager.ExecuteCommand(
								[this]() {
									if (!g_Coordinator.HaveComponent<CollisionComponent>(g_SelectedEntity))
										g_Coordinator.AddComponent<CollisionComponent>(g_SelectedEntity, CollisionComponent());
								},
								[this]() {
									if (g_Coordinator.HaveComponent<CollisionComponent>(g_SelectedEntity))
										g_Coordinator.RemoveComponent<CollisionComponent>(g_SelectedEntity);
								}
							);

						}
					}
					if (ImGui::Selectable("Camera Component"))
					{
						if (!g_Coordinator.HaveComponent<CameraComponent>(g_SelectedEntity))
						{
							g_Coordinator.AddComponent<CameraComponent>(g_SelectedEntity, CameraComponent());
							g_UndoRedoManager.ExecuteCommand(
								[this]() {
									if (!g_Coordinator.HaveComponent<CameraComponent>(g_SelectedEntity))
										g_Coordinator.AddComponent<CameraComponent>(g_SelectedEntity, CameraComponent());
								},
								[this]() {
									if (g_Coordinator.HaveComponent<CameraComponent>(g_SelectedEntity))
										g_Coordinator.RemoveComponent<CameraComponent>(g_SelectedEntity);
								}
							);

						}
					}

					if (ImGui::Selectable("Particle Component"))
					{
						if (!g_Coordinator.HaveComponent<ParticleComponent>(g_SelectedEntity))
						{
							g_Coordinator.AddComponent<ParticleComponent>(g_SelectedEntity, ParticleComponent());
							g_UndoRedoManager.ExecuteCommand(
								[this]() {
									if (!g_Coordinator.HaveComponent<ParticleComponent>(g_SelectedEntity))
										g_Coordinator.AddComponent<ParticleComponent>(g_SelectedEntity, ParticleComponent());
								},
								[this]() {
									if (g_Coordinator.HaveComponent<ParticleComponent>(g_SelectedEntity))
										g_Coordinator.RemoveComponent<ParticleComponent>(g_SelectedEntity);
								}
							);

						}
					}

					

					if (ImGui::Selectable("Light Component"))
					{
						if (!g_Coordinator.HaveComponent<LightComponent>(g_SelectedEntity))
						{
							g_Coordinator.AddComponent<LightComponent>(g_SelectedEntity, LightComponent());
							g_UndoRedoManager.ExecuteCommand(
								[this]() {
									if (!g_Coordinator.HaveComponent<LightComponent>(g_SelectedEntity))
										g_Coordinator.AddComponent<LightComponent>(g_SelectedEntity, LightComponent());
								},
								[this]() {
									if (g_Coordinator.HaveComponent<LightComponent>(g_SelectedEntity))
										g_Coordinator.RemoveComponent<LightComponent>(g_SelectedEntity);
								}
							);

						}
					}

					if (ImGui::Selectable("UI Component"))
					{
						if (!g_Coordinator.HaveComponent<UIComponent>(g_SelectedEntity))
						{
							g_Coordinator.AddComponent<UIComponent>(g_SelectedEntity, UIComponent());
							g_UndoRedoManager.ExecuteCommand(
								[this]() {
									if (!g_Coordinator.HaveComponent<UIComponent>(g_SelectedEntity))
										g_Coordinator.AddComponent<UIComponent>(g_SelectedEntity, UIComponent());
								},
								[this]() {
									if (g_Coordinator.HaveComponent<UIComponent>(g_SelectedEntity))
										g_Coordinator.RemoveComponent<UIComponent>(g_SelectedEntity);
								}
							);

							// If got UI Component, remove Transform & Graphics Component
							if (g_Coordinator.HaveComponent<TransformComponent>(g_SelectedEntity)) 
							{
								g_Coordinator.RemoveComponent<TransformComponent>(g_SelectedEntity);
							}

							if (g_Coordinator.HaveComponent<GraphicsComponent>(g_SelectedEntity))
							{
								g_Coordinator.RemoveComponent<GraphicsComponent>(g_SelectedEntity);
								g_Coordinator.RemoveComponent<MaterialComponent>(g_SelectedEntity);
							}
						}
					}

					if (ImGui::Selectable("Pathfinding Component"))
					{
						if (!g_Coordinator.HaveComponent<PathfindingComponent>(g_SelectedEntity))
						{
							g_Coordinator.AddComponent<PathfindingComponent>(g_SelectedEntity, PathfindingComponent());
							g_UndoRedoManager.ExecuteCommand(
								[this]() {
									if (!g_Coordinator.HaveComponent<PathfindingComponent>(g_SelectedEntity))
										g_Coordinator.AddComponent<PathfindingComponent>(g_SelectedEntity, PathfindingComponent());
								},
								[this]() {
									if (g_Coordinator.HaveComponent<PathfindingComponent>(g_SelectedEntity))
										g_Coordinator.RemoveComponent<PathfindingComponent>(g_SelectedEntity);
								}
								);
							
						}
					}

					if (ImGui::Selectable("Edge Component"))
					{
						if (!g_Coordinator.HaveComponent<EdgeComponent>(g_SelectedEntity))
						{
							g_Coordinator.AddComponent<EdgeComponent>(g_SelectedEntity, EdgeComponent());
							g_UndoRedoManager.ExecuteCommand(
								[this]() {
									if (!g_Coordinator.HaveComponent<EdgeComponent>(g_SelectedEntity))
										g_Coordinator.AddComponent<EdgeComponent>(g_SelectedEntity, EdgeComponent());
								},
								[this]() {
									if (g_Coordinator.HaveComponent<EdgeComponent>(g_SelectedEntity))
										g_Coordinator.RemoveComponent<EdgeComponent>(g_SelectedEntity);
								}
								);

						}
					}

					if (ImGui::Selectable("Node Component"))
					{
						if (!g_Coordinator.HaveComponent<NodeComponent>(g_SelectedEntity))
						{
							g_Coordinator.AddComponent<NodeComponent>(g_SelectedEntity, NodeComponent());
							g_UndoRedoManager.ExecuteCommand(
								[this]() {
									if (!g_Coordinator.HaveComponent<NodeComponent>(g_SelectedEntity))
										g_Coordinator.AddComponent<NodeComponent>(g_SelectedEntity, NodeComponent());
								},
								[this]() {
									if (g_Coordinator.HaveComponent<NodeComponent>(g_SelectedEntity))
										g_Coordinator.RemoveComponent<NodeComponent>(g_SelectedEntity);
								}
								);

						}
					}
					if (ImGui::Selectable("Font Component"))
					{
						if (!g_Coordinator.HaveComponent<FontComponent>(g_SelectedEntity))
						{
							g_Coordinator.AddComponent<FontComponent>(g_SelectedEntity, FontComponent());
							g_UndoRedoManager.ExecuteCommand(
								[this]() {
									if (!g_Coordinator.HaveComponent<FontComponent>(g_SelectedEntity))
										g_Coordinator.AddComponent<FontComponent>(g_SelectedEntity, FontComponent());
								},
								[this]() {
									if (g_Coordinator.HaveComponent<FontComponent>(g_SelectedEntity))
										g_Coordinator.RemoveComponent<FontComponent>(g_SelectedEntity);
								}
							);
							// If got UI Component, remove Transform & Graphics Component
							if (g_Coordinator.HaveComponent<TransformComponent>(g_SelectedEntity))
							{
								g_Coordinator.RemoveComponent<TransformComponent>(g_SelectedEntity);
							}
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
						if (ImGui::Selectable("Transform Component"))
						{
							// Capture the component data before deletion
							auto& componentData = g_Coordinator.GetComponent<TransformComponent>(g_SelectedEntity);

							g_UndoRedoManager.ExecuteCommand(
								[this]() {
									// Do action: Remove the component
									if (g_Coordinator.HaveComponent<TransformComponent>(g_SelectedEntity))
										g_Coordinator.RemoveComponent<TransformComponent>(g_SelectedEntity);
								},
								[this, componentData]() {
									// Undo action: Add the component back with the captured data
									if (!g_Coordinator.HaveComponent<TransformComponent>(g_SelectedEntity))
										g_Coordinator.AddComponent<TransformComponent>(g_SelectedEntity, componentData);
								}
							);
						}
					}

					if (g_Coordinator.HaveComponent<GraphicsComponent>(g_SelectedEntity))
					{

						auto& graphics = g_Coordinator.GetComponent<GraphicsComponent>(g_SelectedEntity);
						auto& material = g_Coordinator.GetComponent<MaterialComponent>(g_SelectedEntity);

						if (ImGui::Selectable("Graphics Component"))
						{
							g_UndoRedoManager.ExecuteCommand(
								[this]() {
									if (g_Coordinator.HaveComponent<GraphicsComponent>(g_SelectedEntity))
										g_Coordinator.RemoveComponent<GraphicsComponent>(g_SelectedEntity);
								},
								[this, graphics]() {
									if (!g_Coordinator.HaveComponent<GraphicsComponent>(g_SelectedEntity))
										g_Coordinator.AddComponent<GraphicsComponent>(g_SelectedEntity, graphics);
								}
							);

							g_UndoRedoManager.ExecuteCommand(
								[this]() {
									if (g_Coordinator.HaveComponent<MaterialComponent>(g_SelectedEntity))
										g_Coordinator.RemoveComponent<MaterialComponent>(g_SelectedEntity);
								},
								[this, material]() {
									if (!g_Coordinator.HaveComponent<MaterialComponent>(g_SelectedEntity))
										g_Coordinator.AddComponent<MaterialComponent>(g_SelectedEntity, material);
								}
							);
						}
					}

					if (g_Coordinator.HaveComponent<AnimationComponent>(g_SelectedEntity))
					{
						if (ImGui::Selectable("Animation Component"))
						{
							auto& componentData = g_Coordinator.GetComponent<AnimationComponent>(g_SelectedEntity);
					
							g_UndoRedoManager.ExecuteCommand(
								[this]() {
									if (g_Coordinator.HaveComponent<AnimationComponent>(g_SelectedEntity))
										g_Coordinator.RemoveComponent<AnimationComponent>(g_SelectedEntity);
								},
								[this, componentData]() {
									if (!g_Coordinator.HaveComponent<AnimationComponent>(g_SelectedEntity))
										g_Coordinator.AddComponent<AnimationComponent>(g_SelectedEntity, componentData);
								}
							);
						}
					}

					if (g_Coordinator.HaveComponent<AudioComponent>(g_SelectedEntity))
					{
						if (ImGui::Selectable("Audio Component"))
						{
							auto& componentData = g_Coordinator.GetComponent<AudioComponent>(g_SelectedEntity);

							g_UndoRedoManager.ExecuteCommand(
								[this]() {
									if (g_Coordinator.HaveComponent<AudioComponent>(g_SelectedEntity))
										g_Coordinator.RemoveComponent<AudioComponent>(g_SelectedEntity);
								},
								[this, componentData]() {
									if (!g_Coordinator.HaveComponent<AudioComponent>(g_SelectedEntity))
										g_Coordinator.AddComponent<AudioComponent>(g_SelectedEntity, componentData);
								}
							);
						}
					}

					if (g_Coordinator.HaveComponent<BehaviourComponent>(g_SelectedEntity))
					{
						if (ImGui::Selectable("Behaviour Component"))
						{
							auto& componentData = g_Coordinator.GetComponent<BehaviourComponent>(g_SelectedEntity);

							g_UndoRedoManager.ExecuteCommand(
								[this]() {
									if (g_Coordinator.HaveComponent<BehaviourComponent>(g_SelectedEntity))
										g_Coordinator.RemoveComponent<BehaviourComponent>(g_SelectedEntity);
								},
								[this, componentData]() {
									if (!g_Coordinator.HaveComponent<BehaviourComponent>(g_SelectedEntity))
										g_Coordinator.AddComponent<BehaviourComponent>(g_SelectedEntity, componentData);
								}
							);
						}
					}

					if (g_Coordinator.HaveComponent<CollisionComponent>(g_SelectedEntity))
					{
						if (ImGui::Selectable("Collision Component"))
						{
							auto& componentData = g_Coordinator.GetComponent<CollisionComponent>(g_SelectedEntity);

							g_UndoRedoManager.ExecuteCommand(
								[this]() {
									if (g_Coordinator.HaveComponent<CollisionComponent>(g_SelectedEntity))
										g_Coordinator.RemoveComponent<CollisionComponent>(g_SelectedEntity);
								},
								[this, componentData]() {
									if (!g_Coordinator.HaveComponent<CollisionComponent>(g_SelectedEntity))
										g_Coordinator.AddComponent<CollisionComponent>(g_SelectedEntity, componentData);
								}
							);
						}
					}

					if (g_Coordinator.HaveComponent<CameraComponent>(g_SelectedEntity))
					{
						if (ImGui::Selectable("Camera Component"))
						{
							auto& componentData = g_Coordinator.GetComponent<CameraComponent>(g_SelectedEntity);

							g_UndoRedoManager.ExecuteCommand(
								[this]() {
									if (g_Coordinator.HaveComponent<CameraComponent>(g_SelectedEntity))
										g_Coordinator.RemoveComponent<CameraComponent>(g_SelectedEntity);
								},
								[this, componentData]() {
									if (!g_Coordinator.HaveComponent<CameraComponent>(g_SelectedEntity))
										g_Coordinator.AddComponent<CameraComponent>(g_SelectedEntity, componentData);
								}
							);
						}
					}

					if (g_Coordinator.HaveComponent<ParticleComponent>(g_SelectedEntity))
					{
						if (ImGui::Selectable("Particle Component"))
						{
							auto& componentData = g_Coordinator.GetComponent<ParticleComponent>(g_SelectedEntity);

							g_UndoRedoManager.ExecuteCommand(
								[this]() {
									if (g_Coordinator.HaveComponent<ParticleComponent>(g_SelectedEntity))
										g_Coordinator.RemoveComponent<ParticleComponent>(g_SelectedEntity);
								},
								[this, componentData]() {
									if (!g_Coordinator.HaveComponent<ParticleComponent>(g_SelectedEntity))
										g_Coordinator.AddComponent<ParticleComponent>(g_SelectedEntity, componentData);
								}
							);
						}
					}
					if (g_Coordinator.HaveComponent<LightComponent>(g_SelectedEntity))
					{
						if (ImGui::Selectable("Light Component"))
						{
							auto& componentData = g_Coordinator.GetComponent<LightComponent>(g_SelectedEntity);

							g_UndoRedoManager.ExecuteCommand(
								[this]() {
									if (g_Coordinator.HaveComponent<LightComponent>(g_SelectedEntity))
										g_Coordinator.RemoveComponent<LightComponent>(g_SelectedEntity);
								},
								[this, componentData]() {
									if (!g_Coordinator.HaveComponent<LightComponent>(g_SelectedEntity))
										g_Coordinator.AddComponent<LightComponent>(g_SelectedEntity, componentData);
								}
							);
						}
					}
					if (g_Coordinator.HaveComponent<UIComponent>(g_SelectedEntity))
					{
						if (ImGui::Selectable("UI Component"))
						{
							auto& componentData = g_Coordinator.GetComponent<UIComponent>(g_SelectedEntity);

							g_UndoRedoManager.ExecuteCommand(
								[this]() {
									if (g_Coordinator.HaveComponent<UIComponent>(g_SelectedEntity))
										g_Coordinator.RemoveComponent<UIComponent>(g_SelectedEntity);
								},
								[this, componentData]() {
									if (!g_Coordinator.HaveComponent<UIComponent>(g_SelectedEntity))
										g_Coordinator.AddComponent<UIComponent>(g_SelectedEntity, componentData);
								}
							);
						}
					}

					if (g_Coordinator.HaveComponent<PathfindingComponent>(g_SelectedEntity))
					{
						if (ImGui::Selectable("Pathfinding Component"))
						{
							auto componentData = g_Coordinator.GetComponent<PathfindingComponent>(g_SelectedEntity);

							g_UndoRedoManager.ExecuteCommand(
								[this]() {
									if (g_Coordinator.HaveComponent<PathfindingComponent>(g_SelectedEntity))
										g_Coordinator.RemoveComponent<PathfindingComponent>(g_SelectedEntity);
								},
								[this, componentData]() {
									if (!g_Coordinator.HaveComponent<PathfindingComponent>(g_SelectedEntity))
										g_Coordinator.AddComponent<PathfindingComponent>(g_SelectedEntity, componentData);
								}
								);
						}
					}

					if (g_Coordinator.HaveComponent<EdgeComponent>(g_SelectedEntity))
					{
						if (ImGui::Selectable("Edge Component"))
						{
							auto componentData = g_Coordinator.GetComponent<EdgeComponent>(g_SelectedEntity);

							g_UndoRedoManager.ExecuteCommand(
								[this]() {
									if (g_Coordinator.HaveComponent<EdgeComponent>(g_SelectedEntity))
										g_Coordinator.RemoveComponent<EdgeComponent>(g_SelectedEntity);
								},
								[this, componentData]() {
									if (!g_Coordinator.HaveComponent<EdgeComponent>(g_SelectedEntity))
										g_Coordinator.AddComponent<EdgeComponent>(g_SelectedEntity, componentData);
								}
								);
						}
					}

					if (g_Coordinator.HaveComponent<NodeComponent>(g_SelectedEntity))
					{
						if (ImGui::Selectable("Node Component"))
						{
							auto componentData = g_Coordinator.GetComponent<NodeComponent>(g_SelectedEntity);

							g_UndoRedoManager.ExecuteCommand(
								[this]() {
									if (g_Coordinator.HaveComponent<NodeComponent>(g_SelectedEntity))
										g_Coordinator.RemoveComponent<NodeComponent>(g_SelectedEntity);
								},
								[this, componentData]() {
									if (!g_Coordinator.HaveComponent<NodeComponent>(g_SelectedEntity))
										g_Coordinator.AddComponent<NodeComponent>(g_SelectedEntity, componentData);
								}
								);
						}
					}

					ImGui::EndPopup();
				}

				if (ImGui::Button("Delete Component"))
				{
					ImGui::OpenPopup("DComponents");
				}


				// Use Reflection to Iterate Over All Components
				const auto& componentTypes = g_Coordinator.GetTotalRegisteredComponents();
				
				for (ComponentType type = 0; type < componentTypes; ++type)
				{
					if (g_Coordinator.GetEntitySignature(g_SelectedEntity).test(type))
					{
						// Retrieve the component type's name
						std::string className = ReflectionManager::Instance().GetTypeNameFromTypeIndex(type);
						// Custom UI for MetadataComponent
						if (className == "MetadataComponent")
						{
							// Custom UI for MetadataComponent
							if (ImGui::CollapsingHeader("Identifier", ImGuiTreeNodeFlags_None))
							{
								auto& metadataComponent = g_Coordinator.GetComponent<MetadataComponent>(g_SelectedEntity);
								metadataComponent.RegisterProperties();

								std::string propertyName = "Name";
								std::string oldValue = metadataComponent.GetName();
								std::string newValue = oldValue;

								// Display Entity ID
								ImGui::Text("Entity ID: %d", g_SelectedEntity);

								// **Check if the entity has a NodeComponent and display Node ID**
								if (g_Coordinator.HaveComponent<NodeComponent>(g_SelectedEntity))
								{
									auto& nodeComponent = g_Coordinator.GetComponent<NodeComponent>(g_SelectedEntity);
									ImGui::Text("Node ID: %d", nodeComponent.GetNodeID());
								}

								ImGui::Text("Name    ");
								ImGui::SameLine();
								ImGui::PushItemWidth(125.0f);
								ImGui::PushID(propertyName.c_str());

								char buffer[256];
								memset(buffer, 0, sizeof(buffer));
								strcpy_s(buffer, sizeof(buffer), oldValue.c_str());

								if (ImGui::InputText("##ObjectName", buffer, sizeof(buffer)))
								{
									newValue = std::string(buffer);
									metadataComponent.SetName(newValue);
								}

								if (ImGui::IsItemActivated())
								{
									oldStringValues[propertyName] = oldValue;
								}

								if (ImGui::IsItemDeactivatedAfterEdit())
								{
									std::string oldVal = oldStringValues[propertyName];
									Entity entity = g_SelectedEntity;
									oldStringValues.erase(propertyName);

									g_UndoRedoManager.ExecuteCommand(
										[entity, newValue]() {
											auto& component = g_Coordinator.GetComponent<MetadataComponent>(entity);
											component.SetName(newValue);
										},
										[entity, oldVal]() {
											auto& component = g_Coordinator.GetComponent<MetadataComponent>(entity);
											component.SetName(oldVal);
										}
									);
								}

								ImGui::PopID();
							}
						}

						if (className == "TransformComponent")
						{
							// Custom UI for TransformComponent
							if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_None))
							{
								auto& transformComponent = g_Coordinator.GetComponent<TransformComponent>(g_SelectedEntity);
								transformComponent.RegisterProperties();

								const auto& properties = ReflectionManager::Instance().GetProperties("TransformComponent");
								for (const auto& property : properties)
								{
									std::string propertyName = property->GetName();
									if (propertyName == "Scale") {
										propertyName += "   "; // Add extra spaces for alignment if the name is "Scale"
									}

									ImGui::PushItemWidth(250.0f);
									ImGui::Text("%s", propertyName.c_str());
									ImGui::SameLine();
									std::string widgetID = "##" + propertyName;

									ImGui::PushID(widgetID.c_str());

									// For glm::vec3 properties
									if (property->GetValue(&transformComponent).find(",") != std::string::npos)
									{
										glm::vec3 vecValue = SerializationHelpers::DeserializeVec3(property->GetValue(&transformComponent));

										if (ImGui::DragFloat3("##Drag", &vecValue.x, 0.1f))
										{
											// Start editing
											transformComponent.SetEditing(true);

											property->SetValue(&transformComponent, SerializationHelpers::SerializeVec3(vecValue));
										}

										if (ImGui::IsItemActivated())
										{
											// Store old value
											oldVec3Values[propertyName] = vecValue;
										}

										if (ImGui::IsItemDeactivatedAfterEdit())
										{
											// Stop editing
											transformComponent.SetEditing(false);

											// Update the physics body position immediately
											g_Coordinator.GetSystem<MyPhysicsSystem>()->UpdateEntityBody(g_SelectedEntity, 0.0f);

											glm::vec3 newValue = vecValue;
											glm::vec3 oldValue = oldVec3Values[propertyName];
											Entity entity = g_SelectedEntity;

											// Clean up the stored old value
											oldVec3Values.erase(propertyName);

											g_UndoRedoManager.ExecuteCommand(
												// Do Action
												[entity, propertyName, newValue]() {
													auto& component = g_Coordinator.GetComponent<TransformComponent>(entity);
													const auto& properties = ReflectionManager::Instance().GetProperties("TransformComponent");
													auto propIt = std::find_if(properties.begin(), properties.end(),
														[&propertyName](const ReflectionPropertyBase* prop) {
															return prop->GetName() == propertyName;
														});
													if (propIt != properties.end())
													{
														(*propIt)->SetValue(&component, SerializationHelpers::SerializeVec3(newValue));
													}
													// Update systems
													g_Coordinator.GetSystem<MyPhysicsSystem>()->UpdateEntityBody(entity, 0.0f);
												},
												// Undo Action
												[entity, propertyName, oldValue]() {
													auto& component = g_Coordinator.GetComponent<TransformComponent>(entity);
													const auto& properties = ReflectionManager::Instance().GetProperties("TransformComponent");
													auto propIt = std::find_if(properties.begin(), properties.end(),
														[&propertyName](const ReflectionPropertyBase* prop) {
															return prop->GetName() == propertyName;
														});
													if (propIt != properties.end())
													{
														(*propIt)->SetValue(&component, SerializationHelpers::SerializeVec3(oldValue));
													}
													// Update systems
													g_Coordinator.GetSystem<MyPhysicsSystem>()->UpdateEntityBody(entity, 0.0f);
												}
											);
										}
									}
									else
									{
										// For scalar properties (float)
										float floatValue = std::stof(property->GetValue(&transformComponent));

										if (ImGui::DragFloat("##Drag", &floatValue, 0.1f))
										{
											property->SetValue(&transformComponent, std::to_string(floatValue));
										}

										if (ImGui::IsItemActivated())
										{
											// Store old value
											oldFloatValues[propertyName] = floatValue;
										}

										if (ImGui::IsItemDeactivatedAfterEdit())
										{
											float newValue = floatValue;
											float oldValue = oldFloatValues[propertyName];
											Entity entity = g_SelectedEntity;

											// Clean up the stored old value
											oldFloatValues.erase(propertyName);

											g_UndoRedoManager.ExecuteCommand(
												// Do Action
												[entity, propertyName, newValue]() {
													auto& component = g_Coordinator.GetComponent<TransformComponent>(entity);
													const auto& properties = ReflectionManager::Instance().GetProperties("TransformComponent");
													auto propIt = std::find_if(properties.begin(), properties.end(),
														[&propertyName](const ReflectionPropertyBase* prop) {
															return prop->GetName() == propertyName;
														});
													if (propIt != properties.end())
													{
														(*propIt)->SetValue(&component, std::to_string(newValue));
													}
													// Update systems
													g_Coordinator.GetSystem<MyPhysicsSystem>()->UpdateEntityBody(entity, 0.0f);
												},
												// Undo Action
												[entity, propertyName, oldValue]() {
													auto& component = g_Coordinator.GetComponent<TransformComponent>(entity);
													const auto& properties = ReflectionManager::Instance().GetProperties("TransformComponent");
													auto propIt = std::find_if(properties.begin(), properties.end(),
														[&propertyName](const ReflectionPropertyBase* prop) {
															return prop->GetName() == propertyName;
														});
													if (propIt != properties.end())
													{
														(*propIt)->SetValue(&component, std::to_string(oldValue));
													}
													// Update systems
													g_Coordinator.GetSystem<MyPhysicsSystem>()->UpdateEntityBody(entity, 0.0f);
												}
											);

										}


									}

									ImGui::PopID();
								}
								if (g_Coordinator.HaveComponent<HierarchyComponent>(g_SelectedEntity)) {
									ImGui::Separator();
									ImGui::Text("World Transform");

									// Get the world matrix from TransformSystem
									auto transformSystem = g_Coordinator.GetSystem<TransformSystem>();
									glm::mat4 worldMatrix = transformSystem->GetWorldMatrix(g_SelectedEntity);

									// Decompose the world matrix
									glm::vec3 worldPosition, worldRotation, worldScale;
									if (DecomposeTransform(worldMatrix, worldPosition, worldRotation, worldScale))
									{
										// Array of property names and values
										std::vector<std::pair<std::string, glm::vec3*>> worldProperties = {
											{"Position", &worldPosition},
											{"Scale   ", &worldScale}, // Add extra spaces for alignment
											{"Rotation", &worldRotation}
										};

										for (auto& [propertyName, valuePtr] : worldProperties)
										{
											ImGui::PushItemWidth(250.0f);
											ImGui::Text("%s", propertyName.c_str());
											ImGui::SameLine();
											std::string widgetID = "##World" + propertyName;

											ImGui::PushID(widgetID.c_str());

											ImGui::InputFloat3("##WorldInput", &valuePtr->x, "%.3f", ImGuiInputTextFlags_ReadOnly);

											ImGui::PopID();
										}
									}
									else
									{
										ImGui::Text("Failed to decompose world matrix.");
									}
								}
							}
						}

						else if(className == "HierarchyComponent")
						{
							if (ImGui::CollapsingHeader("Hierarchy", ImGuiTreeNodeFlags_None))
							{
								auto& hierarchyComponent = g_Coordinator.GetComponent<HierarchyComponent>(g_SelectedEntity);
								hierarchyComponent.RegisterProperties();

								const auto& properties = ReflectionManager::Instance().GetProperties("HierarchyComponent");
								for (const auto& property : properties)
								{
									std::string propertyName = property->GetName();

									ImGui::PushItemWidth(250.0f);
									ImGui::Text("%s", propertyName.c_str());
									ImGui::SameLine();
									std::string widgetID = "##" + propertyName;

									ImGui::PushID(widgetID.c_str());

									// Handle 'parent' property
									if (propertyName == "Parent")
									{
										std::string parentStr = property->GetValue(&hierarchyComponent);
										Entity currentParentEntity = hierarchyComponent.parent;

										// Prepare list of entities for selection
										const auto& allEntities = g_Coordinator.GetAliveEntitiesSet();
										std::vector<std::string> entityNames;
										std::vector<std::string> entityIds;

										entityNames.push_back("None");
										entityIds.push_back("None");

										int currentParentIndex = 0;

										int index = 1; // Start from 1 because "None" is at index 0
										for (auto& ent : allEntities)
										{
											if (ent != g_SelectedEntity) // Exclude the current entity to prevent self-parenting
											{
												bool includeEntity = false;

												if (ent == currentParentEntity)
												{
													// Always include the current parent to allow maintaining the relationship
													includeEntity = true;
												}
												else if (!IsAncestorOf(g_SelectedEntity, ent))
												{
													// Include entity if it is not a descendant of the selected entity
													includeEntity = true;
												}

												if (includeEntity)
												{
													std::string name = "Entity " + std::to_string(ent);
													if (g_Coordinator.HaveComponent<MetadataComponent>(ent))
													{
														name = g_Coordinator.GetComponent<MetadataComponent>(ent).GetName();
													}
													entityNames.push_back(name);
													entityIds.push_back(std::to_string(ent));

													if (parentStr != "None" && std::stoul(parentStr) == ent)
													{
														currentParentIndex = index;
													}

													++index;
												}
												// Else, skip the entity to prevent cyclic dependency
											}
										}

										// Convert std::vector<std::string> to std::vector<const char*>
										std::vector<const char*> items;
										for (const auto& name : entityNames)
										{
											items.push_back(name.c_str());
										}

										// Display combo box for parent selection
										if (ImGui::Combo("##Parent", &currentParentIndex, items.data(), static_cast<int>(items.size())))
										{
											// Parent changed
											std::string newParentStr = entityIds[currentParentIndex];

											// Update the actual parent-child relationship
											Entity oldParent = hierarchyComponent.parent;
											Entity newParent = MAX_ENTITIES;
											if (newParentStr != "None")
											{
												newParent = static_cast<Entity>(std::stoul(newParentStr));
											}

											// Create doAction and undoAction lambdas
											auto doAction = [this, entity = g_SelectedEntity, newParent]() {
												// Apply the new parent
												SetParent(entity, newParent);
												};

											auto undoAction = [this, entity = g_SelectedEntity, oldParent]() {
												// Revert to the old parent
												SetParent(entity, oldParent);
												};


											// Execute the action and add it to the UndoRedoManager
											g_UndoRedoManager.ExecuteCommand(doAction, undoAction);
										}
									}

									// Handle 'children' property
									else if (propertyName == "Children")
									{
										// Display label

										// Get children entities
										std::string childrenStr = property->GetValue(&hierarchyComponent);

										// Parse the string into a vector of Entities
										std::vector<Entity> children;
										std::stringstream ss(childrenStr);
										std::string item;
										while (std::getline(ss, item, ','))
										{
											if (!item.empty())
											{
												Entity childEntity = static_cast<Entity>(std::stoul(item));
												children.push_back(childEntity);
											}
										}
										ImGui::NewLine();
										if (!children.empty())
										{
											for (auto child : children)
											{
												std::string childName = "Entity " + std::to_string(child);
												if (g_Coordinator.HaveComponent<MetadataComponent>(child))
												{
													childName = g_Coordinator.GetComponent<MetadataComponent>(child).GetName();
												}

												// Display child entity as selectable text
												if (ImGui::Selectable(childName.c_str()))
												{
													// Select the child entity
													g_SelectedEntity = child;
												}
											}
										}
										else {
											ImGui::NewLine();
											ImGui::Text("This entity has no children");
										}
									}
									else
									{
										// Handle other properties if necessary
										std::string value = property->GetValue(&hierarchyComponent);
										ImGui::Text("%s", value.c_str());
									}

									ImGui::PopID();
								}
							}
						}


						else if (className == "GraphicsComponent")
						{

							// Custom UI for GraphicsComponent
							if (ImGui::CollapsingHeader("Graphics", ImGuiTreeNodeFlags_None))
							{


								ImGui::Text("Graphics Properties:");

								auto& graphicsComponent = g_Coordinator.GetComponent<GraphicsComponent>(g_SelectedEntity);
								const auto& properties = ReflectionManager::Instance().GetProperties("GraphicsComponent");
							
								graphicsComponent.RegisterProperties();
								
									// Handle ModelName Property
									auto modelNameProperty = std::find_if(properties.begin(), properties.end(),
										[](const ReflectionPropertyBase* prop) { return prop->GetName() == "ModelName"; });




									if (modelNameProperty != properties.end())
									{
										std::string propertyName = "ModelName";
										std::string currentModelName = (*modelNameProperty)->GetValue(&graphicsComponent);
										std::vector<std::string> modelNames = g_ResourceManager.getModelNames();
										int currentItem = 0;
										for (size_t i = 0; i < modelNames.size(); ++i)
										{
											if (modelNames[i] == currentModelName)
											{
												currentItem = static_cast<int>(i);
												break;
											}
										}

										
										ImGui::PushItemWidth(123.0f);
										ImGui::Text("Model   ");
										ImGui::SameLine();
										ImGui::PushID(propertyName.c_str());
										std::string oldModelName = currentModelName;

										if (ImGui::Combo("##ModelCombo", &currentItem, [](void* data, int idx, const char** out_text) {
											const auto& names = *(static_cast<std::vector<std::string>*>(data));
											*out_text = names[idx].c_str();
											return true;
											}, (void*)&modelNames, static_cast<int>(modelNames.size())))
										{


//											auto& animationComponent = g_Coordinator.GetComponent<AnimationComponent>(g_SelectedEntity);

											graphicsComponent.chooseModel = true;

											// Selection changed
											std::string newModelName = modelNames[currentItem];
											(*modelNameProperty)->SetValue(&graphicsComponent, newModelName);
											

											graphicsComponent.SetModel(g_ResourceManager.ModelMap[newModelName].get());


											Entity entity = g_SelectedEntity;

											g_UndoRedoManager.ExecuteCommand(
												[entity, newModelName]() {
													auto& component = g_Coordinator.GetComponent<GraphicsComponent>(entity);
													component.setModelName(newModelName);
												},
												[entity, oldModelName]() {
													auto& component = g_Coordinator.GetComponent<GraphicsComponent>(entity);
													component.setModelName(oldModelName);
												}
											);
										}

										ImGui::PopID();
									}

									// Handel Camera Property
									auto FollowCameraProperty = std::find_if(properties.begin(), properties.end(),
										[](const ReflectionPropertyBase* prop) { return prop->GetName() == "FollowCamera"; });
									if (FollowCameraProperty != properties.end())
									{
										std::string propertyName = "FollowCamera";
										bool isFollowCamera = (*FollowCameraProperty)->GetValue(&graphicsComponent) == "true";

										ImGui::Text("Follow Camera");
										ImGui::SameLine();
										ImGui::PushID(propertyName.c_str());

										if (ImGui::Checkbox("##FollowCamera", &isFollowCamera))
										{
											(*FollowCameraProperty)->SetValue(&graphicsComponent, isFollowCamera ? "true" : "false");
										}
										if (ImGui::IsItemActivated())
										{
											oldBoolValues[propertyName] = isFollowCamera;
										}

										if (ImGui::IsItemDeactivatedAfterEdit())
										{
											bool newValue = isFollowCamera;
											bool oldValue = oldBoolValues[propertyName];
											Entity entity = g_SelectedEntity;
											oldBoolValues.erase(propertyName);

											g_UndoRedoManager.ExecuteCommand(
												[entity, newValue]() {
													auto& component = g_Coordinator.GetComponent<GraphicsComponent>(entity);
													component.SetFollowCamera(newValue);
												},
												[entity, oldValue]() {
													auto& component = g_Coordinator.GetComponent<GraphicsComponent>(entity);
													component.SetFollowCamera(oldValue);
												}
											);
										}

										ImGui::PopID();
									}

									ImGui::Text("Debug   ");
									ImGui::SameLine();
									ImGui::Checkbox("##DebugMode", &GraphicsSystem::debug);

									if (GraphicsSystem::debug)
									{
										if (ImGui::Button("2D"))
										{
											GraphicsSystem::D3 = false;
											GraphicsSystem::D2 = true;
										}
										if (ImGui::Button("3D"))
										{
											GraphicsSystem::D3 = true;
											GraphicsSystem::D2 = false;
										}
									}
									else
									{
										GraphicsSystem::D3 = false;
										GraphicsSystem::D2 = false;
									}

								
							}
						}
						else if (className == "AnimationComponent") {
							// Show the dropdown menu if there are any files
							
							if (ImGui::CollapsingHeader("Animation", ImGuiTreeNodeFlags_None)) {
								
								if (g_Coordinator.HaveComponent<GraphicsComponent>(g_SelectedEntity))
								{

									auto& graphicsComp = g_Coordinator.GetComponent<GraphicsComponent>(g_SelectedEntity);
									auto& animationComp = g_Coordinator.GetComponent<AnimationComponent>(g_SelectedEntity);


									if (g_ResourceManager.AnimatorMap.find(graphicsComp.getModelName()) != g_ResourceManager.AnimatorMap.end())
									{

										//	if (graphicsComp.chooseModel)





										animationComp.animationName = g_Coordinator.GetComponent<GraphicsComponent>(g_SelectedEntity).getModelName();
										animationComp.animatorName = animationComp.animationName;



										// Get Delta Time
										float& deltaTime = g_Coordinator.GetComponent<AnimationComponent>(g_SelectedEntity).m_DeltaTime;

										// Static variables to store animation time and play/pause state
										static float animationTime = 0.0f;
										static float startTime = 0.0f;
										static float endTime = 0.0f;
										static bool printProblem = false;
										static int animNameBuffer = 0;
										static int currentIndex = 0; // Track which animation we are playing
										static bool isPlaying = false; // Play state

										static float startPlay = 0.01f;
										static float endPlay = animationComp.m_Duration;


										static int inputValue = 0;


										ImGui::Text("Name of Animation: %s", animationComp.animationName.c_str());
										ImGui::Text("Duration: %.2f s", animationComp.m_Duration);

										ImGui::Separator();


										if (deltaTime < startPlay || deltaTime> endPlay)
										{
											g_ResourceManager.AnimatorMap[graphicsComp.getModelName()]->SetAnimationTime(startPlay);

											//									g_Coordinator.GetComponent<AnimationComponent>(g_SelectedEntity).m_DeltaTime = deltaTime = startPlay;


											//									graphicsComp.SetAnimationTime(deltaTime);
										}

										ImGui::PushItemWidth(400.f);

										// Slider for animation progress (also acts as manual seek bar)
										if (ImGui::SliderFloat("##AnimationProgress", &deltaTime, startTime, animationComp.m_Duration, "%.2f s")) {
											graphicsComp.pauseAnimation = true;  // Pause when user interacts

											graphicsComp.SetAnimationTime(deltaTime);
										}
										else
										{
											graphicsComp.pauseAnimation = false;  // Pause when user interacts
										}

										ImGui::Text("Delta Time of Game Application:  %.2f", deltaTime);



										ImGui::Text("Enter Animation Time Range");



										ImGui::PushItemWidth(100.f);

										// Input floats for Start and End Time
										ImGui::Value("Save", static_cast<int>(animationComp.animationVector.size()));
										ImGui::PushItemWidth(400.f);
										ImGui::SliderFloat("Start Time", &startTime, 0.1f, animationComp.m_Duration, "%.2f s");
										ImGui::SliderFloat("End Time", &endTime, 0.1f, animationComp.m_Duration, "%.2f s");


										if (ImGui::Button("Save Range")) {
											printProblem = false;

											//  Check if input is valid
											if (startTime < endTime) {
												animationComp.animationVector.push_back({ inputValue , startTime, endTime });

												std::cout << "Saved Animation Range: [" << inputValue
													<< "] (" << startTime << "s - " << endTime << "s)\n";

												inputValue++;
												startTime = 0.0f;
												endTime = 0.0f;
											}
											else {
												printProblem = true;
											}
										}

										if (printProblem)
										{
											ImGui::Separator();
											ImGui::Text(" Invalid range! Start time must be less than End time");
											ImGui::Text(" & Name cannot be empty.");
										}


										ImGui::Separator(); // UI separator for clarity

										if (ImGui::InputInt("Enter Animation Name", &animNameBuffer)) {
											currentIndex = 0; // Reset index if input changes
										}



										if (ImGui::Button("Play")) {

											// Find animation range in the vector using the input animation ID
											auto it = std::find_if(animationComp.animationVector.begin(), animationComp.animationVector.end(),
												[&](const auto& d) { return std::get<0>(d) == animNameBuffer; });

											if (it != animationComp.animationVector.end()) {

												isPlaying = !isPlaying; // Toggle play state
												if (isPlaying) {
													int val = std::get<0>(*it);
													float start = std::get<1>(*it);
													float end = std::get<2>(*it);

													std::cout << "Playing animation ID [" << val << "] from "
														<< start << "s to " << end << "s\n";

													animationComp.playThisAnimation = std::make_tuple(val, start, end);
													//	animationComp.currentTime = start;  // Reset time to startt
													startPlay = std::get<1>(animationComp.playThisAnimation);
													endPlay = std::get<2>(animationComp.playThisAnimation);


													currentIndex = 0;  // Reset index when starting
												}

											}
											else {

												startPlay = 0.0f;
												endPlay = animationComp.m_Duration;
												std::cout << "Animation ID not found!\n";
											}
										}

										// Display the saved animation time ranges
										if (!animationComp.animationVector.empty()) {
											ImGui::Text("Saved Animation Ranges:");
											int i = 0;
											for (const auto& d : animationComp.animationVector)
											{
												ImGui::Text("[%zu] %.2f - %.2f ", std::get<0>(d), std::get<1>(d), std::get<2>(d));
											}
										}



										ImGui::Text("Stuff: [%zu] %.2f - %.2f ", std::get<0>(animationComp.playThisAnimation), std::get<1>(animationComp.playThisAnimation), std::get<2>(animationComp.playThisAnimation));

									}
									else
									{
										ImGui::Text("No Animation found for this model.");
									}


								}

							
							}
						}
						else if (className == "AudioComponent")
						{
							// Custom UI for AudioComponent
							if (ImGui::CollapsingHeader("Audio", ImGuiTreeNodeFlags_None))
							{
								auto& audioComponent = g_Coordinator.GetComponent<AudioComponent>(g_SelectedEntity);
								audioComponent.RegisterProperties();

								const auto& properties = ReflectionManager::Instance().GetProperties("AudioComponent");

								// Handle FilePath Property
								auto filePathProperty = std::find_if(properties.begin(), properties.end(),
									[](const ReflectionPropertyBase* prop) { return prop->GetName() == "FilePath"; });

								if (filePathProperty != properties.end())
								{
									std::string propertyName = "FilePath";
									std::string currentFilePath = (*filePathProperty)->GetValue(&audioComponent);
									std::string newFilePath = currentFilePath;

									ImGui::PushItemWidth(150.0f);
									ImGui::Text("Filename");
									ImGui::SameLine();
									ImGui::PushID(propertyName.c_str());

									char buffer[256];
									memset(buffer, 0, sizeof(buffer));
									strcpy_s(buffer, sizeof(buffer), currentFilePath.c_str());

									ImGui::InputText("##FileName", buffer, IM_ARRAYSIZE(buffer), ImGuiInputTextFlags_ReadOnly);
									ImGui::SameLine();

									// Declare a static variable to store the old value
									static std::string oldFilePath = "";

									if (ImGui::Button("Browse"))
									{
										oldFilePath = currentFilePath; // Capture the old value before opening the dialog
										ImGuiFileDialog::Instance()->OpenDialog("SelectWavFile", "Choose WAV File", ".wav", "../BoofWoof/Assets/Audio");
									}

									if (ImGuiFileDialog::Instance()->Display("SelectWavFile"))
									{
										if (ImGuiFileDialog::Instance()->IsOk())
										{
											std::string selectedFilePath = ImGuiFileDialog::Instance()->GetFilePathName();
											std::string fileName = selectedFilePath.substr(selectedFilePath.find_last_of("/\\") + 1);
											std::string formattedPath = "../BoofWoof/Assets/Audio/" + fileName;

											newFilePath = formattedPath;
											(*filePathProperty)->SetValue(&audioComponent, newFilePath);

											// Execute undo/redo command
											std::string oldValue = oldFilePath;
											Entity entity = g_SelectedEntity;

											g_UndoRedoManager.ExecuteCommand(
												[entity, newFilePath]() {
													auto& component = g_Coordinator.GetComponent<AudioComponent>(entity);
													component.SetFilePath(newFilePath);
												},
												[entity, oldValue]() {
													auto& component = g_Coordinator.GetComponent<AudioComponent>(entity);
													component.SetFilePath(oldValue);
												}
											);
										}
										ImGuiFileDialog::Instance()->Close();
									}

									ImGui::PopID();
								}

								// Handle Volume Property
								auto volumeProperty = std::find_if(properties.begin(), properties.end(),
									[](const ReflectionPropertyBase* prop) { return prop->GetName() == "Volume"; });

								if (volumeProperty != properties.end())
								{
									std::string propertyName = "Volume";
									float volume = std::stof((*volumeProperty)->GetValue(&audioComponent));

									ImGui::Text("Volume  ");
									ImGui::SameLine();
									ImGui::PushID(propertyName.c_str());

									if (ImGui::DragFloat("##Volume", &volume, 0.01f))
									{
										(*volumeProperty)->SetValue(&audioComponent, std::to_string(volume));
									}

									if (ImGui::IsItemActivated())
									{
										oldFloatValues[propertyName] = volume;
									}

									if (ImGui::IsItemDeactivatedAfterEdit())
									{
										float newValue = volume;
										float oldValue = oldFloatValues[propertyName];
										Entity entity = g_SelectedEntity;
										oldFloatValues.erase(propertyName);

										g_UndoRedoManager.ExecuteCommand(
											[entity, newValue]() {
												auto& component = g_Coordinator.GetComponent<AudioComponent>(entity);
												component.SetVolume(newValue);
											},
											[entity, oldValue]() {
												auto& component = g_Coordinator.GetComponent<AudioComponent>(entity);
												component.SetVolume(oldValue);
											}
										);
									}

									ImGui::PopID();
								}

								// Handle ShouldLoop Property
								auto shouldLoopProperty = std::find_if(properties.begin(), properties.end(),
									[](const ReflectionPropertyBase* prop) { return prop->GetName() == "ShouldLoop"; });

								if (shouldLoopProperty != properties.end())
								{
									std::string propertyName = "ShouldLoop";
									bool isLooping = (*shouldLoopProperty)->GetValue(&audioComponent) == "true";

									ImGui::Text("Loops   ");
									ImGui::SameLine();
									ImGui::PushID(propertyName.c_str());

									if (ImGui::Checkbox("##Loops", &isLooping))
									{
										(*shouldLoopProperty)->SetValue(&audioComponent, isLooping ? "true" : "false");
									}

									if (ImGui::IsItemActivated())
									{
										oldBoolValues[propertyName] = isLooping;
									}

									if (ImGui::IsItemDeactivatedAfterEdit())
									{
										bool newValue = isLooping;
										bool oldValue = oldBoolValues[propertyName];
										Entity entity = g_SelectedEntity;
										oldBoolValues.erase(propertyName);

										g_UndoRedoManager.ExecuteCommand(
											[entity, newValue]() {
												auto& component = g_Coordinator.GetComponent<AudioComponent>(entity);
												component.SetLoop(newValue);
											},
											[entity, oldValue]() {
												auto& component = g_Coordinator.GetComponent<AudioComponent>(entity);
												component.SetLoop(oldValue);
											}
										);
									}

									ImGui::PopID();
								}
							}
						}

						else if (className == "BehaviourComponent")
						{
							// Custom UI for BehaviourComponent
							if (ImGui::CollapsingHeader("Behaviour", ImGuiTreeNodeFlags_None))
							{
								auto& behaviourComponent = g_Coordinator.GetComponent<BehaviourComponent>(g_SelectedEntity);
								behaviourComponent.RegisterProperties();

								const auto& properties = ReflectionManager::Instance().GetProperties("BehaviourComponent");

								auto behaviourNameProperty = std::find_if(properties.begin(), properties.end(),
									[](const ReflectionPropertyBase* prop) { return prop->GetName() == "BehaviourName"; });

								if (behaviourNameProperty != properties.end())
								{
									std::string propertyName = "BehaviourName";
									std::string currentBehaviourName = (*behaviourNameProperty)->GetValue(&behaviourComponent);
									std::string newBehaviourName = currentBehaviourName;

									const char* behaviourNames[] = { "Null", "Player", "Treat", "Rex", "RexChase", "Toys", "Puppy"};
									int currentItem = 0;

									for (int i = 0; i < IM_ARRAYSIZE(behaviourNames); ++i)
									{
										if (behaviourNames[i] == currentBehaviourName)
										{
											currentItem = i;
											break;
										}
									}

									ImGui::PushItemWidth(123.0f);
									ImGui::Text("Name    ");
									ImGui::SameLine();
									ImGui::PushID(propertyName.c_str());

									std::string oldBehaviourName = currentBehaviourName;

									if (ImGui::Combo("##NameCombo", &currentItem, behaviourNames, IM_ARRAYSIZE(behaviourNames)))
									{

										
											newBehaviourName = behaviourNames[currentItem];
											(*behaviourNameProperty)->SetValue(&behaviourComponent, newBehaviourName);
											if (g_Coordinator.GetSystem<LogicSystem>()->ApprovingScriptChange(g_SelectedEntity)) {
												Entity entity = g_SelectedEntity;
												g_Coordinator.GetSystem<LogicSystem>()->InitScript(entity);

												g_UndoRedoManager.ExecuteCommand(
													[entity, newBehaviourName]() {
														auto& component = g_Coordinator.GetComponent<BehaviourComponent>(entity);
														component.SetBehaviourName(newBehaviourName);
													},
													[entity, oldBehaviourName]() {
														auto& component = g_Coordinator.GetComponent<BehaviourComponent>(entity);
														component.SetBehaviourName(oldBehaviourName);
													}
												);
											}								
									}

									ImGui::PopID();								
								}
							}
						}
						else if (className == "CollisionComponent")
						{
							// Custom UI for CollisionComponent
							if (ImGui::CollapsingHeader("Collision", ImGuiTreeNodeFlags_None))
							{
								auto& collisionComponent = g_Coordinator.GetComponent<CollisionComponent>(g_SelectedEntity);
								if (g_Coordinator.HaveComponent<GraphicsComponent>(g_SelectedEntity))
								{
									auto& graphicsComponent = g_Coordinator.GetComponent<GraphicsComponent>(g_SelectedEntity);

									// Toggle Dynamic/Static checkbox
									bool isDynamic = collisionComponent.IsDynamic();
									if (ImGui::Checkbox("Dynamic", &isDynamic)) {
										// Ensure only one mode is enabled at a time
										if (isDynamic) {
											collisionComponent.SetIsKinematic(false);  // Disable kinematic mode if dynamic is set
										}

										if (collisionComponent.IsDynamic() != isDynamic) {
											collisionComponent.SetIsDynamic(isDynamic);
											// Call UpdateEntityBody instead of removing and adding the entity again
											g_Coordinator.GetSystem<MyPhysicsSystem>()->UpdateEntityBody(g_SelectedEntity, 0.0f);
										}
									}

									// Toggle Kinematic checkbox
									bool isKinematic = collisionComponent.IsKinematic();
									if (ImGui::Checkbox("Kinematic", &isKinematic)) {
										collisionComponent.SetIsKinematic(isKinematic);

										// Ensure only one mode is enabled at a time
										if (isKinematic) {
											collisionComponent.SetIsDynamic(false);  // Disable dynamic mode if kinematic is set
										}

										// Update the physics system to apply the changes
										g_Coordinator.GetSystem<MyPhysicsSystem>()->UpdateEntityBody(g_SelectedEntity, 0.0f);
									}

									// Debugging output for motion type
									ImGui::Text("Current Motion Type: %s",
										collisionComponent.IsDynamic() ? "Dynamic" :
										(collisionComponent.IsKinematic() ? "Kinematic" : "Static"));

									// Checkbox to mark as player
									bool isPlayer = collisionComponent.IsPlayer();
									if (ImGui::Checkbox("Is Player", &isPlayer))
									{
										collisionComponent.SetIsPlayer(isPlayer);

										// Automatically set to dynamic if it's marked as a player
										if (isPlayer)
										{
											collisionComponent.SetIsDynamic(true);
										}

										g_Coordinator.GetSystem<MyPhysicsSystem>()->UpdateEntityBody(g_SelectedEntity, 0.0f);
									}

									// Debugging for player type
									ImGui::Text("Is Player: %s", collisionComponent.IsPlayer() ? "Yes" : "No");

									// Checkbox for Sensor flag
									bool isSensor = collisionComponent.IsSensor();
									if (ImGui::Checkbox("Is Sensor", &isSensor)) {
										collisionComponent.SetIsSensor(isSensor);

										// Sensors also need body rebuild to apply the Jolt sensor flag
										g_Coordinator.GetSystem<MyPhysicsSystem>()->UpdateEntityBody(g_SelectedEntity, 0.0f);
									}

									// AABB Size Editor
									graphicsComponent.boundingBox = collisionComponent.GetAABBSize();

									// Retrieve the scale from the TransformComponent
									if (g_Coordinator.HaveComponent<TransformComponent>(g_SelectedEntity)) {
										auto& transformComponent = g_Coordinator.GetComponent<TransformComponent>(g_SelectedEntity);
										glm::vec3 scale = transformComponent.GetScale();

										// Compute the effective AABB by scaling the bounding box
										glm::vec3 effectiveAABB = graphicsComponent.boundingBox * scale;

										// Set a minimum size to prevent crashes
										glm::vec3 minAABBSize(0.1f, 0.1f, 0.1f); // Minimum size for AABB
										if (ImGui::DragFloat3("Unscaled AABB Size", &graphicsComponent.boundingBox.x, 0.05f, 0.1f, 100.0f, "%.2f")) {
											// Clamp the values to prevent zero or invalid sizes
											graphicsComponent.boundingBox.x = glm::max(graphicsComponent.boundingBox.x, minAABBSize.x);
											graphicsComponent.boundingBox.y = glm::max(graphicsComponent.boundingBox.y, minAABBSize.y);
											graphicsComponent.boundingBox.z = glm::max(graphicsComponent.boundingBox.z, minAABBSize.z);

											// Update AABB size in the CollisionComponent
											collisionComponent.SetAABBSize(graphicsComponent.boundingBox);

											// Update the physics body in real time
											g_Coordinator.GetSystem<MyPhysicsSystem>()->UpdateEntityBody(g_SelectedEntity, 0.0f);
										}

										// Debug Output for both unscaled and effective (scaled) AABB
										ImGui::Text("Unscaled AABB: (%.2f, %.2f, %.2f)", graphicsComponent.boundingBox.x, graphicsComponent.boundingBox.y, graphicsComponent.boundingBox.z);
										ImGui::Text("Scaled AABB: (%.2f, %.2f, %.2f)", effectiveAABB.x, effectiveAABB.y, effectiveAABB.z);
									}

									// Add UI for editing collision offset
									glm::vec3 currentOffset = collisionComponent.GetAABBOffset();
									if (ImGui::DragFloat3("AABB Offset", &currentOffset.x, 0.05f, -100.0f, 100.0f, "%.2f")) {
										collisionComponent.SetAABBOffset(currentOffset);

										// Update the physics body in real time to reflect the offset
										g_Coordinator.GetSystem<MyPhysicsSystem>()->UpdateEntityBody(g_SelectedEntity, 0.0f);

									}

									// Debug Output
									ImGui::Text("Current Offset: (%.2f, %.2f, %.2f)", currentOffset.x, currentOffset.y, currentOffset.z);

									// Debug for last colliding entity
									ImGui::Text("Colliding: %s", collisionComponent.GetIsColliding() ? "Yes" : "No");
									ImGui::Text("Last Collided Object: %s", collisionComponent.GetLastCollidedObjectName().c_str());

									// Mass Editor
									float currentMass = collisionComponent.GetMass();
									if (ImGui::DragFloat("Mass", &currentMass, 0.1f, 0.0f, 100.0f, "%.2f")) {
										collisionComponent.SetMass(currentMass);

										// Update the physics body in real time to reflect the new mass
										g_Coordinator.GetSystem<MyPhysicsSystem>()->UpdateEntityBody(g_SelectedEntity, currentMass);
									}

									// Debug Output for mass
									ImGui::Text("Actual Mass: %.2f", collisionComponent.GetActualMass());

								}
							}
						}

						else if (className == "CameraComponent")
						{
							// Custom UI for CameraComponent
							if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_None))
							{
								auto& cameraComponent = g_Coordinator.GetComponent<CameraComponent>(g_SelectedEntity);
								cameraComponent.RegisterProperties();

								const auto& properties = ReflectionManager::Instance().GetProperties("CameraComponent");

								std::unordered_map<std::string, int> spaceMapping = {
									{"WorldUp", 1}, {"Pitch", 3}, {"Yaw", 5}
								};

								// Handle Camera Property
								for (const auto& property : properties)
								{
									std::string propertyName = property->GetName();

									if (spaceMapping.find(propertyName) != spaceMapping.end())
									{
										// Append the corresponding number of spaces
										propertyName += std::string(spaceMapping[propertyName], ' ');
									}

									ImGui::PushItemWidth(250.0f);
									ImGui::Text("%s", propertyName.c_str());
									ImGui::SameLine();
									std::string widgetID = "##" + propertyName;

									ImGui::PushID(widgetID.c_str());

									// For glm::vec3 properties
									if (property->GetValue(&cameraComponent).find(",") != std::string::npos)
									{
										glm::vec3 vecValue = SerializationHelpers::DeserializeVec3(property->GetValue(&cameraComponent));

										if (ImGui::DragFloat3("##Drag", &vecValue.x, 0.1f))
										{
											property->SetValue(&cameraComponent, SerializationHelpers::SerializeVec3(vecValue));
										}

										if (ImGui::IsItemActivated())
										{
											// Store old value
											oldVec3Values[propertyName] = vecValue;
										}

										if (ImGui::IsItemDeactivatedAfterEdit())
										{
											glm::vec3 newValue = vecValue;
											glm::vec3 oldValue = oldVec3Values[propertyName];
											Entity entity = g_SelectedEntity;

											// Clean up the stored old value
											oldVec3Values.erase(propertyName);

											g_UndoRedoManager.ExecuteCommand(
												[entity, propertyName, newValue]() {
													auto& component = g_Coordinator.GetComponent<CameraComponent>(entity);
													const auto& properties = ReflectionManager::Instance().GetProperties("CameraComponent");
													auto propIt = std::find_if(properties.begin(), properties.end(),
														[&propertyName](const ReflectionPropertyBase* prop) {
															return prop->GetName() == propertyName;
														});
													if (propIt != properties.end())
													{
														(*propIt)->SetValue(&component, SerializationHelpers::SerializeVec3(newValue));
													}
												},
												[entity, propertyName, oldValue]() {
													auto& component = g_Coordinator.GetComponent<CameraComponent>(entity);
													const auto& properties = ReflectionManager::Instance().GetProperties("CameraComponent");
													auto propIt = std::find_if(properties.begin(), properties.end(),
														[&propertyName](const ReflectionPropertyBase* prop) {
															return prop->GetName() == propertyName;
														});
													if (propIt != properties.end())
													{
														(*propIt)->SetValue(&component, SerializationHelpers::SerializeVec3(oldValue));
													}
												}
											);
										}
									}
									else
									{
										// Check if the value is a boolean or float
										std::string propertyValue = property->GetValue(&cameraComponent);

										bool isBool = false;
										bool boolValue = false;

										// Check if it's a boolean
										if (propertyValue == "true" || propertyValue == "false")
										{
											isBool = true;
											boolValue = (propertyValue == "true");
										}

										if (isBool)
										{
											// Handle boolean property
											if (ImGui::Checkbox("##Checkbox", &boolValue))
											{
												property->SetValue(&cameraComponent, boolValue ? "true" : "false");
											}

											if (ImGui::IsItemActivated())
											{
												// Store old boolean value
												oldBoolValues[propertyName] = boolValue;
											}

											if (ImGui::IsItemDeactivatedAfterEdit())
											{
												bool newValue = boolValue;
												bool oldValue = oldBoolValues[propertyName];
												Entity entity = g_SelectedEntity;

												// Clean up the stored old value
												oldBoolValues.erase(propertyName);

												g_UndoRedoManager.ExecuteCommand(
													[entity, propertyName, newValue]() {
														auto& component = g_Coordinator.GetComponent<CameraComponent>(entity);
														const auto& properties = ReflectionManager::Instance().GetProperties("CameraComponent");
														auto propIt = std::find_if(properties.begin(), properties.end(),
															[&propertyName](const ReflectionPropertyBase* prop) {
																return prop->GetName() == propertyName;
															});
														if (propIt != properties.end())
														{
															(*propIt)->SetValue(&component, newValue ? "true" : "false");
														}
													},
													[entity, propertyName, oldValue]() {
														auto& component = g_Coordinator.GetComponent<CameraComponent>(entity);
														const auto& properties = ReflectionManager::Instance().GetProperties("CameraComponent");
														auto propIt = std::find_if(properties.begin(), properties.end(),
															[&propertyName](const ReflectionPropertyBase* prop) {
																return prop->GetName() == propertyName;
															});
														if (propIt != properties.end())
														{
															(*propIt)->SetValue(&component, oldValue ? "true" : "false");
														}
													}
												);
											}
										}
										else
										{
											// For scalar properties (float)
											float floatValue = std::stof(property->GetValue(&cameraComponent));

											if (ImGui::DragFloat("##Drag", &floatValue, 0.1f))
											{
												property->SetValue(&cameraComponent, std::to_string(floatValue));
											}

											if (ImGui::IsItemActivated())
											{
												// Store old value
												oldFloatValues[propertyName] = floatValue;
											}

											if (ImGui::IsItemDeactivatedAfterEdit())
											{
												float newValue = floatValue;
												float oldValue = oldFloatValues[propertyName];
												Entity entity = g_SelectedEntity;

												// Clean up the stored old value
												oldFloatValues.erase(propertyName);

												g_UndoRedoManager.ExecuteCommand(
													[entity, propertyName, newValue]() {
														auto& component = g_Coordinator.GetComponent<CameraComponent>(entity);
														const auto& properties = ReflectionManager::Instance().GetProperties("CameraComponent");
														auto propIt = std::find_if(properties.begin(), properties.end(),
															[&propertyName](const ReflectionPropertyBase* prop) {
																return prop->GetName() == propertyName;
															});
														if (propIt != properties.end())
														{
															(*propIt)->SetValue(&component, std::to_string(newValue));
														}
													},
													[entity, propertyName, oldValue]() {
														auto& component = g_Coordinator.GetComponent<CameraComponent>(entity);
														const auto& properties = ReflectionManager::Instance().GetProperties("CameraComponent");
														auto propIt = std::find_if(properties.begin(), properties.end(),
															[&propertyName](const ReflectionPropertyBase* prop) {
																return prop->GetName() == propertyName;
															});
														if (propIt != properties.end())
														{
															(*propIt)->SetValue(&component, std::to_string(oldValue));
														}
													}
												);
											}
										}
									}
									ImGui::PopID();
								}
							}
						}

						else if (className == "ParticleComponent")
						{
							// Custom UI for MetadataComponent
							if (ImGui::CollapsingHeader("Particle", ImGuiTreeNodeFlags_None))
							{
								auto& particleComponent = g_Coordinator.GetComponent<ParticleComponent>(g_SelectedEntity);
								// set the particle type
								ParticleType particleType = particleComponent.getParticleType();
								ImGui::Text("Particle Type");
								ImGui::SameLine();
								ImGui::PushItemWidth(125.0f);
								ImGui::PushID("ParticleType");

								std::string typestring{};
								switch (particleType)
								{
								case ParticleType::TEXTURED_3D:
									typestring = "TEXTURED_3D";
									break;
								case ParticleType::TEXTURED:
									typestring = "TEXTURED";
									break;
								case ParticleType::POINT:
									typestring = "POINT";
									break;
								}
								//select bar
								if (ImGui::BeginCombo("##ParticleType", typestring.c_str()))
								{
									if (ImGui::Selectable("TEXTURED_3D", typestring == "TEXTURED_3D"))
									{
										particleComponent.setParticleType(ParticleType::TEXTURED_3D);
									}
									if (ImGui::Selectable("TEXTURED", typestring == "TEXTURED"))
									{
										particleComponent.setParticleType(ParticleType::TEXTURED);
									}
									if (ImGui::Selectable("POINT", typestring == "POINT"))
									{
										particleComponent.setParticleType(ParticleType::POINT);
									}
									
									ImGui::EndCombo();
								}
								ImGui::PopID();

								// set the properties
								glm::vec3 position = particleComponent.getPosMin();
								glm::vec3 positionMax = particleComponent.getPosMax();

								ImGui::Text("Position Min");
								ImGui::SameLine();
								ImGui::PushItemWidth(125.0f);
								ImGui::PushID("PositionMin");

								if (ImGui::DragFloat3("##PositionMin", &position.x, 0.1f))
								{
									particleComponent.setPosMin(position);
								}

								ImGui::PopID();
								ImGui::PopItemWidth();

								ImGui::Text("Position Max");
								ImGui::SameLine();
								ImGui::PushItemWidth(125.0f);
								ImGui::PushID("PositionMax");

								if (ImGui::DragFloat3("##PositionMax", &positionMax.x, 0.1f))
								{
									particleComponent.setPosMax(positionMax);
								}

								ImGui::PopID();
								ImGui::PopItemWidth();

								// set the density
								float density = particleComponent.getDensity();
								ImGui::Text("Density");
								ImGui::SameLine();
								ImGui::PushItemWidth(125.0f);
								ImGui::PushID("Density");

								if (ImGui::DragFloat("##Density", &density, 0.1f))
								{
									particleComponent.setDensity(density);
								}

								ImGui::PopID();
								ImGui::PopItemWidth();

								// Fetch the current velocity minimum and maximum from the particle component
								float vel_min = particleComponent.getVelocityMin();
								float vel_max = particleComponent.getVelocityMax();

								// Begin the velocity section
								ImGui::Text("Velocity: Min");
								ImGui::SameLine();
								ImGui::PushItemWidth(125.0f);
								ImGui::PushID("VelocityMin");

								if (ImGui::DragFloat("##VelocityMin", &vel_min, 0.1f, 0.0f, vel_max)) {
									particleComponent.setVelocityMin(vel_min);
								}
								ImGui::PopID();
								ImGui::PopItemWidth();
								ImGui::SameLine();

								ImGui::Text("Max");
								ImGui::SameLine();
								ImGui::PushItemWidth(125.0f);
								ImGui::PushID("VelocityMax");
								if (ImGui::DragFloat("##VelocityMax", &vel_max, 0.1f, vel_min, FLT_MAX)) {
									particleComponent.setVelocityMax(vel_max);
								}
								ImGui::PopID();
								ImGui::PopItemWidth();

								// set TargetPositions
								std::vector<glm::vec3> targetPositions = particleComponent.getTargetPositions();
								if (ImGui::CollapsingHeader("Target Positions")) {

									for (size_t i = 0; i < targetPositions.size(); ++i) {
										ImGui::PushID(static_cast<int>(i));
										ImGui::Text("Position %zu", i + 1);
										ImGui::SameLine();

										if (ImGui::DragFloat3("##TargetPos", &targetPositions[i].x, 0.1f)) {
											particleComponent.setTargetPositions(targetPositions);
										}
										ImGui::PopID();
									}


									if (ImGui::Button("Add Position")) {
										targetPositions.push_back(glm::vec3(0.0f));
										particleComponent.setTargetPositions(targetPositions);
									}

									if (!targetPositions.empty()) {
										ImGui::SameLine();
										if (ImGui::Button("Remove Position")) {
											targetPositions.pop_back();
											particleComponent.setTargetPositions(targetPositions);
										}
									}
								}

								if (particleType == ParticleType::POINT) {
									// set particle size
									float particleSize = particleComponent.getParticleSize();
									ImGui::Text("Particle Size");
									ImGui::SameLine();
									ImGui::PushItemWidth(125.0f);
									ImGui::PushID("ParticleSize");

									if (ImGui::DragFloat("##ParticleSize", &particleSize, 0.1f))
									{
										particleComponent.setParticleSize(particleSize);
									}

									ImGui::PopID();
									ImGui::PopItemWidth();

									// set particle color
									glm::vec3 particleColor = particleComponent.getParticleColor();
									ImGui::Text("Particle Color");
									ImGui::SameLine();
									ImGui::PushItemWidth(125.0f);
									ImGui::PushID("ParticleColor");

									if (ImGui::ColorEdit4("##ParticleColor", &particleColor.x))
									{
										particleComponent.setParticleColor(particleColor);
									}

									ImGui::PopID();
									ImGui::PopItemWidth();

									
								}
								else if (particleType == ParticleType::TEXTURED) {
									// set the texture
									std::string textureName = particleComponent.getParticleTexturename();
									ImGui::Text("Texture :");
									ImGui::SameLine();
									ImGui::PushItemWidth(125.0f);
									ImGui::Text("%s", textureName.c_str());
									ImGui::SameLine();
									ImGui::PushItemWidth(125.0f);
									ImGui::PushID("Texture");
									if (ImGui::Button("Change Texture"))
									{
										ImGuiFileDialog::Instance()->OpenDialog("ChangeTexture", "Choose File", ".png,.dds", "../BoofWoof/Assets");
									}

									if (ImGuiFileDialog::Instance()->Display("ChangeTexture"))
									{
										if (ImGuiFileDialog::Instance()->IsOk())
										{
											// User selected a file
											std::string selectedFile = ImGuiFileDialog::Instance()->GetCurrentFileName();
											size_t lastDotPos = selectedFile.find_last_of(".");
											if (lastDotPos != std::string::npos)
											{
												selectedFile = selectedFile.substr(0, lastDotPos);
											}
											particleComponent.setParticleTexturename(selectedFile);

										}
										ImGuiFileDialog::Instance()->Close();
									}

									ImGui::PopID();
									ImGui::PopItemWidth();

								}else if (particleType == ParticleType::TEXTURED_3D) {
									std::string modelname = particleComponent.getParticleModelname();
									ImGui::Text("Model :");
									ImGui::SameLine();
									ImGui::PushItemWidth(125.0f);
									ImGui::Text("%s", modelname.c_str());
									ImGui::SameLine();
									ImGui::PushItemWidth(125.0f);
									ImGui::PushID("Model");
									if (ImGui::Button("Change Model"))
									{
										ImGuiFileDialog::Instance()->OpenDialog("ChangeModel", "Choose File", ".obj", "../BoofWoof/Assets/Objects");
									}

									if (ImGuiFileDialog::Instance()->Display("ChangeModel"))
									{
										if (ImGuiFileDialog::Instance()->IsOk())
										{
											// User selected a file
											std::string selectedFile = ImGuiFileDialog::Instance()->GetCurrentFileName();
											size_t lastDotPos = selectedFile.find_last_of(".");
											if (lastDotPos != std::string::npos)
											{
												selectedFile = selectedFile.substr(0, lastDotPos);
											}
											particleComponent.setParticleModelname(selectedFile);
											particleComponent.setParticleTexturename(selectedFile);

										}
										ImGuiFileDialog::Instance()->Close();
									}

									ImGui::PopID();
									ImGui::PopItemWidth();
									
								}
								float opacity = particleComponent.getOpacity();
								ImGui::Text("Opacity");
								ImGui::SameLine();
								ImGui::PushItemWidth(125.0f);
								ImGui::PushID("Opacity");

								if (ImGui::DragFloat("##Opacity", &opacity, 0.1f, 0.0f, 1.0f))
								{
									particleComponent.setOpacity(opacity);
								}

								ImGui::PopID();
								ImGui::PopItemWidth();

							}
						}


						else if (className == "MaterialComponent")
						{
							auto& graphicsComponent = g_Coordinator.GetComponent<GraphicsComponent>(g_SelectedEntity); // Reference to GraphicsComponent
						
							std::string objectName = g_Coordinator.GetComponent<MetadataComponent>(g_SelectedEntity).GetName();
						
							auto& material = graphicsComponent.material;
						

						
						
							std::string loadSaveLocation = FILEPATH_ASSET_MATERIAL + "\\" + objectName + ".mat";
						
							std::string loadSaveLocationExt = FILEPATH_ASSET_MATERIAL + "\\" + graphicsComponent.material.GetMaterialNameMat();
						
						
							auto& materialComp = g_Coordinator.GetComponent<MaterialComponent>(g_SelectedEntity);
						
						
							materialComp = material;
						
							if (!material.loadedMaterial)
							{
								//if (material.LoadMaterialDescriptor(loadSaveLocation))
								{
									std::cout << "load material is true\n";
									material.loadedMaterial = true;
								}
							}
							else
							{
							

								if (ImGui::CollapsingHeader("Materials", ImGuiTreeNodeFlags_DefaultOpen))
								{
									// do some search for the Shader name:
							
							
									static float WidthIndentation = 125.0f;
									static float ComboIdentation = 300.0f;
							
									/*
										MATERIAL NAME
									*/
							
									ImGui::Text("Material Name: ");
									ImGui::SameLine(WidthIndentation);
							
									ImGui::PushItemWidth(150); // Set the width in pixels
							
									std::string matNameBuffer = "Default Material";
									matNameBuffer.resize(256);
							
									if (ImGui::InputText("##MatName1001", &matNameBuffer[0], matNameBuffer.capacity())) {
										// Trim to actual length after editing
										matNameBuffer.resize(strlen(matNameBuffer.c_str()));
							
										// You might also want to trigger an update or validate the name here
										graphicsComponent.material.SetMaterialName(matNameBuffer); // If there's a setter
										graphicsComponent.material.SetMaterialNameMat(matNameBuffer); // If there's a setter
									}
							
									// ImGui::InputText("##MatName1001 ", graphicsComponent.material.GetMaterialName().data(), graphicsComponent.material.GetMaterialName().capacity() + 1);
							
							
							
							
									ImGui::PushID("LoadMaterialz");
									ImGui::SameLine();
							
									// Store old value before opening the file dialog
									// static std::string oldTextureName = "";
									if (ImGui::Button("Load Material"))
									{
										//oldTextureName = currentTextureName; // Capture the old value
										ImGuiFileDialog::Instance()->OpenDialog("LoadMat", "Choose File", ".mat", "../BoofWoof/Assets/Material/");
							
									}
							
									if (ImGuiFileDialog::Instance()->Display("LoadMat"))
									{
										if (ImGuiFileDialog::Instance()->IsOk())
										{
											// User selected a file
											std::string selectedFile = ImGuiFileDialog::Instance()->GetCurrentFileName();
											size_t lastDotPos = selectedFile.find_last_of(".");
											if (lastDotPos != std::string::npos)
											{
												selectedFile = selectedFile.substr(0, lastDotPos);
											}
							
											graphicsComponent.LoadMaterialDesc(FILEPATH_ASSET_MATERIAL + "\\" + selectedFile + ".mat");
							
							
											// Execute undo/redo command
											/*std::string oldValue = oldTextureName;
											Entity entity = g_SelectedEntity;*/
							
											/*g_UndoRedoManager.ExecuteCommand(
												[entity, newTextureName]() {
													auto& component = g_Coordinator.GetComponent<GraphicsComponent>(entity);
													component.setTexture(newTextureName);
												},
												[entity, oldValue]() {
													auto& component = g_Coordinator.GetComponent<GraphicsComponent>(entity);
													component.setTexture(oldValue);
												}
											);*/
										}
										ImGuiFileDialog::Instance()->Close();
									}
							
							
									ImGui::PopID();
							
							
									/*
										SHADER
									*/
							
									ImGui::Text("Material");
									ImGui::SameLine(WidthIndentation);
							
									std::string comboItems;
									for (const auto& item : g_AssetManager.shdrpgmOrder) {
										comboItems += item;
										comboItems += '\0'; // Null-terminate each item
									}
							
									// Ensure default selection is the first item
									if (material.GetShaderIndex() < 0 || material.GetShaderIndex() >= g_AssetManager.shdrpgmOrder.size()) {
										material.SetShaderIndex(7); // Default to the first item
										material.SetShaderName(g_AssetManager.shdrpgmOrder[7]); // Set the chosen shader
									}
							
									int& Material_current_idx = material.GetShaderIDRef(); // Index for the selected item
							
									ImGui::SetNextItemWidth(WidthIndentation); // Set combo box width
									if (ImGui::Combo("##MatCombo1", &Material_current_idx, comboItems.c_str())) {
										// Update material.GetShaderIndex() with the selected index
										material.SetShaderIndex(Material_current_idx);
							
							
							
										std::cout << material.GetShaderIndex() << '\n';
							
										graphicsComponent.material.SetShaderIndex(Material_current_idx);
							
										// Retrieve the selected string from the original vector
										//material.SetShaderName(g_AssetManager.shdrpgmOrder[material.GetShaderIndex()]);
										graphicsComponent.material.SetShaderName(g_AssetManager.shdrpgmOrder[material.GetShaderIndex()]);
									}
							
	//								graphicsComponent.material.SetDiffuseName(graphicsComponent.getModelName());
	//								graphicsComponent.material.SetDiffuseID(g_ResourceManager.GetTextureDDS(graphicsComponent.getModelName()));
							
							
//									std::cout << materialComp.GetDiffuseName() << '\t' << material.GetDiffuseName() << '\t' << materialComp.GetDiffuseID() << '\t' << material.GetDiffuseID();
							
							
									//									std::cout << materialComp.GetShaderName() << '\n';
							
									ImGui::Text("Texture ");
									ImGui::SameLine(WidthIndentation);
									ImGui::PushItemWidth(150); // Set the width in pixels
									ImGui::InputText("##TexDiff1", graphicsComponent.material.GetDiffuseName().data(), graphicsComponent.material.GetDiffuseName().capacity() + 1);
							
							
									ImGui::PushID("Textures");
									ImGui::SameLine();
							
									// Store old value before opening the file dialog
									 static std::string oldTextureName = "";
									if (ImGui::Button("Set Texture"))
									{
										//oldTextureName = currentTextureName; // Capture the old value
										ImGuiFileDialog::Instance()->OpenDialog("SetTexture", "Choose File", ".png,.dds", "../BoofWoof/Assets");
							
									}
							
									if (ImGuiFileDialog::Instance()->Display("SetTexture"))
									{
										if (ImGuiFileDialog::Instance()->IsOk())
										{
											// User selected a file
											std::string selectedFile = ImGuiFileDialog::Instance()->GetCurrentFileName();
											size_t lastDotPos = selectedFile.find_last_of(".");
											if (lastDotPos != std::string::npos)
											{
												selectedFile = selectedFile.substr(0, lastDotPos);
											}
							
//											newTextureName = selectedFile;
//											(*textureNameProperty)->SetValue(&graphicsComponent, newTextureName);
											int textureId = g_ResourceManager.GetTextureDDS(selectedFile);
											graphicsComponent.SetDiffuse(textureId);
											graphicsComponent.AddTexture(textureId);
											graphicsComponent.setTexture(selectedFile);
						
											std::cout << textureId << '\n';
							
											graphicsComponent.material.SetDiffuseID(textureId);
											graphicsComponent.material.SetDiffuseName(selectedFile);
											 material.SetDiffuseID(textureId);
											 material.SetDiffuseName(selectedFile);
							
											std::cout << material.GetDiffuseID() << graphicsComponent.material.GetDiffuseID() << '\n';
							

							
											// Execute undo/redo command
											// std::string oldValue = oldTextureName;
											// Entity entity = g_SelectedEntity;
											// 
											// g_UndoRedoManager.ExecuteCommand(
											// 	[entity, newTextureName]() {
											// 		auto& component = g_Coordinator.GetComponent<GraphicsComponent>(entity);
											// 		component.setTexture(newTextureName);
											// 	},
											// 	[entity, oldValue]() {
											// 		auto& component = g_Coordinator.GetComponent<GraphicsComponent>(entity);
											// 		component.setTexture(oldValue);
											// 	}
											// );
										}
										ImGuiFileDialog::Instance()->Close();
									}
							
							
									ImGui::PopID();
							
									ImGui::SameLine();
									if (ImGui::Button("Remove Texture")) {
										graphicsComponent.material.SetDiffuseName("NothingTexture");
										graphicsComponent.material.SetDiffuseID(-1);
										graphicsComponent.RemoveDiffuse();
									}
							
									ImGui::Text("Normal ");
									ImGui::SameLine(WidthIndentation);
									ImGui::PushItemWidth(150); // Set the width in pixels
									ImGui::InputText("##TexNorm1", graphicsComponent.GetNormalName().data(), graphicsComponent.GetNormalName().capacity() + 1);
							
									ImGui::PushID("Normal");
									ImGui::SameLine();
							
									// Store old value before opening the file dialog
									// static std::string oldTextureName = "";
									if (ImGui::Button("Set Normal"))
									{
										//oldTextureName = currentTextureName; // Capture the old value
										ImGuiFileDialog::Instance()->OpenDialog("SetN", "Choose Normal", ".png,.dds", "../BoofWoof/Assets");
									}
							
									if (ImGuiFileDialog::Instance()->Display("SetN"))
									{
										if (ImGuiFileDialog::Instance()->IsOk())
										{
											// User selected a file
											std::string selectedFile = ImGuiFileDialog::Instance()->GetCurrentFileName();
											size_t lastDotPos = selectedFile.find_last_of(".");
											if (lastDotPos != std::string::npos)
											{
												selectedFile = selectedFile.substr(0, lastDotPos);
											}
							
											//newTextureName = selectedFile;
											//(*textureNameProperty)->SetValue(&graphicsComponent, newTextureName);
											int textureId = g_ResourceManager.GetTextureDDS(selectedFile);
							
							
											graphicsComponent.SetNormal(textureId);
							
											material.SetNormalID(textureId);
											material.SetNormalName(selectedFile);
							
							
											//std::cout << "Testing PLS WROK\t" << material.materialDesc.DiffuseID << '\t' << material.materialDesc.textureDiffuse << '\n';
							
							
							
											// Execute undo/redo command
											/*std::string oldValue = oldTextureName;
											Entity entity = g_SelectedEntity;*/
							
											/*g_UndoRedoManager.ExecuteCommand(
												[entity, newTextureName]() {
													auto& component = g_Coordinator.GetComponent<GraphicsComponent>(entity);
													component.setTexture(newTextureName);
												},
												[entity, oldValue]() {
													auto& component = g_Coordinator.GetComponent<GraphicsComponent>(entity);
													component.setTexture(oldValue);
												}
											);*/
										}
										ImGuiFileDialog::Instance()->Close();
									}
									ImGui::SameLine();
									if (ImGui::Button("Remove Normal")) {
										graphicsComponent.material.SetNormalName("NothingNormal");
										graphicsComponent.material.SetNormalID(-1);
										graphicsComponent.RemoveNormal();
									}
							
									ImGui::PopID();
									ImGui::Text("Height ");
									ImGui::SameLine(WidthIndentation);
									ImGui::PushItemWidth(150); // Set the width in pixels
									ImGui::InputText("##TexHeight1 ", graphicsComponent.GetHeightName().data(), graphicsComponent.GetHeightName().capacity() + 1);
							
							
									ImGui::PushID("Height");
									ImGui::SameLine();
							
									// Store old value before opening the file dialog
									// static std::string oldTextureName = "";
									if (ImGui::Button("Set Height"))
									{
										//oldTextureName = currentTextureName; // Capture the old value
										ImGuiFileDialog::Instance()->OpenDialog("SetH", "Choose Height", ".png,.dds", "../BoofWoof/Assets");
									}
							
									if (ImGuiFileDialog::Instance()->Display("SetH"))
									{
										if (ImGuiFileDialog::Instance()->IsOk())
										{
											// User selected a file
											std::string selectedFile = ImGuiFileDialog::Instance()->GetCurrentFileName();
											size_t lastDotPos = selectedFile.find_last_of(".");
											if (lastDotPos != std::string::npos)
											{
												selectedFile = selectedFile.substr(0, lastDotPos);
											}
							
											//newTextureName = selectedFile;
											//(*textureNameProperty)->SetValue(&graphicsComponent, newTextureName);
											int textureId = g_ResourceManager.GetTextureDDS(selectedFile);
							
							
							
											graphicsComponent.SetHeight(textureId);
											material.SetHeightID(textureId);
											material.SetHeightName(selectedFile);
							
							
											//std::cout << "Testing PLS WROK\t" << material.materialDesc.DiffuseID << '\t' << material.materialDesc.textureDiffuse << '\n';
							
							
							
											// Execute undo/redo command
											/*std::string oldValue = oldTextureName;
											Entity entity = g_SelectedEntity;*/
							
											/*g_UndoRedoManager.ExecuteCommand(
												[entity, newTextureName]() {
													auto& component = g_Coordinator.GetComponent<GraphicsComponent>(entity);
													component.setTexture(newTextureName);
												},
												[entity, oldValue]() {
													auto& component = g_Coordinator.GetComponent<GraphicsComponent>(entity);
													component.setTexture(oldValue);
												}
											);*/
										}
										ImGuiFileDialog::Instance()->Close();
									}
							
							
							
									ImGui::PopID();
							
									ImGui::SameLine();
									if (ImGui::Button("Remove Height")) {
										graphicsComponent.material.SetHeightName("NothingHeight");
										graphicsComponent.material.SetHeightID(-1);
										graphicsComponent.RemoveHeight();
									}
							
							
							
									/*
										Main Maps
									*/
							
							
									//ImGui::BeginChild()ImGui::Text("Main Maps");
									//if (ImGui::TreeNode("Main Maps")) {
									ImGui::Text("Main Maps");
							
							
									/*
										Albedo
									*/
							
									ImGui::Button("##MatButton1 ", ImVec2(15, 15)); // Create a visual box
							
									if (ImGui::BeginDragDropTarget()) {
										// Check if a payload is available
										if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_ITEM_TYPE")) {
											// Process the payload data (for example, a file path)
											const char* droppedData = (const char*)payload->Data;
											ImGui::Text("Dropped: %s", droppedData);
										}
										ImGui::EndDragDropTarget();
									}
							
									ImGui::SameLine();
							
									//Put a Add texture here
							
									ImGui::Text("Albedo");
									ImGui::SameLine();
							
									float color[4];// = { 1.0f, 0.0f, 0.0f, 1.0f }; // Example initial color (red)
							
									color[0] = material.GetMaterialRed();
									color[1] = material.GetMaterialGreen();
									color[2] = material.GetMaterialBlue();
									color[3] = material.GetMaterialAlpha();
							
									ImGui::SetNextItemWidth(100.0f); // Adjust this value to change the color picker's width
									if (ImGui::ColorButton("Color Bar", ImVec4(color[0], color[1], color[2], color[3]), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoBorder, ImVec2(300, 10))) {
										// Open the color picker popup when the color bar is clicked
										ImGui::OpenPopup("Color Picker Popup");
									}
							
									// Create the popup for the color picker
									if (ImGui::BeginPopup("Color Picker Popup")) {
										ImGui::Text("Select a color:");
										// Set the width of the color picker
										ImGui::SetNextItemWidth(250.0f); // Adjust this value to change the color picker's width
							
										// Display the color picker inside the popup
										if (ImGui::ColorPicker4("##picker", color, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_PickerHueBar))
										{
							
							
											material.SetColor(glm::vec4(color[0], color[1], color[2], color[3]));
							
							
											//std::cout << "================================\n";
											//std::cout << material.GetMaterialRed() << '\t'
											//	<< material.GetMaterialGreen() << '\t'
											//	<< material.GetMaterialBlue() << '\t'
											//	<< material.GetMaterialAlpha() << '\n';
											//std::cout << "================================\n";
										}
							
							
										ImGui::EndPopup();
									}
							
							
									ImGui::Text("Final Alpha"); ImGui::SameLine(WidthIndentation); ImGui::PushItemWidth(250);
							
							
									float& FinalAlphaVal = material.GetFinalAlphaRef();
							
									// Create a drag float that increments by 0.1 within a range of 0 to 10
									if (ImGui::SliderFloat("##FinalAlpha1", &FinalAlphaVal, 0.0f, 1.0f, "%.3f"))
									{
										material.SetFinalAlpha(FinalAlphaVal);
									}
							
									/*
										Metallic
									*/
							
									ImGui::Button("##MatButton2 ", ImVec2(15, 15)); // Create a visual box
									ImGui::SameLine();
							
									//Put a Add texture here
							
									ImGui::Text("Metallic"); ImGui::SameLine(WidthIndentation); ImGui::PushItemWidth(250);
							
							
									float& MetallicMatValue = material.GetMetallicRef();
							
									// Create a drag float that increments by 0.1 within a range of 0 to 10
									if (ImGui::SliderFloat("##MetallicMat1", &MetallicMatValue, 0.0f, 1.0f, "%.3f"))
									{
										material.SetMetallic(MetallicMatValue);
							
									}
									// put the value here. that can set one.
							
									float& SmoothnessValue = material.GetSmoothnessRef();
							
									ImGui::Indent(20); ImGui::Text("Smoothness"); ImGui::SameLine(WidthIndentation);
									if (ImGui::SliderFloat("##SmoothnessMat1", &SmoothnessValue, 0.0f, 1.0f, "%.3f"))
									{
										material.SetSmoothness(SmoothnessValue);
									}
							
									const char* imG_MaterialAlpha[] = { "Metallic Alpha", "Albedo Alpha" };
									static int MatAlphacurrent_idx = 0; // Index for the selected item
									//ImGui::Set
									ImGui::SetNextItemWidth(200.0f);
							
									ImGui::Indent(20); ImGui::Text("Source");  ImGui::SameLine(WidthIndentation);
									if (ImGui::Combo("##MatCombo2", &MatAlphacurrent_idx, imG_MaterialAlpha, IM_ARRAYSIZE(imG_MaterialAlpha)))
									{
							
										material.SetAlpha(static_cast<float>(MatAlphacurrent_idx));
									}
							
							
							
							
									ImGui::Unindent(40);
							
							
									ImGui::Button("##MatButton4 ", ImVec2(15, 15)); ImGui::SameLine();  ImGui::Text("Normal Map");   	 // Create a visual box
							
							





							
									ImGui::NewLine();	ImGui::NewLine();	ImGui::NewLine();	ImGui::NewLine();	ImGui::NewLine();	ImGui::NewLine();
							
							
									ImGui::Indent(300);
							
									if (ImGui::Button("Apply"))
									{
										if (graphicsComponent.material.GetMaterialNameMat().empty())
										{
											std::string loadSaveLocation2 = FILEPATH_ASSET_MATERIAL + "\\" + g_Coordinator.GetComponent<MetadataComponent>(g_SelectedEntity).GetName() + ".mat";
							
											std::cout << loadSaveLocation2 << '\n';
							
											material.SaveMaterialDescriptor(loadSaveLocation2);
											material.LoadMaterialDescriptor(loadSaveLocation2);
							
										}
										else
										{
											std::cout << graphicsComponent.material.GetMaterialNameMat() << '\n';
											//Save
											std::cout << "saved at location: " << loadSaveLocationExt << '\n';
											material.SaveMaterialDescriptor(loadSaveLocationExt);
											material.LoadMaterialDescriptor(loadSaveLocationExt);
										}
										// g_AssetManager.ReloadTextures();
									}
							
									ImGui::SameLine();
									if (ImGui::Button("Revert To Default"))
									{
										// Go back to default settings
										MaterialDescriptor desc;
							
										std::string file = FILEPATH_ASSETS + "\\Material\\GameObject.mat";

//										std::cout << "here1123" << file << '\n';
										// Save default settings to the descriptor file
										if (desc.SaveMaterialDescriptor(file) ){
											// Load the reverted settings to refresh the inspector
											if (material.LoadMaterialDescriptor(file) ){
												std::cout << "Reverted to default settings." << std::endl;
											}
											else {
												std::cerr << "Failed to load the reverted descriptor file." << std::endl;
											}
										}
										else {
											std::cerr << "Failed to save the reverted descriptor file." << std::endl;
										}
									}
							
							
									ImGui::Unindent(300);
							
							
									ImGui::Separator();
							
									static glm::vec3 lightPos = glm::vec3(-18.5f, -12.6f, 31.6f);
									static bool showLightControls = false;
							
							
									glEnable(GL_MULTISAMPLE);
							
							
							
									static GLuint fbo = 0, textureColorbuffer = 0, rbo = 0; // Declare outside to persist across frames
									static bool isFramebufferGenerated = false; // Flag to prevent generating framebuffer every frame
							
							
									if (!isFramebufferGenerated) {
										// Generate framebuffer only once
										g_Coordinator.GetSystem<GraphicsSystem>()->generateNewFrameBuffer(fbo, textureColorbuffer, rbo, 512, 288);
										// std::cout << '\t' << fbo << '\t' << textureColorbuffer << '\t' << rbo << '\n';
										isFramebufferGenerated = true;
									}
							
									// Set clear color
									glClearColor(46.f / 255.f, 46.f / 255.f, 46.f / 255.f, 1.0f);  // Set the color to clear the framebuffer to
							
							
							
							
							
									/*
										SAVE THE DAMNED VIEWPORT FIRST
									*/
							
									GLint viewport[4];
									glGetIntegerv(GL_VIEWPORT, viewport);
							
							
							
									// Bind the custom framebuffer (where you want to render)
									glBindFramebuffer(GL_FRAMEBUFFER, fbo);
									glViewport(0, 0, 512, 288);  // Set the custom viewport size for this framebuffer
							
							
							
									// Clear the framebuffer
									glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear the color and depth buffers
							
									// Now render your objects
					// 		
								 //   g_Coordinator.GetSystem<GraphicsSystem>()->DrawMaterialSphere(); // Your rendering code
							
							
									g_AssetManager.GetShader("Material").Use();
							
									glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);  // Position of the camera
									glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);     // Point the camera is looking at (center of the sphere)
									glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);         // Up direction
							
									glm::mat4 view = glm::lookAt(cameraPos, target, up);
							
									float aspectRatio = 512.0f / 288.0f;
									glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
							
							
							
									g_AssetManager.GetShader("Material").SetUniform("vertexTransform", glm::mat4(1.0f)); // Identity matrix for no transformation
									g_AssetManager.GetShader("Material").SetUniform("view", view);
									g_AssetManager.GetShader("Material").SetUniform("projection", projection);
									g_AssetManager.GetShader("Material").SetUniform("inputColor", glm::vec4(color[0], color[1], color[2], color[3]));
									g_AssetManager.GetShader("Material").SetUniform("inputLight", lightPos);
							
									g_AssetManager.GetShader("Material").SetUniform("viewPos", cameraPos);
									g_AssetManager.GetShader("Material").SetUniform("metallic", MetallicMatValue);
									g_AssetManager.GetShader("Material").SetUniform("smoothness", SmoothnessValue);
									//				g_AssetManager.GetShader("Material").SetUniform("shininess", shininessValue);  // Shininess value
											// Check if a texture is set, and bind it
							
							
									if (graphicsComponent.material.GetDiffuseID() >= 0) { // Assuming textureID is -1 if no texture
										glActiveTexture(GL_TEXTURE0);
										glBindTexture(GL_TEXTURE_2D, graphicsComponent.material.GetDiffuseID());
										g_AssetManager.GetShader("Material").SetUniform("albedoTexture", 0);
										g_AssetManager.GetShader("Material").SetUniform("useTexture", true);
									}
									else {
										g_AssetManager.GetShader("Material").SetUniform("useTexture", false);
									}
							
									g_AssetManager.GetShader("Material").SetUniform("albedoTexture", 0);
							
							
							
									Model* sphereModel = g_ResourceManager.getModel("sphere");
							
									if (sphereModel)
										sphereModel->DrawMaterial(g_AssetManager.GetShader("Material"));
									else
										std::cerr << "cant find sphere model\n";
							
							
							
							
									g_AssetManager.GetShader("Material").UnUse();
							
							
									// After rendering to the custom framebuffer, unbind it to render to the default framebuffer
									glBindFramebuffer(GL_FRAMEBUFFER, 0);  // Unbind the custom framebuffer (restore to default framebuffer)
							
									// Reset the viewport for the default framebuffer (main window size)
									glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
							
									//glGetIntegerv(GL_VIEWPORT, viewport);
									//std::cout << "Viewport: " << viewport[0] << ", " << viewport[1] << ", "
									//	<< viewport[2] << ", " << viewport[3] << std::endl;
									//
									//glGetIntegerv(GL_FRAMEBUFFER_BINDING, &framebuffer);
									//std::cout << "Framebuffer: " << framebuffer << std::endl;
							
									// Display the custom framebuffer texture (this will display in the ImGui window)
									ImGui::Image((void*)(intptr_t)textureColorbuffer, ImVec2(512, 288));  // Display texture in the ImGui window
							
							
									ImGui::Separator();
									ImGui::Text("Adjust Light Position");
							
									if (ImGui::Button("Open Light Controls")) {
										ImGui::OpenPopup("LightControlPopup");
									}
							
									// Popup modal window
									if (ImGui::BeginPopup("LightControlPopup")) {
										ImGui::Text("Adjust Light Position");
										ImGui::DragFloat("X", &lightPos.x, 0.1f, -10000.0f, 10000.0f, "X: %.1f");
										ImGui::DragFloat("Y", &lightPos.y, 0.1f, -10000.0f, 10000.0f, "Y: %.1f");
										ImGui::DragFloat("Z", &lightPos.z, 0.1f, -10000.0f, 10000.0f, "Z: %.1f");
							
										// Close button inside the popup
										if (ImGui::Button("Close")) {
											ImGui::CloseCurrentPopup();
										}
										ImGui::EndPopup();
									}
							
							
								}
							
								
							
							
							
							
								//
								//ImGui::Text("Albedo");
								//ImGui::SameLine();
								//
								//
								//float color[4] = {
								//	material.GetColor().r,
								//	material.GetColor().g,
								//	material.GetColor().b,
								//	material.GetColor().a
								//};
								//
								//ImGui::SetNextItemWidth(100.0f);
								//if (ImGui::ColorButton("Color Bar", ImVec4(color[0], color[1], color[2], color[3]),
								//	ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoBorder, ImVec2(300, 10))) {
								//	ImGui::OpenPopup("Color Picker Popup");
								//}
								//
								//if (ImGui::BeginPopup("Color Picker Popup")) {
								//	ImGui::Text("Select a color:");
								//	ImGui::SetNextItemWidth(250.0f);
								//
								//	if (ImGui::ColorPicker4("##picker1", color, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_PickerHueBar)) {
								//
								//
								//		material.SetRed(color[0]);
								//		material.SetGreen(color[1]);
								//		material.SetBlue(color[2]);
								//		material.SetAlpha(color[3]);
								//
								//
								//
								//
								//	}
								//
								//
								//	ImGui::EndPopup();
								//}
							
								//								ImGui::Text("Shininess %.2f", );
							
							
//								ImGui::Text("%s", graphicsComponent.GetDiffuseName().c_str());
							
							
							}
							
						}


						else if (className == "LightComponent") 
						{
							if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_None))
							{
								auto& lightComponent = g_Coordinator.GetComponent<LightComponent>(g_SelectedEntity);
								float lightIntensity = lightComponent.getIntensity();
								ImGui::Text("Intensity");
								ImGui::SameLine();
								ImGui::PushItemWidth(125.0f);
								ImGui::PushID("Intensity");

								if (ImGui::DragFloat("##Intensity", &lightIntensity, 0.1f))
								{
									lightComponent.setIntensity(lightIntensity);
								}

								ImGui::PopID();
								ImGui::PopItemWidth();

								glm::vec3 lightColor = lightComponent.getColor();	
								ImGui::Text("Color");
								ImGui::SameLine();
								ImGui::PushItemWidth(125.0f);
								ImGui::PushID("Color");

								if (ImGui::ColorEdit3("##Color", &lightColor.x))
								{
									lightComponent.setColor(lightColor);
								}

								ImGui::PopID();
								ImGui::PopItemWidth();

								// set shadow 
								bool shadow = lightComponent.getShadow();
								ImGui::Text("Shadow");
								ImGui::SameLine();
								ImGui::Checkbox("##Shadow", &shadow);
								lightComponent.setShadow(shadow);
								// same line change direction
								ImGui::SameLine();
								glm::vec3 direction = lightComponent.getDirection();
								ImGui::Text("Direction");
								ImGui::SameLine();
								ImGui::PushItemWidth(125.0f);
								ImGui::PushID("Direction");

								if (ImGui::DragFloat3("##Direction", &direction.x, 0.1f))
								{
									lightComponent.setDirection(direction);
								}

								ImGui::PopID();
								ImGui::PopItemWidth();

								float range = lightComponent.getRange();
								ImGui::Text("Range");
								ImGui::SameLine();
								ImGui::PushItemWidth(125.0f);
								ImGui::PushID("Range");

								if (ImGui::DragFloat("##Range", &range, 0.1f))
								{
									lightComponent.setRange(range);
								}

								ImGui::PopID();
								ImGui::PopItemWidth();


							}

						}
						//UI Component editor
						else if (className == "UIComponent") {
							if (ImGui::CollapsingHeader("UI", ImGuiTreeNodeFlags_None))
							{
								auto& uiComponent = g_Coordinator.GetComponent<UIComponent>(g_SelectedEntity);
								
								// set texture ID 
								std::string textureName = uiComponent.get_texturename();
								ImGui::Text("Texture :");
								ImGui::SameLine();
								ImGui::PushItemWidth(125.0f);

								ImGui::Text("%s", textureName.c_str());
								
								ImGui::SameLine();
								ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);

								ImGui::PushID("Texture");

								if (ImGui::Button("Change Texture"))
								{
									ImGuiFileDialog::Instance()->OpenDialog("ChangeTexture", "Choose File", ".png,.dds", "../BoofWoof/Assets");
								}

								if (ImGuiFileDialog::Instance()->Display("ChangeTexture"))
								{
									if (ImGuiFileDialog::Instance()->IsOk())
									{
										// User selected a file
										std::string selectedFile = ImGuiFileDialog::Instance()->GetCurrentFileName();
										size_t lastDotPos = selectedFile.find_last_of(".");
										if (lastDotPos != std::string::npos)
										{
											selectedFile = selectedFile.substr(0, lastDotPos);
										}
										uiComponent.set_texturename(selectedFile);

									}
									ImGuiFileDialog::Instance()->Close();
								}

								ImGui::PopID();
								ImGui::PopItemWidth();

								
								//set position
								glm::vec2 position = uiComponent.get_position();
								ImGui::Text("Position");
								ImGui::SameLine();
								ImGui::PushItemWidth(125.0f);
								ImGui::PushID("Position");

								if (ImGui::DragFloat2("##Position", &position.x, 0.1f))
								{
									uiComponent.set_position(position);
								}

								ImGui::PopID();
								ImGui::PopItemWidth();

								//set scale
								glm::vec2 scale = uiComponent.get_scale();
								ImGui::Text("Scale");
								ImGui::SameLine();
								ImGui::PushItemWidth(125.0f);
								ImGui::PushID("Scale");

								if (ImGui::DragFloat2("##Scale", &scale.x, 0.1f))
								{
									uiComponent.set_scale(scale);
								}

								ImGui::PopID();
								ImGui::PopItemWidth();

								// display layer number
								ImGui::Text("Layer: %f" , uiComponent.get_UI_layer());
								ImGui::SameLine();
							
								//move forward or backward button
								if (ImGui::Button("Move Forward"))
								{
									uiComponent.set_UI_layer(uiComponent.get_UI_layer() - 0.01f);
								}
								ImGui::SameLine();
								if (ImGui::Button("Move Backward"))
								{
									uiComponent.set_UI_layer(uiComponent.get_UI_layer() + 0.01f);
								}

								// check box for selectablity
								bool selectable = uiComponent.get_selectable();
								ImGui::Checkbox("Selectable", &selectable);
								uiComponent.set_selectable(selectable);

								// opacity
								float opacity = uiComponent.get_opacity();
								ImGui::Text("Opacity");
								ImGui::SameLine();
								ImGui::PushItemWidth(125.0f);
								ImGui::PushID("Opacity");

								if (ImGui::DragFloat("##Opacity", &opacity, 0.1f))
								{
									uiComponent.set_opacity(opacity);
								}

								ImGui::PopID();
								ImGui::PopItemWidth();

								// animated
								bool animate = uiComponent.get_animate();
								ImGui::Checkbox("Animated", &animate);
								uiComponent.set_animate(animate);

								if (animate) {
									// Variables to store the rows and cols values
									int rows = uiComponent.get_rows();
									int cols = uiComponent.get_cols();

									// Textbox for inputting rows
									ImGui::InputInt("Rows", &rows);
									if (rows < 1) rows = 1; // Ensure rows is at least 1

									// Textbox for inputting cols
									ImGui::InputInt("Columns", &cols);
									if (cols < 1) cols = 1; // Ensure cols is at least 1

									// Update rows and cols in the UIComponent
									uiComponent.set_rows(rows);
									uiComponent.set_cols(cols);

									// Modify current row and column
									int currRow = uiComponent.get_curr_row();
									int currCol = uiComponent.get_curr_col();

									// Input fields for changing the current row/column
									if (ImGui::InputInt("Current Row", &currRow)) {
										// Clamp row value to stay within valid range
										if (currRow < 0) currRow = 0;
										if (currRow >= rows) currRow = rows - 1;
										uiComponent.set_curr_row(currRow);
									}

									if (ImGui::InputInt("Current Column", &currCol)) {
										// Clamp column value to stay within valid range
										if (currCol < 0) currCol = 0;
										if (currCol >= cols) currCol = cols - 1;
										uiComponent.set_curr_col(currCol);
									}

									// Add controls for frame interval
									float frameInterval = uiComponent.get_frame_interval();
									ImGui::InputFloat("Frame Interval", &frameInterval);
									if (frameInterval < 0.0f) frameInterval = 0.0f; // Ensure positive value
									uiComponent.set_frame_interval(frameInterval);

									// Checkbox for "Stay on Row"
									bool stayOnRow = uiComponent.get_stay_on_row();
									if (ImGui::Checkbox("Stay on Row", &stayOnRow)) {
										uiComponent.set_stay_on_row(stayOnRow);
									}

									// Play/Pause buttons
									bool isPlaying = uiComponent.get_playing(); // Get animation state
									if (ImGui::Button(isPlaying ? "Pause" : "Play")) {
										uiComponent.set_playing(!isPlaying); // Toggle play/pause state
									}
								}
							}
						}
						//Pathfinding Component editor
						else if (className == "PathfindingComponent") 
						{
							if (ImGui::CollapsingHeader("Pathfinding", ImGuiTreeNodeFlags_None)) {
								auto& pathfindingComponent = g_Coordinator.GetComponent<PathfindingComponent>(g_SelectedEntity);

								// Start Node
								Entity startNode = pathfindingComponent.GetStartNode();
								ImGui::Text("Start Node");
								ImGui::SameLine();
								ImGui::PushItemWidth(150.0f);
								ImGui::PushID("StartNode");

								if (ImGui::DragInt("##StartNode", reinterpret_cast<int*>(&startNode), 1.0f, 0, MAX_ENTITIES)) {
									pathfindingComponent.SetStartNode(startNode);
									g_Coordinator.GetSystem<PathfindingSystem>()->ResetPathfinding();
								}

								ImGui::PopID();
								ImGui::PopItemWidth();

								// Goal Node
								Entity goalNode = pathfindingComponent.GetGoalNode();
								ImGui::Text("Goal Node");
								ImGui::SameLine();
								ImGui::PushItemWidth(150.0f);
								ImGui::PushID("GoalNode");

								if (ImGui::DragInt("##GoalNode", reinterpret_cast<int*>(&goalNode), 1.0f, 0, MAX_ENTITIES)) {
									pathfindingComponent.SetGoalNode(goalNode);
									g_Coordinator.GetSystem<PathfindingSystem>()->ResetPathfinding();
								}

								ImGui::PopID();
								ImGui::PopItemWidth();

								// Current Status
								PathfindingStatus status = pathfindingComponent.GetStatus();
								ImGui::Text("Status: %s",
									status == PathfindingStatus::IDLE ? "Idle" :
									status == PathfindingStatus::PROCESSING ? "Processing" :
									status == PathfindingStatus::COMPLETE ? "Complete" : "Failed");
							}
						}
						// Node Component editor
						// Node Component editor
						else if (className == "NodeComponent") 
						{
							if (ImGui::CollapsingHeader("Node Component", ImGuiTreeNodeFlags_None)) {
								auto& nodeComponent = g_Coordinator.GetComponent<NodeComponent>(g_SelectedEntity);
								glm::vec3 nodePosition = nodeComponent.GetPosition();
								bool isWalkable = nodeComponent.IsWalkable();

								bool hasTransform = g_Coordinator.HaveComponent<TransformComponent>(g_SelectedEntity);
								bool hasHierarchy = g_Coordinator.HaveComponent<HierarchyComponent>(g_SelectedEntity);

								glm::vec3 previousPosition = nodePosition; // Store old position for comparison

								if (hasTransform) {
									auto& transformComp = g_Coordinator.GetComponent<TransformComponent>(g_SelectedEntity);

									if (hasHierarchy) {
										// Use World Transform if HierarchyComponent exists
										auto transformSystem = g_Coordinator.GetSystem<TransformSystem>();
										glm::mat4 worldMatrix = transformSystem->GetWorldMatrix(g_SelectedEntity);

										glm::vec3 worldPosition, worldRotation, worldScale;
										if (DecomposeTransform(worldMatrix, worldPosition, worldRotation, worldScale)) {
											nodePosition = worldPosition; // Override position
										}
										ImGui::Text("Node Position (World)");
									}
									else {
										// Use local transform if no hierarchy
										nodePosition = transformComp.GetPosition();
										ImGui::Text("Node Position (Local)");
									}
								}
								else {
									ImGui::Text("Node Position (Manual)");
								}

								ImGui::SameLine();
								ImGui::PushItemWidth(150.0f);
								ImGui::PushID("NodePosition");

								bool positionChanged = false; // Flag for detecting change

								// If the entity has a transform, display the position but disable editing
								if (hasTransform) {
									ImGui::InputFloat3("##NodePosition", &nodePosition.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
								}
								else {
									// Allow manual setting if no TransformComponent
									if (ImGui::DragFloat3("##NodePosition", &nodePosition.x, 0.1f)) {
										nodeComponent.SetPosition(nodePosition);
										positionChanged = true; // Mark change
									}
								}

								ImGui::PopID();
								ImGui::PopItemWidth();

								// Walkable Checkbox
								bool walkable = nodeComponent.IsWalkable();
								ImGui::Text("Walkable");
								ImGui::SameLine();
								if (ImGui::Checkbox("##Walkable", &walkable)) {
									nodeComponent.SetWalkable(walkable);
									positionChanged = true; // Rebuild graph if walkability changes
								}

								// **Rebuild Graph if Position Changes**
								if (positionChanged && nodePosition != previousPosition) {
									auto pathfindingSystem = g_Coordinator.GetSystem<PathfindingSystem>();
									if (pathfindingSystem) {
										pathfindingSystem->BuildGraph(); // Rebuild graph
										std::cout << "[DEBUG] Pathfinding graph rebuilt due to node position change." << std::endl;
									}
								}
							}
						}



						// Edge Component editor
						else if (className == "EdgeComponent") 
						{
							if (ImGui::CollapsingHeader("Edge", ImGuiTreeNodeFlags_None)) {
								auto& edgeComponent = g_Coordinator.GetComponent<EdgeComponent>(g_SelectedEntity);

								// Start Node
								Entity startNode = edgeComponent.GetStartNode();
								ImGui::Text("Start Node");
								ImGui::SameLine();
								ImGui::PushItemWidth(150.0f);
								ImGui::PushID("StartNode");

								if (ImGui::DragInt("##StartNode", reinterpret_cast<int*>(&startNode), 1.0f, 0, MAX_ENTITIES)) {
									edgeComponent.SetStartNode(startNode);
									g_Coordinator.GetSystem<PathfindingSystem>()->ResetPathfinding();
								}

								ImGui::PopID();
								ImGui::PopItemWidth();

								// End Node
								Entity endNode = edgeComponent.GetEndNode();
								ImGui::Text("End Node");
								ImGui::SameLine();
								ImGui::PushItemWidth(150.0f);
								ImGui::PushID("EndNode");

								if (ImGui::DragInt("##EndNode", reinterpret_cast<int*>(&endNode), 1.0f, 0, MAX_ENTITIES)) {
									edgeComponent.SetEndNode(endNode);
									g_Coordinator.GetSystem<PathfindingSystem>()->ResetPathfinding();
								}

								ImGui::PopID();
								ImGui::PopItemWidth();

								// Edge Cost
								float cost = edgeComponent.GetCost();
								ImGui::Text("Cost");
								ImGui::SameLine();
								ImGui::PushItemWidth(150.0f);
								ImGui::PushID("EdgeCost");

								if (ImGui::DragFloat("##EdgeCost", &cost, 0.1f, 0.0f)) {
									edgeComponent.SetCost(cost);
									g_Coordinator.GetSystem<PathfindingSystem>()->ResetPathfinding();
								}

								ImGui::PopID();
								ImGui::PopItemWidth();
							}
						}
						else if (className == "FontComponent") {
							if (ImGui::CollapsingHeader("Font", ImGuiTreeNodeFlags_None))
							{
								// Grab the FontComponent from the selected entity
								auto& fontComponent = g_Coordinator.GetComponent<FontComponent>(g_SelectedEntity);
								Entity entity = g_SelectedEntity; // for lambda captures

								// family
								ImGui::Text("Family");
								ImGui::SameLine();
								ImGui::PushItemWidth(125.0f);
								ImGui::PushID("Family");

								// Get the list of font names from the resource manager
								std::vector<std::string> fontNames = g_ResourceManager.GetFontNames();

								// Get the current family and find the index of it in fontNames
								std::string currentFamily = fontComponent.get_family();
								int currentIndex = -1;

								// Find the index of the current family in the list
								for (int i = 0; i < fontNames.size(); ++i) {
									if (fontNames[i] == currentFamily) {
										currentIndex = i;
										break;
									}
								}

								// If not found, set to 0 (default to first font)
								if (currentIndex == -1 && !fontNames.empty()) {
									currentIndex = 0;
								}

								// Convert the font names to a C-array of const char*
								std::vector<const char*> fontNamesCStr;
								for (const auto& name : fontNames) {
									fontNamesCStr.push_back(name.c_str());
								}

								// Display the dropdown (combo box)
								if (ImGui::Combo("##Family", &currentIndex, fontNamesCStr.data(), (int)(fontNamesCStr.size()))) {
									// Update the family when a new font is selected
									if (currentIndex >= 0 && currentIndex < fontNames.size()) {
										fontComponent.set_family(fontNames[currentIndex]);
									}
								}

								ImGui::PopID();
								ImGui::PopItemWidth();



								// position
								glm::vec2 position = fontComponent.get_pos();
								ImGui::Text("Position");
								ImGui::SameLine();
								ImGui::PushItemWidth(125.0f);
								ImGui::PushID("Position");
								if (ImGui::DragFloat2("##Position", &position.x, 0.1f))
								{
									fontComponent.set_pos(position);
								}

								ImGui::PopID();
								ImGui::PopItemWidth();
								// scale_x and scale_y
								glm::vec2 scale = fontComponent.get_scale();
								ImGui::Text("Scale");
								ImGui::SameLine();
								ImGui::PushItemWidth(125.0f);
								ImGui::PushID("Scale");
								if (ImGui::DragFloat2("##Scale", &scale.x, 0.1f))
								{
									fontComponent.set_scale(scale);
								}
								ImGui::PopID();
								ImGui::PopItemWidth();
								
								// text
								char text[128]{};
								strcpy_s(text, fontComponent.get_text().c_str());
								ImGui::Text("Text");
								ImGui::SameLine();
								ImGui::PushItemWidth(125.0f);
								ImGui::PushID("Text");
								if (ImGui::InputText("##Text", text, 128))
								{
									fontComponent.set_text(text);
								}
								ImGui::PopID();
								ImGui::PopItemWidth();
								// color
								glm::vec3 color = fontComponent.get_color();
								ImGui::Text("Color");
								ImGui::SameLine();
								ImGui::PushItemWidth(125.0f);
								ImGui::PushID("Color");
								if (ImGui::ColorEdit3("##Color", &color.x))
								{
									fontComponent.set_color(color);
								}
								ImGui::PopID();
								ImGui::PopItemWidth();

								// layer
								float layer = fontComponent.get_layer();
								ImGui::Text("Layer");
								ImGui::SameLine();
								ImGui::PushItemWidth(125.0f);
								ImGui::PushID("Layer: ");
								// display layer number
								ImGui::Text("Layer: %f", layer);
								ImGui::SameLine();
								//move forward or backward button
								if (ImGui::Button("Move Forward"))
								{
									fontComponent.set_layer(fontComponent.get_layer() - 0.01f);
								}
								ImGui::SameLine();
								if (ImGui::Button("Move Backward"))
								{
									fontComponent.set_layer(fontComponent.get_layer() + 0.01f);
								}
								ImGui::PopID();
								ImGui::PopItemWidth();

							}
						}
					}
				}
			}
			else
			{
				ImGui::Text("No entity selected or invalid entity ID.");
			}
		}
		else
		{
			// Convert paths to strings for comparison
			std::string selectedFilePath = m_SelectedFile.string();
			std::replace(selectedFilePath.begin(), selectedFilePath.end(), '/', '\\');
			//std::cout << selectedFilePath << std::endl;

			if (selectedFilePath == FILEPATH_ASSET_TEXTURES) { ImGui::Text("Textures (Default Asset)\n\n\nContains texture files.\n\nAccepted file extensions are: .png"); }
			else if (selectedFilePath == FILEPATH_ASSET_ART) { ImGui::Text("Art (Default Asset)\n\n\nContains all art-related files."); }
			else if (selectedFilePath == FILEPATH_ASSET_SCENES) { ImGui::Text("Scenes (Default Asset)\n\n\nContains audio files.\n\nAccepted file extensions are: .json"); }
			else if (selectedFilePath == FILEPATH_ASSET_OBJECTS) { ImGui::Text("Objects (Default Asset)\n\n\nContains object files.\n\nAccepted file extensions are: .obj, .mtl, .png"); }
			else if (selectedFilePath == FILEPATH_ASSET_SHADERS) { ImGui::Text("Shaders (Default Asset)\n\n\nContains shader files.\n\nAccepted file extensions are: .frag, .vert"); }
			else if (selectedFilePath == FILEPATH_ASSET_FONTS) { ImGui::Text("Fonts (Default Asset)\n\n\nContains font files.\n\nAccepted file extensions are: .ttf"); }
			else if (selectedFilePath == FILEPATH_ASSET_AUDIO) { ImGui::Text("Audio (Default Asset)\n\n\nContains audio files.\n\nAccepted file extensions are: "); }
			else if (selectedFilePath == FILEPATH_ASSET_TRASHBIN) { ImGui::Text("Trash Bin (Default Asset)\n\n\nAssets placed in the wrong folder will be moved here."); }

			else if (selectedFilePath.find(".png") != std::string::npos)
			{

				std::string selectedFileName = m_SelectedFile.stem().string();
				GLuint pictureIcon = g_ResourceManager.GetTextureDDS(selectedFileName);
				GLuint pictureWidth = g_ResourceManager.GetTextureDDSWidth(selectedFileName);
				GLuint pictureHeight = g_ResourceManager.GetTextureDDSHeight(selectedFileName);

				// 256 x 256 is our max picture size to show.
				GLuint maxWidth = 32;


				//std::cout << selectedFileName << '\t' << g_ResourceManager.GetTextureDDSWidth(selectedFileName) << '\t' << std::to_string(g_ResourceManager.GetTextureDDSHeight(selectedFileName))  << '\t' << '\n';

				std::string inputTextToDisplay = selectedFileName + "(Texture 2D) Import Settings";

				// Fix such that the picture does not appear too big.
				GLuint newWidth = pictureWidth;
				GLuint newHeight = pictureHeight;



				float aspectRatio = static_cast<float>(pictureWidth) / static_cast<float>(pictureHeight);

				if (pictureWidth > maxWidth) {
					newWidth = maxWidth;
					newHeight = static_cast<int>(newWidth / aspectRatio);
				}

				ImGui::Image((ImTextureID)(uintptr_t)(pictureIcon != -1 ? pictureIcon : g_ResourceManager.GetTextureDDS("BlackScreen")), ImVec2(static_cast<float>(newWidth), static_cast<float>(newHeight)));
				ImGui::SameLine();
				ImGui::Text(inputTextToDisplay.c_str());





				ImGui::Separator();






				if (ImGui::BeginTabBar("InspectorTabs"))
				{
					// First Tab: Default
					if (ImGui::BeginTabItem("Texture Descriptor"))
					{

						/*
	TEXTURE TYPE
*/

						ImGui::Text("Texture Type ");
						ImGui::SameLine(225);

						const char* imG_TexType[] = { "Default", "Normal Map", "Editor GUI and Legacy GUI", "Sprite (2D and UI)", "Cursor", "Cookie", "Lightmap", "Directional Lightmap", "Shadowmask", "Single Channel" };
						int& TexType_current_idx = textureInfo.textureType; // Index for the selected item
						//ImGui::Set
						ImGui::SetNextItemWidth(200.0f);
						if (ImGui::Combo("##Combo1", &TexType_current_idx, imG_TexType, IM_ARRAYSIZE(imG_TexType))) {
							textureInfo.textureType = TexType_current_idx; // Update the textureInfo value
						}

						/*
							TEXTURE SHAPE
						*/


						ImGui::Text("Texture Shape ");
						ImGui::SameLine(225);

						const char* imG_TexShape[] = { "2D","Cube","2D Array", "3D" };
						int& TexShape_current_idx = textureInfo.textureShape;

						ImGui::SetNextItemWidth(200.0f);
						if (ImGui::Combo("##Combo2", &TexShape_current_idx, imG_TexShape, IM_ARRAYSIZE(imG_TexShape))) {
							textureInfo.textureShape = TexShape_current_idx;
						};



						/*
							sRGB Color Texture
						*/


						//static bool sRGBCheck = false;



				//		ImGui::Text("Max Size");
				//		ImGui::SameLine(225);
				//
				//		const char* imG_MaxSize[] = { "32","64","128","256", "512" ,"1024", "2048", "4096", "8192", "16384" };
				//		int& MaxSize_current_idx = textureInfo.maxSize; // Index for the selected item
				//		ImGui::SetNextItemWidth(150.0f);
				//		if (ImGui::Combo("##Combo6", &MaxSize_current_idx, imG_MaxSize, IM_ARRAYSIZE(imG_MaxSize))) {
				//			textureInfo.maxSize = MaxSize_current_idx;
				//		};
				//
				//
				//
				//		ImGui::Text("Resize Algorithm");
				//		ImGui::SameLine(225);
				//
				//		const char* imG_ResizeAlgo[] = { "Mitchell", "Bilinear" };
				//		int& ResizeAlg_current_idx = textureInfo.resizeAlgorithm; // Index for the selected item
				//		ImGui::SetNextItemWidth(150.0f);
				//		if (ImGui::Combo("##Combo7", &ResizeAlg_current_idx, imG_ResizeAlgo, IM_ARRAYSIZE(imG_ResizeAlgo))) {
				//			textureInfo.resizeAlgorithm = ResizeAlg_current_idx;
				//		};


						ImGui::Text("Format");
						ImGui::SameLine(225);

						const char* imG_OperatingSystemFormat[] = {
							"RGB Uncompressed",
							"ATC_RGB",
							"ATC_RGBA_Explicit",
							"ATC_RGBA_Interpolated",
							"ATI1N",
							"ATI2N",
							"ATI2N_XY",
							"ATI2N_DXT5",
							"BC1",
							"BC2",
							"BC3",
							"BC4",
							"BC4_S",
							"BC5",
							"BC5_S",
							"BC6H",
							"BC7",
							"DXT1",
							"DXT3",
							"DXT5",
							"DXT5_xGBR",
							"DXT5_RxBG",
							"DXT5_RBxG",
							"DXT5_xRBG",
							"DXT5_RGxB",
							"DXT5_xGxR"
						};
						int& OS_current_idx = textureInfo.format; // Index for the selected item
						ImGui::SetNextItemWidth(150.0f);
						if (ImGui::Combo("##Combo8", &OS_current_idx, imG_OperatingSystemFormat, IM_ARRAYSIZE(imG_OperatingSystemFormat))) {
							textureInfo.format = OS_current_idx;
						};




						ImGui::Spacing();
						ImGui::Text("sRGB (Color Texture) ");
						ImGui::SameLine(225);

						ImGui::Checkbox("##Button1 ", &textureInfo.sRGB);




						ImGui::Text("Generate MipMaps");
						ImGui::SameLine(225);

						ImGui::Checkbox("##CBoxGenMip1 ", &textureInfo.generateMipMaps);




						// ImGui::Text("Compression");
						// ImGui::SameLine(225);
						// 
						// const char* imG_CompressionQuality[] = { "None", "Low Quality", "Normal Quality", "High Quality" };
						// int& Compression_current_idx = textureInfo.compression; // Index for the selected item
						// ImGui::SetNextItemWidth(150.0f);
						// if (ImGui::Combo("##Combo9", &Compression_current_idx, imG_CompressionQuality, IM_ARRAYSIZE(imG_CompressionQuality))) {
						// 	textureInfo.compression = Compression_current_idx;
						// };


						//static bool crunchCompress = false;

						//ImGui::Text("Use Crunch Compression");
						//ImGui::SameLine(225);
						//
						//ImGui::Checkbox("##Button10 ", &textureInfo.useCrunchCompression);

						// More controls as needed...
						ImGui::EndTabItem();
					}

					// Second Tab: Another settings tab
					//if (ImGui::BeginTabItem("Advanced"))
					//{
					//	static bool overrite = false;
					//
					//	ImGui::Text("Override For Windows, Mac, Linux");
					//	ImGui::SameLine(225);
					//
					//	ImGui::Checkbox(" ", &overrite);
					//
					//	if (overrite)
					//	{
					//
					//		const char* imG_MaxSize[] = { "32","64","128","256", "512" ,"1024", "2048", "4096", "8192", "16384" };
					//		int& MaxSize_current_idx = textureInfo.maxSize; // Index for the selected item
					//		ImGui::SetNextItemWidth(150.0f);
					//		if (ImGui::Combo("##Combo10", &MaxSize_current_idx, imG_MaxSize, IM_ARRAYSIZE(imG_MaxSize))) {
					//			textureInfo.maxSize = MaxSize_current_idx;
					//		};
					//
					//
					//
					//		ImGui::Text("Resize Algorithm");
					//		ImGui::SameLine(225);
					//
					//		const char* imG_ResizeAlgo[] = { "Mitchell", "Bilinear" };
					//		int& ResizeAlg_current_idx = textureInfo.resizeAlgorithm; // Index for the selected item
					//		ImGui::SetNextItemWidth(150.0f);
					//		if (ImGui::Combo("##Combo11", &ResizeAlg_current_idx, imG_ResizeAlgo, IM_ARRAYSIZE(imG_ResizeAlgo))) {
					//			textureInfo.resizeAlgorithm = ResizeAlg_current_idx;
					//		};
					//
					//
					//		ImGui::Text("Format");
					//		ImGui::SameLine(225);
					//
					//		const char* imG_OperatingSystemFormat[] = { "RGBA Compressed DXT5|BC3" };
					//		int OS_current_idx = 0; // Index for the selected item
					//		ImGui::SetNextItemWidth(150.0f);
					//		if (ImGui::Combo("##Combo12", &OS_current_idx, imG_OperatingSystemFormat, IM_ARRAYSIZE(imG_OperatingSystemFormat))) {
					//			textureInfo.format = OS_current_idx;
					//		};
					//
					//	}
					//
					//
					//	// Additional widgets can go here
					//	ImGui::EndTabItem();
					//}

					ImGui::EndTabBar();  // End the tab bar
				}



				ImGui::NewLine();	ImGui::NewLine();	ImGui::NewLine();	ImGui::NewLine();	ImGui::NewLine();	ImGui::NewLine();


				ImGui::Indent(300);

				if (ImGui::Button("Apply"))
				{
					std::cout << "lmeo\t" << FILEPATH_DESCRIPTORS << '\n';


					//Save
					std::cout << FILEPATH_DESCRIPTOR_TEXTURES + "/" + selectedFileName + ".json";
					textureInfo.SaveTextureDescriptor(FILEPATH_DESCRIPTOR_TEXTURES + "/" + selectedFileName + ".json");
					g_AssetManager.ReloadTextures();
				}

				ImGui::SameLine();
				if (ImGui::Button("Revert To Default"))
				{
					// Go back to default settings
					TextureDescriptor desc;

					std::cout << FILEPATH_DESCRIPTOR_TEXTURES << '\n';

					// Save default settings to the descriptor file
					if (desc.SaveTextureDescriptor(FILEPATH_DESCRIPTOR_TEXTURES + "/" + selectedFileName + ".json")) {
						// Load the reverted settings to refresh the inspector
						if (textureInfo.LoadTextureDescriptor(FILEPATH_DESCRIPTOR_TEXTURES + "/" + selectedFileName + ".json")) {
							std::cout << "Reverted to default settings." << std::endl;
						}
						else {
							std::cerr << "Failed to load the reverted descriptor file." << std::endl;
						}
					}
					else {
						std::cerr << "Failed to save the reverted descriptor file." << std::endl;
					}
				}

				ImGui::Unindent(300);


				maxWidth = 256;

				newWidth = pictureWidth;
				newHeight = pictureHeight;


				if (pictureWidth > maxWidth) {
					newWidth = maxWidth;
					newHeight = static_cast<int>(newWidth / aspectRatio);
				}


				ImGui::Separator();
				ImGui::Text(inputTextToDisplay.c_str());
				if (ImGui::BeginTabBar("InspectorTab2"))
				{
					// First Tab: Default
					if (ImGui::BeginTabItem("Texture Image"))
					{
						ImGui::Image((ImTextureID)(uintptr_t)(pictureIcon != -1 ? pictureIcon : g_ResourceManager.GetTextureDDS("BlackScreen")), ImVec2(static_cast<float>(newWidth), static_cast<float>(newHeight)));

						ImGui::EndTabItem();
					}

					ImGui::EndTabBar();  // End the tab bar
				}





			}
			else if (selectedFilePath.find(".mat") != std::string::npos)
			{


				// do some search for the Shader name:

				static float WidthIndentation = 125.0f;
				static float ComboIdentation = 300.0f;

				/*
					SHADER
				*/

				ImGui::Text("Material");
				ImGui::SameLine(WidthIndentation);

				std::string comboItems;
				for (const auto& item : g_AssetManager.shdrpgmOrder) {
					comboItems += item;
					comboItems += '\0'; // Null-terminate each item
				}

				// Ensure default selection is the first item
				if (materialInfo.shaderIndex < 0 || materialInfo.shaderIndex >= g_AssetManager.shdrpgmOrder.size()) {
					materialInfo.shaderIndex = 5; // Default to the first item
					materialInfo.shaderChosen = g_AssetManager.shdrpgmOrder[5]; // Set the chosen shader
				}

				int& Material_current_idx = materialInfo.shaderIndex; // Index for the selected item

				ImGui::SetNextItemWidth(WidthIndentation); // Set combo box width
				if (ImGui::Combo("##MatCombo1", &Material_current_idx, comboItems.c_str())) {
					// Update materialInfo.shaderIndex with the selected index
					materialInfo.shaderIndex = Material_current_idx;

					// Retrieve the selected string from the original vector
					materialInfo.shaderChosen = g_AssetManager.shdrpgmOrder[materialInfo.shaderIndex];
				}


				/*
					Main Maps
				*/


				//ImGui::BeginChild()ImGui::Text("Main Maps");
				//if (ImGui::TreeNode("Main Maps")) {
				ImGui::Text("Main Maps");


				/*
					Albedo
				*/

				ImGui::Button("##MatButton1 ", ImVec2(15, 15)); // Create a visual box

				if (ImGui::BeginDragDropTarget()) {
					// Check if a payload is available
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_ITEM_TYPE")) {
						// Process the payload data (for example, a file path)
						const char* droppedData = (const char*)payload->Data;
						ImGui::Text("Dropped: %s", droppedData);
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::SameLine();

				//Put a Add texture here

				ImGui::Text("Albedo");
				ImGui::SameLine();

				float color[4];// = { 1.0f, 0.0f, 0.0f, 1.0f }; // Example initial color (red)

				color[0] = materialInfo.albedoColorRed;
				color[1] = materialInfo.albedoColorGreen;
				color[2] = materialInfo.albedoColorBlue;
				color[3] = materialInfo.albedoColorAlpha;

				ImGui::SetNextItemWidth(100.0f); // Adjust this value to change the color picker's width
				if (ImGui::ColorButton("Color Bar", ImVec4(color[0], color[1], color[2], color[3]), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoBorder, ImVec2(300, 10))) {
					// Open the color picker popup when the color bar is clicked
					ImGui::OpenPopup("Color Picker Popup");
				}

				// Create the popup for the color picker
				if (ImGui::BeginPopup("Color Picker Popup")) {
					ImGui::Text("Select a color:");
					// Set the width of the color picker
					ImGui::SetNextItemWidth(250.0f); // Adjust this value to change the color picker's width

					// Display the color picker inside the popup
					if (ImGui::ColorPicker4("##picker", color, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_PickerHueBar))
					{
						materialInfo.albedoColorRed = color[0];
						materialInfo.albedoColorGreen = color[1];
						materialInfo.albedoColorBlue = color[2];
						materialInfo.albedoColorAlpha = color[3];

						std::cout << "================================\n";
						std::cout << materialInfo.albedoColorRed << '\t'
							<< materialInfo.albedoColorGreen << '\t'
							<< materialInfo.albedoColorBlue << '\t'
							<< materialInfo.albedoColorAlpha << '\n';
						std::cout << "================================\n";
					}


					ImGui::EndPopup();
				}


				/*
					Metallic
				*/

				ImGui::Button("##MatButton2 ", ImVec2(15, 15)); // Create a visual box
				ImGui::SameLine();

				//Put a Add texture here

				ImGui::Text("Metallic"); ImGui::SameLine(WidthIndentation); ImGui::PushItemWidth(250);


				float& MetallicMatValue = materialInfo.metallic;

				// Create a drag float that increments by 0.1 within a range of 0 to 10
				if (ImGui::SliderFloat("##MetallicMat1", &MetallicMatValue, 0.0f, 1.0f, "%.3f"))
				{
					materialInfo.metallic = MetallicMatValue;

				}
				// put the value here. that can set one.

				float& SmoothnessValue = materialInfo.smoothness;

				ImGui::Indent(20); ImGui::Text("Smoothness"); ImGui::SameLine(WidthIndentation);
				if (ImGui::SliderFloat("##SmoothnessMat1", &SmoothnessValue, 0.0f, 1.0f, "%.3f"))
				{
					materialInfo.smoothness = SmoothnessValue;
				}

				const char* imG_MaterialAlpha[] = { "Metallic Alpha", "Albedo Alpha" };
				static int MatAlphacurrent_idx = 0; // Index for the selected item
				//ImGui::Set
				ImGui::SetNextItemWidth(200.0f);

				ImGui::Indent(20); ImGui::Text("Source");  ImGui::SameLine(WidthIndentation);
				if (ImGui::Combo("##MatCombo2", &MatAlphacurrent_idx, imG_MaterialAlpha, IM_ARRAYSIZE(imG_MaterialAlpha)))
				{

					materialInfo.materialAlpha = MatAlphacurrent_idx;
				}


				ImGui::Unindent(40);


				ImGui::Button("##MatButton4 ", ImVec2(15, 15)); ImGui::SameLine();  ImGui::Text("Normal Map");   	 // Create a visual box



				ImGui::NewLine();	ImGui::NewLine();	ImGui::NewLine();	ImGui::NewLine();	ImGui::NewLine();	ImGui::NewLine();


				ImGui::Indent(300);

				if (ImGui::Button("Apply"))
				{

					//Save
					std::cout << m_SelectedFile.string();
					materialInfo.SaveMaterialDescriptor(m_SelectedFile.string());
					materialInfo.LoadMaterialDescriptor(m_SelectedFile.string());
					// g_AssetManager.ReloadTextures();
				}

				ImGui::SameLine();
				if (ImGui::Button("Revert To Default"))
				{
					// Go back to default settings
					MaterialDescriptor desc;


					std::cout <<  "WHART: " << m_SelectedFile.string() << '\n';
					// Save default settings to the descriptor file
					if (desc.SaveMaterialDescriptor(m_SelectedFile.string())) {
						// Load the reverted settings to refresh the inspector
						if (materialInfo.LoadMaterialDescriptor(m_SelectedFile.string())) {
							std::cout << "Reverted to default settings." << std::endl;
						}
						else {
							std::cerr << "Failed to load the reverted descriptor file." << std::endl;
						}
					}
					else {
						std::cerr << "Failed to save the reverted descriptor file." << std::endl;
					}
				}

				ImGui::Unindent(300);

				//ImGui::Button("##MatButton5 ", ImVec2(15, 15)); ImGui::SameLine();	ImGui::Text("Height Map"); 	 // Create a visual box
				//
				//ImGui::Button("##MatButton6 ", ImVec2(15, 15)); ImGui::SameLine();	ImGui::Text("Occulsion");  	 // Create a visual box
				//
				//ImGui::Button("##MatButton7 ", ImVec2(15, 15)); ImGui::SameLine();	ImGui::Text("Detail Mask");	 // Create a visual box
				//
				//
				//static bool mat_Emission = false;
				//
				//ImGui::Text("Emission ");
				//ImGui::SameLine(WidthIndentation);
				//
				//ImGui::Checkbox("##MatEmission1 ", &mat_Emission);
				//
				//static float TillingxValue = 0.0f;
				//static float TillingyValue = 0.0f;
				//
				//
				//ImGui::Text("Tilling");
				//ImGui::SameLine(WidthIndentation);
				//
				//// Create a line with "X" label, a draggable float, and text input
				//ImGui::Text("X");	ImGui::SameLine();	ImGui::PushItemWidth(100.0f); // Adjust the width as needed
				//ImGui::DragFloat("##XValue1", &TillingxValue, 0.03f, -FLT_MAX, FLT_MAX, "%.2f");
				//ImGui::PopItemWidth();
				//
				//ImGui::SameLine();
				//// Create a line with "Y" label, a draggable float, and text input
				//ImGui::Text("Y");	ImGui::SameLine();	ImGui::PushItemWidth(100.0f); // Adjust the width as needed
				//ImGui::DragFloat("##YValue1", &TillingyValue, 0.03f, -FLT_MAX, FLT_MAX, "%.2f");
				//ImGui::PopItemWidth();
				//
				//
				//static float OffsetXVal = 0.0f;
				//static float OffsetYVal = 0.0f;
				//
				//ImGui::Text("Offset");
				//ImGui::SameLine(WidthIndentation);
				//
				//// Create a line with "X" label, a draggable float, and text input
				//ImGui::Text("X");	ImGui::SameLine();	ImGui::PushItemWidth(100.0f); // Adjust the width as needed
				//ImGui::DragFloat("##XValue2", &OffsetXVal, 0.03f, -FLT_MAX, FLT_MAX, "%.2f");
				//ImGui::PopItemWidth();
				//
				//ImGui::SameLine();
				//// Create a line with "Y" label, a draggable float, and text input
				//ImGui::Text("Y");	ImGui::SameLine();	ImGui::PushItemWidth(100.0f); // Adjust the width as needed
				//ImGui::DragFloat("##YValue2", &OffsetYVal, 0.03f, -FLT_MAX, FLT_MAX, "%.2f");
				//ImGui::PopItemWidth();
				//
				//ImGui::NewLine();
				//ImGui::Text("Secondary Map");
				//
				//
				//ImGui::Button("##MatButton8 ", ImVec2(15, 15)); // Create a visual box
				//
				//if (ImGui::BeginDragDropTarget()) {
				//	// Check if a payload is available
				//	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_ITEM_TYPE")) {
				//		// Process the payload data (for example, a file path)
				//		const char* droppedData = (const char*)payload->Data;
				//		ImGui::Text("Dropped: %s", droppedData);
				//	}
				//	ImGui::EndDragDropTarget();
				//}
				//
				//ImGui::SameLine();	ImGui::Text("Detail Albedo x2");
				//
				//ImGui::Button("##MatButton9 ", ImVec2(15, 15)); // Create a visual box
				//
				//if (ImGui::BeginDragDropTarget()) {
				//	// Check if a payload is available
				//	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_ITEM_TYPE")) {
				//		// Process the payload data (for example, a file path)
				//		const char* droppedData = (const char*)payload->Data;
				//		ImGui::Text("Dropped: %s", droppedData);
				//	}
				//	ImGui::EndDragDropTarget();
				//}
				//
				//static float nMapVal = 0.0f;
				//
				//ImGui::SameLine();	ImGui::Text("Normal Map"); ImGui::SameLine(WidthIndentation);
				//ImGui::InputFloat("##NormalMapInput1", &nMapVal, 0.0f, 0.0f);
				//// ImGui::Button("##MatButton9 ", ImVec2(15, 15)); // Create a visual box
				//
				//static float SMap_TillingxValue = 0.0f;
				//static float SMap_TillingyValue = 0.0f;
				//
				//
				//ImGui::Text("Tilling");
				//ImGui::SameLine(WidthIndentation);
				//
				//// Create a line with "X" label, a draggable float, and text input
				//ImGui::Text("X");	ImGui::SameLine();	ImGui::PushItemWidth(100.0f); // Adjust the width as needed
				//ImGui::DragFloat("##XValue3", &SMap_TillingxValue, 0.03f, -FLT_MAX, FLT_MAX, "%.2f");
				//ImGui::PopItemWidth();
				//
				//ImGui::SameLine();
				//// Create a line with "Y" label, a draggable float, and text input
				//ImGui::Text("Y");	ImGui::SameLine();	ImGui::PushItemWidth(100.0f); // Adjust the width as needed
				//ImGui::DragFloat("##YValue3", &SMap_TillingyValue, 0.03f, -FLT_MAX, FLT_MAX, "%.2f");
				//ImGui::PopItemWidth();
				//
				//
				//static float S_MapOffsetXVal = 0.0f;
				//static float S_MapOffsetYVal = 0.0f;
				//
				//ImGui::Text("Offset");
				//ImGui::SameLine(WidthIndentation);
				//
				//// Create a line with "X" label, a draggable float, and text input
				//ImGui::Text("X");	ImGui::SameLine();	ImGui::PushItemWidth(100.0f); // Adjust the width as needed
				//ImGui::DragFloat("##XValue4", &S_MapOffsetXVal, 0.03f, -FLT_MAX, FLT_MAX, "%.2f");
				//ImGui::PopItemWidth();
				//
				//ImGui::SameLine();
				//// Create a line with "Y" label, a draggable float, and text input
				//ImGui::Text("Y");	ImGui::SameLine();	ImGui::PushItemWidth(100.0f); // Adjust the width as needed
				//ImGui::DragFloat("##YValue4", &S_MapOffsetYVal, 0.03f, -FLT_MAX, FLT_MAX, "%.2f");
				//ImGui::PopItemWidth();
				//
				//
				//
				//
				//
				//
				//
				//const char* imG_UVSet[] = { "UV0", "UV1" };
				//static int MatUVcurrent_idx = 0; // Index for the selected item
				////ImGui::Set
				//ImGui::SetNextItemWidth(200.0f);
				//
				//ImGui::Text("UV Set");  ImGui::SameLine(WidthIndentation); ImGui::Combo("##MatCombo3", &MatUVcurrent_idx, imG_UVSet, IM_ARRAYSIZE(imG_UVSet));
				//
				//
				//static bool mat_SpecularHighlights = false;
				//
				//ImGui::Text("Specular Highlights ");
				//ImGui::SameLine(WidthIndentation);
				//
				//ImGui::Checkbox("##MatSpecularHigh1 ", &mat_SpecularHighlights);
				//
				//static bool mat_Reflections = false;
				//
				//ImGui::Text("Reflections ");
				//ImGui::SameLine(WidthIndentation);
				//
				//ImGui::Checkbox("##MatReflection1 ", &mat_Reflections);
				//
				//
				//
				//
				//ImGui::Text("Advanced Options");
				//
				//
				//const char* imG_RenderQueue[] = { "From Shader", "Geometry", "AlphaTest", "Transparent" };
				//static int MatR_Queue_current_idx = 0; // Index for the selected item
				////ImGui::Set
				//ImGui::SetNextItemWidth(200.0f);
				//
				//ImGui::Text("UV Set");  ImGui::SameLine(WidthIndentation); ImGui::Combo("##MatCombo4", &MatR_Queue_current_idx, imG_RenderQueue, IM_ARRAYSIZE(imG_RenderQueue));
				//static float rQueueVal = 0.0f;
				//
				//ImGui::Text("Render Queue"); ImGui::SameLine(WidthIndentation); ImGui::InputFloat("##RenderQueueInput1", &rQueueVal, 0.0f, 0.0f);
				//
				//
				//
				//
				//
				//static bool mat_GPUInstancing = false;
				//
				//ImGui::Text("Reflections ");
				//ImGui::SameLine(WidthIndentation);
				//
				//ImGui::Checkbox("##MatGPUInsta1 ", &mat_GPUInstancing);
				//
				//static bool mat_GlobalIllum = false;
				//
				//ImGui::Text("Double Sided Global Illumination ");
				//ImGui::SameLine(WidthIndentation);
				//
				//ImGui::Checkbox("##MatIllum1 ", &mat_GlobalIllum);



				ImGui::Separator();

				static glm::vec3 lightPos = glm::vec3(-18.5f, -12.6f, 31.6f);
				static bool showLightControls = false;


				glEnable(GL_MULTISAMPLE);



				static GLuint fbo = 0, textureColorbuffer = 0, rbo = 0; // Declare outside to persist across frames
				static bool isFramebufferGenerated = false; // Flag to prevent generating framebuffer every frame


				if (!isFramebufferGenerated) {
					// Generate framebuffer only once
					g_Coordinator.GetSystem<GraphicsSystem>()->generateNewFrameBuffer(fbo, textureColorbuffer, rbo, 512, 288);
					// std::cout << '\t' << fbo << '\t' << textureColorbuffer << '\t' << rbo << '\n';
					isFramebufferGenerated = true;
				}

				// Set clear color
				glClearColor(46.f / 255.f, 46.f / 255.f, 46.f / 255.f, 1.0f);  // Set the color to clear the framebuffer to

				// Bind the custom framebuffer (where you want to render)
				glBindFramebuffer(GL_FRAMEBUFFER, fbo);
				glViewport(0, 0, 512, 288);  // Set the custom viewport size for this framebuffer

				// Clear the framebuffer
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear the color and depth buffers

				// Now render your objects
// 
			 //   g_Coordinator.GetSystem<GraphicsSystem>()->DrawMaterialSphere(); // Your rendering code


				g_AssetManager.GetShader("Material").Use();

				glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);  // Position of the camera
				glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);     // Point the camera is looking at (center of the sphere)
				glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);         // Up direction

				glm::mat4 view = glm::lookAt(cameraPos, target, up);

				float aspectRatio = 512.0f / 288.0f;
				glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);


				g_AssetManager.GetShader("Material").SetUniform("model", glm::mat4(1.0f)); // Identity matrix for no transformation
				g_AssetManager.GetShader("Material").SetUniform("view", view);
				g_AssetManager.GetShader("Material").SetUniform("projection", projection);
				g_AssetManager.GetShader("Material").SetUniform("inputColor", glm::vec4(color[0], color[1], color[2], color[3]));
				g_AssetManager.GetShader("Material").SetUniform("inputLight", lightPos);

				g_AssetManager.GetShader("Material").SetUniform("viewPos", cameraPos);
				g_AssetManager.GetShader("Material").SetUniform("metallic", MetallicMatValue);
				g_AssetManager.GetShader("Material").SetUniform("smoothness", SmoothnessValue);
				//				g_AssetManager.GetShader("Material").SetUniform("shininess", shininessValue);  // Shininess value


				Model* sphereModel = g_ResourceManager.getModel("sphere");

				if (sphereModel)
					sphereModel->DrawMaterial(g_AssetManager.GetShader("Material"));
				else
					std::cerr << "cant find sphere model\n";




				g_AssetManager.GetShader("Material").UnUse();


				// After rendering to the custom framebuffer, unbind it to render to the default framebuffer
				glBindFramebuffer(GL_FRAMEBUFFER, 0);  // Unbind the custom framebuffer (restore to default framebuffer)

				// Reset the viewport for the default framebuffer (main window size)
				glViewport(0, 0, g_WindowX, g_WindowY);

				// Display the custom framebuffer texture (this will display in the ImGui window)
				ImGui::Image((void*)(intptr_t)textureColorbuffer, ImVec2(512, 288));  // Display texture in the ImGui window


				ImGui::Separator();
				ImGui::Text("Adjust Light Position");

				if (ImGui::Button("Open Light Controls")) {
					ImGui::OpenPopup("LightControlPopup");
				}

				// Popup modal window
				if (ImGui::BeginPopup("LightControlPopup")) {
					ImGui::Text("Adjust Light Position");
					ImGui::DragFloat("X", &lightPos.x, 0.1f, -10000.0f, 10000.0f, "X: %.1f");
					ImGui::DragFloat("Y", &lightPos.y, 0.1f, -10000.0f, 10000.0f, "Y: %.1f");
					ImGui::DragFloat("Z", &lightPos.z, 0.1f, -10000.0f, 10000.0f, "Z: %.1f");

					// Close button inside the popup
					if (ImGui::Button("Close")) {
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndPopup();
				}

			}
		}

	}

	ImGui::End();
}



// Asset Window is incomplete cause no Asset Manager yet
void ImGuiEditor::AssetWindow()
{
	std::string assetName;

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
			ImGuiFileDialog::Instance()->OpenDialog("AddAsset", "Choose File", ".png,.mp3,.wav,.csv,.json,.ttf,.vert,.frag,.fbx,.obj,.mtl", "../BoofWoof/Assets/");
		}

		ImGui::SameLine();
		if (ImGui::Button("Delete Asset"))
		{
			ImGuiFileDialog::Instance()->OpenDialog("DeleteAsset", "Choose File", ".png,.mp3,.wav,.csv,.json,.ttf,.vert,.frag,.fbx,.obj,.mtl", "../BoofWoof/Assets/");
		}

		if (ImGuiFileDialog::Instance()->Display("AddAsset"))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				// Get the selected file path
				fs::path previousPath(ImGuiFileDialog::Instance()->GetFilePathName());
				fs::path destinationPath = m_CurrDir / previousPath.filename();

				// If the destination file already exists, rename it with a counter
				if (fs::exists(destinationPath)) {
					int counter = 1;
					std::string nameWithoutExtension = previousPath.stem().string();  // File name without extension
					std::string extension = previousPath.extension().string();        // File extension

					std::string newFileName = nameWithoutExtension + "(" + std::to_string(counter) + ")" + extension;
					fs::path newDestinationPath = m_CurrDir / newFileName;

					// Find an available name
					while (fs::exists(newDestinationPath)) {
						counter++;
						newFileName = nameWithoutExtension + "(" + std::to_string(counter) + ")" + extension;
						newDestinationPath = m_CurrDir / newFileName;
					}

					// Move the file to the final destination with the new name
					try {
						fs::copy(previousPath, newDestinationPath);
					}
					catch (const fs::filesystem_error& e) {
						std::cerr << "Error copying file: " << e.what() << std::endl;
					}
				}
				else {
					// If no file with the same name exists, move it directly
					try {
						fs::copy(previousPath, destinationPath);
					}
					catch (const fs::filesystem_error& e) {
						std::cerr << "Error copying file: " << e.what() << std::endl;
					}
				}
			}

			// close
			ImGuiFileDialog::Instance()->Close();
		}

		if (ImGuiFileDialog::Instance()->Display("DeleteAsset"))
		{
			// Check if the user made a selection
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				//// Get the selected file path
				//std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

				//// find last / to get file name only
				//size_t lastSlash = filePathName.find_last_of("/\\");
				//std::string deleteFileName = filePathName.substr(lastSlash + 1);

				// Asset manager to delete the assets e.g. g_AssetManager.DeleteAssets(deleteFileName);
				g_AssetManager.DiscardToTrashBin(ImGuiFileDialog::Instance()->GetFilePathName(), FILEPATH_ASSET_TRASHBIN, false);
			}

			// close
			ImGuiFileDialog::Instance()->Close();
		}

		ImGui::Spacing();  ImGui::Separator(); ImGui::Spacing();

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int colCount = (int)(panelWidth / 100);
		if (colCount < 1) colCount = 1;	// to ensure that it will not go down to 0 when u push the column smaller than 1 size

		ImGui::Columns(colCount, 0, false);	// for resizing purposes

		//		std::cout << m_SelectedFile << '\n';

		static int materialCount = 1;
		// CREATE WINDOW WITH RIGHT CLICK HERE RIGHT CLICK RIGHT CLICK RIGHT CLICK RIGHT CLICK RIGHTCLICKRIGHTCLICK
		// Alternatively, use ImGui::BeginPopupContextWindow() for right-click anywhere in the window
		if (ImGui::BeginPopupContextWindow("WindowContextMenu", ImGuiMouseButton_Right)) {
			if (ImGui::MenuItem("Create Material Instances")) {
				// Handle action for "Create Material Instances"

				// Generate the material name based on the current count
				std::string materialName = "Material_" + std::to_string(materialCount);

				rapidjson::Document document;
				document.SetObject();
				rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

				// Add name and type (similar to your previous structure)
				document.AddMember("name", rapidjson::Value(materialName.c_str(), allocator), allocator);
				document.AddMember("type", rapidjson::Value("Example", allocator), allocator);

				// Add material properties (color, shininess, textureID, etc.)
				rapidjson::Value properties(rapidjson::kObjectType);

				// Adding color as a JSON array for RGBA values
				rapidjson::Value color(rapidjson::kArrayType);
				color.PushBack(1.0f, allocator);  // R
				color.PushBack(0.0f, allocator);  // G
				color.PushBack(0.0f, allocator);  // B
				color.PushBack(1.0f, allocator);  // A (Alpha)
				properties.AddMember("color", color, allocator);

				// Add shininess
				properties.AddMember("shininess", 0.5f, allocator);

				// Add texture ID
				properties.AddMember("textureID", 0, allocator);  // Modify this if you have a specific texture ID

				// Add shader name
				properties.AddMember("shader", rapidjson::Value("Direction_obj_render", allocator), allocator);

				// Add reflectivity
				properties.AddMember("reflectivity", 0.8f, allocator);

				// Adding the properties object to the document
				document.AddMember("properties", properties, allocator);

				// Serialize the JSON document to a string with pretty printing
				rapidjson::StringBuffer buffer;
				rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
				document.Accept(writer);

				// Write the JSON string to a file
				std::string filePath = entireFilePath.c_str();  // Convert path to string
				filePath += "/" + materialName + ".mat";  // Append the new file name (customize this if needed)

				// Write the JSON string to the file
				std::ofstream file(filePath);
				if (file.is_open()) {
					file << buffer.GetString();  // Write the serialized JSON
					file.close();
					std::cout << "File created: " << filePath << std::endl;
				}
				else {
					std::cerr << "Error: Could not create the file." << std::endl;
				}

				// Increment the material count for the next creation
				materialCount++;
			}

			if (ImGui::MenuItem("DOES NOT CREATE MATERIAL INSTANCES")) {
				// Handle action for "Window Option 2"
			}
			ImGui::EndPopup();
		}


		for (auto& entry : fs::directory_iterator(m_CurrDir))
		{

			const auto& path = entry.path();
			auto relativePath = fs::relative(path, m_BaseDir);
			std::string fileNameExt = relativePath.filename().string();

			// std::cout << path.string() << '\n';
			ImGui::PushID(fileNameExt.c_str()); // using filename as the ID so all are unique, the drag drop won't open same source
			ImGui::PushStyleColor(ImGuiCol_Button, ((path == m_SelectedFile) ? ImVec4(0.3f, 0.6f, 1.0f, 0.5f) : ImVec4(0, 0, 0, 0))); // for button background opacity to go 0

			size_t lastDot = fileNameExt.find_last_of(".");
			std::string fileName = fileNameExt.substr(0, lastDot);
			std::string fileExtension = fileNameExt.substr(lastDot + 1);
			std::string icon = entry.is_directory() ? "FolderIcon" : (fileExtension == "png" ? fileName : "TextIcon");
			int iconDDS = g_ResourceManager.GetTextureDDS(icon);

			ImGui::ImageButton((ImTextureID)(uintptr_t)(iconDDS != -1 ? iconDDS : g_ResourceManager.GetTextureDDS("BlackScreen")),
				{ 70, 70 }, { 0, 0 }, { 1, 1 }, 8);



			// drag from assets to components
			if (ImGui::BeginDragDropSource())
			{
				fs::path outerRelativePath = path.c_str();
				const wchar_t* itemPath = outerRelativePath.c_str();
				ImGui::SetDragDropPayload("Ass", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();
			}

			ImGui::PopStyleColor();

			// Detect selection on click
			if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
				// Check if the currently selected file is clicked again
				if (m_SelectedFile == path) {
					m_SelectedFile.clear(); // Deselect the file by clearing the selection
				}
				else {
					m_SelectedFile = path; // Set the selected file

					// Check for .png files (case insensitive)
					std::string extension = m_SelectedFile.extension().string();
					std::transform(
						extension.begin(),
						extension.end(),
						extension.begin(),
						[](unsigned char c) { return static_cast<char>(std::tolower(c)); }
					);
					if (extension == ".png") {
						std::string descriptorPath = FILEPATH_DESCRIPTOR_TEXTURES + "/" + m_SelectedFile.stem().string() + ".json";

						// Attempt to load the texture descriptor and handle potential failure
						if (!textureInfo.LoadTextureDescriptor(descriptorPath)) {
							std::cerr << "Failed to load texture descriptor for: " << descriptorPath << std::endl;
						}
					}
					else if (extension == ".mat")
					{
						//std::cout << m_SelectedFile.string() << '\n';

						if (materialInfo.LoadMaterialDescriptor(m_SelectedFile.string()))
						{
							std::cerr << "Failed to load material for this: " << m_SelectedFile.stem().string() << '\n';
						}
					}
				}
			}


			fs::path currentDirectory = fs::canonical(m_CurrDir);
			fs::path selectedFilePath = fs::canonical(m_SelectedFile);
			// Check if the selected file is within the current directory
			if ((selectedFilePath.parent_path() == currentDirectory) && fs::exists(selectedFilePath)) {
				// The selected file is in the current directory or its subdirectory
				//std::cout << "Selected file is within the current directory." << std::endl;
			}
			else {
				// The selected file is not in the current directory or its subdirectory
				//std::cout << "Selected file is NOT within the current directory." << std::endl;

				m_SelectedFile.clear();
			}

			// must double click to go next directory
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (entry.is_directory())
				{
					// showing folders
					m_CurrDir /= path.filename();
				}

				// if in the audio folder
				if (m_CurrDir == "../BoofWoof/Assets\\Audio")
				{
					if (fileExtension == "wav")
					{
						m_ShowAudio = true;
						m_AudioName = fileName;
					}
					else
					{
						m_ShowAudio = false;
						ImGui::OpenPopup("INCORRECT EXTENSION");
					}
				}
				if (m_CurrDir == "../BoofWoof/Assets/Audio/Corgi/Dog_Footsteps_Walk")
				{
					if (fileExtension == "wav")
					{
						m_ShowAudio = true;
						m_AudioName = fileName;
					}
					else
					{
						m_ShowAudio = false;
						ImGui::OpenPopup("INCORRECT EXTENSION");
					}
				}

			}

			if (ImGui::BeginPopupModal("INCORRECT EXTENSION", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("\n Only wav files are playable\n\n");
				ImGui::Separator();

				if (ImGui::Button("OK", ImVec2(50, 0))) { ImGui::CloseCurrentPopup(); }
				ImGui::EndPopup();
			}

			ImGui::TextWrapped(fileName.c_str());

			ImGui::NextColumn();

			ImGui::PopID();
		}




		ImGui::Columns(1);
		ImGui::End();
	}
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

void ImGuiEditor::Settings()
{
	ImGui::Begin("Settings");
	{




		if (ImGui::BeginTabBar("Controls")) {
			if (ImGui::BeginTabItem("Editor Properties")) {

				ImGui::SeparatorText("Configurations");

				// Window Size, Frame Rate, Frame Count, Camera Position (future)
				ImGui::Text("Window Size: %d x %d", g_Window->GetWindowWidth(), g_Window->GetWindowHeight());
				ImGui::Text("Frame Rate: %f", g_Window->GetFPS());
				ImGui::Text("Frame Count: %d", g_Core->m_CurrNumSteps);



				ImGui::PushItemWidth(250.0f);
				ImGui::Spacing();

				ImGui::SeparatorText("System DT (% of Total Game Loop)");

				//ImGui::Text("Graphics DT: %f", ((g_Core->m_GraphicsDT / g_Core->m_ElapsedDT) * 100));
				PlotSystemDT("Physics DT", static_cast<float>((g_Core->m_PhysicsDT / g_Core->m_ElapsedDT) * 100), static_cast<float>(g_Core->m_ElapsedDT));
				PlotSystemDT("GameLogic DT", static_cast<float>((g_Core->m_LogicDT / g_Core->m_ElapsedDT) * 100), static_cast<float>(g_Core->m_ElapsedDT));
				PlotSystemDT("Graphics DT", static_cast<float>((g_Core->m_GraphicsDT / g_Core->m_ElapsedDT) * 100), static_cast<float>(g_Core->m_ElapsedDT));

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Light")) {

				ImGui::SeparatorText("Light Configurations");

				ImGui::Text("Light On"); ImGui::SameLine(150.0f);
				ImGui::Checkbox("##Light On", &GraphicsSystem::lightOn);
				if (GraphicsSystem::lightOn)
				{
					GraphicsSystem::lightOn = true;
				}
				else
				{
					GraphicsSystem::lightOn = false;
				}

				// light position
				ImGui::PushItemWidth(250.0f);
				

				ImGui::Text("LightPos"); ImGui::SameLine(150.0f);

				// Fetch the current light position from the Graphics System
				glm::vec3 lightPos = g_Coordinator.GetSystem<GraphicsSystem>()->GetLightPos();

				if (ImGui::DragFloat3("##Light Pos", &lightPos.x, 0.1f))
				{
					g_Coordinator.GetSystem<GraphicsSystem>()->SetLightPos(lightPos);
				}



				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Gamma")) {
				// Juuuu

				ImGui::SeparatorText("Gamma Configurations");

				ImGui::PushItemWidth(250.0f);



				static float gammaValue = 0; // Index for the selected item
				// ImGui Controls
				static bool defaultGamma = true; // Variable to hold the state

				ImGui::Text("Default Gamma"); ImGui::SameLine(150.0f);

				ImGui::Checkbox("##Default Gamma", &defaultGamma);


				ImGui::Text("Gamma Value"); ImGui::SameLine(150.0f);

				if (ImGui::SliderFloat("##GammaSlidr", &gammaValue, 1.0f, 3.0f) || defaultGamma)
				{
					if (defaultGamma)
						gammaValue = 2.2f;

					GraphicsSystem::gammaValue = gammaValue;
				}



				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Raycast Debug")) {
				// Juuuu

				ImGui::SeparatorText("Display Ray Cast");

				ImGui::PushItemWidth(250.0f);

				ImGui::Text("Display Ray Cast"); ImGui::SameLine(150.0f);
				ImGui::Checkbox("##Display Ray Cast", &MyPhysicsSystem::RayCastDebug);
				if (MyPhysicsSystem::RayCastDebug)
				{
					MyPhysicsSystem::RayCastDebug = true;
				}
				else
				{
					MyPhysicsSystem::RayCastDebug = false;
				}




				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Brightness")) {

				ImGui::SeparatorText("Brightness Configurations");
				ImGui::Text("Brightness Value"); ImGui::SameLine(150.0f);
				float brightness = g_Coordinator.GetSystem<GraphicsSystem>()->GetBrightness();
				if (ImGui::SliderFloat("##Brightness", &brightness, 0.0f, 1.0f))
				{
					g_Coordinator.GetSystem<GraphicsSystem>()->SetBrightness(brightness);
				}

				ImGui::EndTabItem();

			}
			//if (ImGui::BeginTabItem("Tab 3")) {
			//	ImGui::Text("This is content for Tab 3.");
			//	ImGui::EndTabItem();
			//}
			ImGui::EndTabBar();
		}


	}

	ImGui::End();


	/// camera control
	if (g_Input.GetKeyState(GLFW_KEY_UP))
	{
		g_Coordinator.GetSystem<GraphicsSystem>()->GetCamera().ProcessKeyboard(Camera_Movement::FORWARD, static_cast<float>(g_Core->m_DeltaTime) + 0.05f);
	}
	if (g_Input.GetKeyState(GLFW_KEY_DOWN))
	{
		g_Coordinator.GetSystem<GraphicsSystem>()->GetCamera().ProcessKeyboard(Camera_Movement::BACKWARD, static_cast<float>(g_Core->m_DeltaTime) + 0.05f);
	}
	if (g_Input.GetKeyState(GLFW_KEY_LEFT))
	{
		g_Coordinator.GetSystem<GraphicsSystem>()->GetCamera().ProcessKeyboard(Camera_Movement::LEFT, static_cast<float>(g_Core->m_DeltaTime) + 0.05f);
	}
	if (g_Input.GetKeyState(GLFW_KEY_RIGHT))
	{
		g_Coordinator.GetSystem<GraphicsSystem>()->GetCamera().ProcessKeyboard(Camera_Movement::RIGHT, static_cast<float>(g_Core->m_DeltaTime) + 0.05f);
	}
	if (g_Input.GetKeyState(GLFW_KEY_RIGHT_SHIFT))
	{
		g_Coordinator.GetSystem<GraphicsSystem>()->GetCamera().ProcessKeyboard(Camera_Movement::UP, static_cast<float>(g_Core->m_DeltaTime) + 0.05f);
	}
	if (g_Input.GetKeyState(GLFW_KEY_RIGHT_CONTROL))
	{
		g_Coordinator.GetSystem<GraphicsSystem>()->GetCamera().ProcessKeyboard(Camera_Movement::DOWN, static_cast<float>(g_Core->m_DeltaTime) + 0.05f);
	}


	static glm::vec2 lastMousePos = glm::vec2(0.0f);
	if (g_Input.GetMouseState(GLFW_MOUSE_BUTTON_RIGHT))
	{

		glm::vec2 offset = g_Input.GetMousePosition() - lastMousePos;
		g_Coordinator.GetSystem<GraphicsSystem>()->GetCamera().ProcessMouseMovement(static_cast<float>(offset.x), static_cast<float>(-offset.y), 1);
	}
	lastMousePos = g_Input.GetMousePosition();
}

void ImGuiEditor::Scenes()
{
	static char fileNameBuffer[256] = "unnamed_scene"; // Default filename
	static bool showSavePopup = false;
	static bool showTransitionPopup = false;
	static float transitionDuration = 1.0f; // Default transition duration
	static bool showSceneSelectionWarning = false; // Flag to show the warning
	static bool showOverwritePopup = false;
	static bool showSavedPopup = false;
	static bool showFailedPopup = false;

	ImGui::Begin("Scenes");
	{
		// Load button with file dialog
		if (ImGui::Button("Load"))
		{
			ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".json", "../BoofWoof/Assets/Scenes/");
		}

		if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
				m_LastOpenedFile = filePathName;
				g_SceneManager.LoadScene(filePathName);
				g_Coordinator.GetSystem<LogicSystem>()->ReInit();
			}
			ImGuiFileDialog::Instance()->Close();
		}

		ImGui::SameLine();

		// Save Current World button functionality
		if (ImGui::Button("Save Current World"))
		{
			showSavePopup = true;  // Show the popup when the button is pressed
		}

		// Create a popup for file naming and saving
		if (showSavePopup)
		{
			ImGui::OpenPopup("Save Scene As");
		}

		if (ImGui::BeginPopupModal("Save Scene As", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Enter the name of the scene:");
			ImGui::InputText("##FileName", fileNameBuffer, IM_ARRAYSIZE(fileNameBuffer));

			if (ImGui::Button("Save"))
			{
				m_FileName = fileNameBuffer;
				if (m_FileName.empty())
				{
					m_FileName = "unnamed_scene";
				}

				m_FinalFileName = m_FileName;
				m_FilePath = GetScenesDir() + "/" + m_FinalFileName + ".json";

				if (fs::exists(m_FilePath))
				{
					showSavePopup = false;
					showOverwritePopup = true;
				}
				else
				{
					showSavePopup = false;

					if (g_SceneManager.SaveScene(m_FinalFileName + ".json"))
					{
						showSavedPopup = true;
					}
					else {
						showFailedPopup = true;
					}
				}
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel")) {
				showSavePopup = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		if (showOverwritePopup)
		{
			ImGui::OpenPopup("Overwrite Confirmation");
		}

		if (ImGui::BeginPopupModal("Overwrite Confirmation", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			showOverwritePopup = false;
			ImGui::Text("\nFile already exists. Do you want to overwrite?\n\n");

			if (ImGui::Button("Yes"))
			{
				// Overwrite the file
				if (g_SceneManager.SaveScene(m_FinalFileName + ".json"))
				{
					showSavedPopup = true;
				}
				else {
					showFailedPopup = true;
				}
				ImGui::CloseCurrentPopup();  // Close the overwrite popup
			}

			ImGui::SameLine();

			if (ImGui::Button("No"))
			{
				int counter = 1;

				while (fs::exists(m_FilePath))
				{
					m_FinalFileName = m_FileName + "_" + std::to_string(counter);
					m_FilePath = GetScenesDir() + "/" + m_FinalFileName + ".json";
					counter++;
				}

				if (g_SceneManager.SaveScene(m_FinalFileName + ".json"))
				{
					showSavedPopup = true;
				}
				else {
					showFailedPopup = true;
				}
				ImGui::CloseCurrentPopup();  // Close the overwrite popup
			}

			ImGui::EndPopup();
		}

		if (showSavedPopup)
		{
			ImGui::OpenPopup("Saved");
		}

		// Confirmation popup when the scene is saved
		if (ImGui::BeginPopupModal("Saved", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("\nYour world has been saved!\n\n");

			if (ImGui::Button("OK", ImVec2(50, 0)))
			{
				showSavedPopup = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		if (showFailedPopup)
		{
			ImGui::OpenPopup("Failed");
		}

		if (ImGui::BeginPopupModal("Failed", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("\nSaving failed! Try again.\n\n");

			if (ImGui::Button("OK", ImVec2(50, 0)))
			{
				showFailedPopup = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		ImGui::SeparatorText("Currently Loaded Scenes");

		// Loaded scenes display
		const auto& scenes = g_SceneManager.GetAllScenes();
		if (scenes.empty())
		{
			ImGui::Text("No scenes loaded");
			ImGui::NewLine();
		}
		else
		{
			for (const auto& [sceneGUID, scenePath] : scenes)
			{
				// Extract the file name from the full path
				fs::path path(scenePath);
				std::string sceneName = path.filename().string();  // Get the file name
				ImGui::Text("Scene: %s", sceneName.c_str());
				ImGui::Text("GUID: %s", sceneGUID.c_str());
				ImGui::SeparatorText("");
			}
		}
		// Add Transition button
		if (ImGui::Button("Transition to Scene"))
		{
			showTransitionPopup = true;  // Show the transition popup when button is pressed
		}

		// Open popup for scene transition
		if (showTransitionPopup)
		{
			ImGui::OpenPopup("Transition Scene");
		}

		// Transition scene popup
		if (ImGui::BeginPopupModal("Transition Scene", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Select a scene to transition to:");
			ImGui::SameLine();
			if (ImGui::Button("Open Scene Browser"))
			{
				ImGuiFileDialog::Instance()->OpenDialog("ChooseTransitionSceneDlgKey", "Choose Scene", ".json", "../BoofWoof/Assets/Scenes/");
			}

			ImGui::NewLine();

			// Display file dialog for selecting transition scene
			static std::string selectedTransitionScene;
			if (ImGuiFileDialog::Instance()->Display("ChooseTransitionSceneDlgKey"))
			{
				if (ImGuiFileDialog::Instance()->IsOk())
				{
					selectedTransitionScene = ImGuiFileDialog::Instance()->GetFilePathName();
				}
				ImGuiFileDialog::Instance()->Close();
			}

			// Show selected scene
			if (!selectedTransitionScene.empty())
			{
				fs::path transitionPath(selectedTransitionScene);
				ImGui::Text("Selected Scene: %s", transitionPath.filename().string().c_str());
				ImGui::NewLine();
			}

			// Input for transition duration
			ImGui::Text("Transition Duration (seconds):"); ImGui::SameLine();
			ImGui::PushItemWidth(150);  // Set the width to 50 pixels
			ImGui::InputFloat("##TransitionDuration", &transitionDuration, 0.1f, 1.0f, "%.2f");
			ImGui::PopItemWidth();     // Reset to default width after the widget
			ImGui::NewLine();

			// Start transition
			if (ImGui::Button("Start Transition"))
			{
				if (!selectedTransitionScene.empty())
				{
					g_SceneManager.TransitionToScene(selectedTransitionScene, transitionDuration);
					showTransitionPopup = false;
					ImGui::CloseCurrentPopup();
					selectedTransitionScene = "";
				}
				else
				{
					showSceneSelectionWarning = true;  // Set flag to true when no scene is selected
				}
			}

			ImGui::SameLine();

			// Cancel button
			if (ImGui::Button("Cancel"))
			{
				showTransitionPopup = false;
				selectedTransitionScene = "";
				ImGui::CloseCurrentPopup();
			}

			// Show the warning message if no scene was selected
			if (showSceneSelectionWarning)
			{
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "Please select a scene.");
			}

			ImGui::EndPopup();
		}

		ImGui::End();
	}
}

void ImGuiEditor::Audio()
{
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));                            // make button transparent
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.305f, 0.31f, 0.5f));    // same as original imgui colors but just lighter opacity
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.305f, 0.31f, 0.5f));

	// Check if the window is open, and assign it to a flag variable.
	bool audioWindowOpen = ImGui::Begin("Audio", &m_ShowAudio, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	if (audioWindowOpen)
	{
		// The window is open, proceed with audio controls.
		ImGui::Text(m_AudioName.c_str());
		ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
		std::string audioFileName = "../BoofWoof/Assets/Audio/";
		float size = ImGui::GetWindowHeight() * 0.3f;

		// Play Button
		if (ImGui::ImageButton((ImTextureID)(uintptr_t)g_ResourceManager.GetTextureDDS("PlayButton"), { size, size }, ImVec2(0, 0), ImVec2(1, 1), 0))
		{
			g_Audio.PlayFile(audioFileName + m_AudioName + ".wav");  // Play the audio file
			if (g_Audio.IsPaused()) {
				g_Audio.ResumeBGM();  // Resume the audio if it is paused
			}
		}

		ImGui::SameLine();

		// Pause Button - Toggle Pause/Resume based on current state
		if (g_Audio.IsPaused() == false) {
			if (ImGui::ImageButton((ImTextureID)(uintptr_t)g_ResourceManager.GetTextureDDS("PauseButton"), { size, size }, ImVec2(0, 0), ImVec2(1, 1), 0))
			{
				g_Audio.PauseBGM();  // Pause the audio if it is playing
			}
		}


		ImGui::SameLine();

		// Stop Button
		if (ImGui::ImageButton((ImTextureID)(uintptr_t)g_ResourceManager.GetTextureDDS("StopButton"), { size, size }, ImVec2(0, 0), ImVec2(1, 1), 0))
		{
			g_Audio.StopBGM();
		}
	}

	ImGui::End();

	// Invert m_ShowAudio if the window was closed
	if (!m_ShowAudio)
	{
		// If the window was closed, stop the audio
		g_Audio.StopBGM();
	}

	ImGui::PopStyleColor(3);
}

void ImGuiEditor::PlayStopRunBtn()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));						// so that there's a padding at the bottom of the button

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));							// make button transparent
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.305f, 0.31f, 0.5f));	// same as original imgui colors but just lighter opacity
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.305f, 0.31f, 0.5f));

	ImGui::Begin("##PlayStopButtons", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	{
		float size = ImGui::GetWindowHeight() - 4.0f;
		std::string icon = (m_State == States::Stop) ? "PlayButton" : "StopButton";
		// half the button, offset -> centered
		ImGui::SameLine((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));

		// If player doesn't exist, don't allow it to run.
		if (!m_PlayerExist)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f); // Optional: reduce the button's alpha to make it look disabled
		}

		if (ImGui::ImageButton((ImTextureID)(uintptr_t)g_ResourceManager.GetTextureDDS(icon), { size,size }, ImVec2(0, 0), ImVec2(1, 1), 0))
		{
			switch (m_State)
			{
			case States::Play:
				if (m_State != States::Stop)
				{
					// Hide this part first, will uncomment later on when basic scene is ready.
					/*g_Coordinator.ResetEntities();
					g_SceneManager.LoadScene(m_LastOpenedFile);*/
					m_State = States::Stop;
				}
				break;

			case States::Stop:
				if (m_State != States::Play)
				{
					// Maybe check whether editor active ? 
					m_State = States::Play;
				}
				break;
			}
		}

		if (m_State == States::Play && ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Stop");
			ImGui::EndTooltip();

		}
		if (m_State == States::Stop && ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Play");
			ImGui::EndTooltip();
		}

		ImGui::SameLine();

		if (ImGui::ImageButton((ImTextureID)(uintptr_t)g_ResourceManager.GetTextureDDS("RunScene"), { size,size }, ImVec2(0, 0), ImVec2(1, 1), 0))
		{
			// Go to full screen & Run actual game
		}

		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Run Scene");
			ImGui::EndTooltip();
		}

		ImGui::PopStyleVar();
		ImGui::PopStyleColor(3);

		if (!m_PlayerExist)
		{
			// Re-enable future items
			ImGui::PopItemFlag();
			ImGui::PopStyleVar(); // Only if you pushed the style var
		}

		ImGui::End();
	}
}

void ImGuiEditor::ArchetypeTest()
{
	ImGui::Begin("Archetype Manager");
	{
		// Input for new archetype name

		ImGui::Text("Archetype Name"); ImGui::SameLine();
		static char archetypeName[128] = "";
		ImGui::InputText("##ArchetypeName", archetypeName, IM_ARRAYSIZE(archetypeName));

		// Checkbox list for available component types
		static bool transformComponentSelected = false; // Toggle state for Transform Component
		static bool graphicsComponentSelected = false; // Toggle state for Graphics Component
		static bool collisionComponentSelected = false; // Toggle state for Collision Component
		static bool audioComponentSelected = false; // Toggle state for Collision Component

		ImGui::SeparatorText("Available Components");

		if (ImGui::Checkbox("Transform Component", &transformComponentSelected)) {
			if (transformComponentSelected) {
				compTypes.push_back(typeid(TransformComponent)); // Add component type
			}
			else {
				compTypes.erase(std::remove(compTypes.begin(), compTypes.end(), typeid(TransformComponent)), compTypes.end());
			}
		}

		if (ImGui::Checkbox("Graphics Component", &graphicsComponentSelected)) {
			if (graphicsComponentSelected) {
				compTypes.push_back(typeid(GraphicsComponent)); // Add component type
			}
			else {
				compTypes.erase(std::remove(compTypes.begin(), compTypes.end(), typeid(GraphicsComponent)), compTypes.end());
			}
		}

		if (ImGui::Checkbox("Collision Component", &collisionComponentSelected)) {
			if (collisionComponentSelected) {
				compTypes.push_back(typeid(CollisionComponent)); // Add component type
			}
			else {
				compTypes.erase(std::remove(compTypes.begin(), compTypes.end(), typeid(CollisionComponent)), compTypes.end());
			}
		}

		if (ImGui::Checkbox("Audio Component", &audioComponentSelected)) {
			if (audioComponentSelected) {
				compTypes.push_back(typeid(AudioComponent)); // Add component type
			}
			else {
				compTypes.erase(std::remove(compTypes.begin(), compTypes.end(), typeid(AudioComponent)), compTypes.end());
			}
		}

		// Flags for messages
		static bool showWarning = false;
		static std::string warningMessage;

		if (ImGui::Button("Create Archetype"))
		{
			bool noName = strlen(archetypeName) == 0;
			bool noComp = compTypes.empty();

			if (!noName && !noComp) {
				// Create archetype
				compTypes.push_back(typeid(MetadataComponent)); // Always include MetadataComponent
				std::vector<std::type_index> selectedComponents = compTypes;
				g_Arch.createArchetype(archetypeName, selectedComponents);

				// Reset inputs and selections
				memset(archetypeName, 0, sizeof(archetypeName));
				compTypes.clear();
				transformComponentSelected = graphicsComponentSelected = collisionComponentSelected = audioComponentSelected = false;

				showWarning = false; // Clear warning
			}
			else {
				// Set warning message based on input
				warningMessage = noName ? "Please add a name." : "Please select at least one component.";
				showWarning = true;
			}
		}

		if (showWarning)
		{
			ImGui::Text("%s", warningMessage.c_str());
		}

		// Dropdown to select existing archetypes for updating
		static int selectedArchetypeIndex = -1;
		auto archetypes = g_Arch.getArchetypes(); // Assuming this returns a vector of Archetype pointers
		std::vector<const char*> archetypeNames; // Vector to hold archetype names

		// Populate archetype names
		for (auto& archetype : archetypes) {
			archetypeNames.push_back(archetype->name.c_str()); // Assuming Archetype has a name property
		}


		ImGui::NewLine();
		ImGui::SeparatorText("Select Archetype to Update");
		ImGui::Text("Names"); ImGui::SameLine();
		ImGui::Combo("##UpdateArch", &selectedArchetypeIndex, archetypeNames.data(), static_cast<int>(archetypeNames.size()));

		if (selectedArchetypeIndex != -1) {
			ImGui::Text("Tick the above checkboxes to update!");
		}

		// Button to update the selected archetype
		if (ImGui::Button("Update Archetype") && selectedArchetypeIndex != -1)
		{
			Archetype* selectedArchetype = archetypes[selectedArchetypeIndex]; // Get selected archetype

			// Ensure always got metadatacomp
			compTypes.push_back(typeid(MetadataComponent));
			g_Arch.updateArchetype(selectedArchetype, compTypes); // Update the archetype with selected component types

			compTypes.clear(); // Clear selected component types for the next update
			transformComponentSelected = false; // Reset component selection states
			graphicsComponentSelected = false;
			collisionComponentSelected = false;
			audioComponentSelected = false;
			selectedArchetypeIndex = -1;
		}

		ImGui::NewLine();

		// List existing archetypes 
		ImGui::SeparatorText("Existing Archetypes");
		for (auto& archetype : g_Arch.getArchetypes())
		{
			// Create a tree node for components
			if (ImGui::TreeNode("%s", archetype->name.c_str()))
			{
				for (const auto& componentType : archetype->componentTypes)
				{
					// Display the component type as a bullet point
					if (componentType == typeid(TransformComponent)) ImGui::BulletText("TransformComponent");
					if (componentType == typeid(GraphicsComponent)) ImGui::BulletText("GraphicsComponent");
					if (componentType == typeid(CollisionComponent)) ImGui::BulletText("CollisionComponent");
					if (componentType == typeid(AudioComponent)) ImGui::BulletText("AudioComponent");
				}
				ImGui::TreePop(); // Close the tree node
			}

			// Create an entity using the selected archetype
			if (ImGui::Button(("Create " + archetype->name + " Entity").c_str())) {
				g_Arch.createEntity(archetype);
			}
		}
	}
	ImGui::End();
}


void ImGuiEditor::ShowPickingDebugWindow()
{
	ImGui::Begin("Picking Debug View");

	// Get the dimensions of the picking texture
	int width = g_Coordinator.GetSystem<GraphicsSystem>()->GetViewportWidth();
	int height = g_Coordinator.GetSystem<GraphicsSystem>()->GetViewportHeight();

	// Retrieve the picking texture
	GLuint pickingTexture = g_Coordinator.GetSystem<GraphicsSystem>()->GetPickingTexture();

	// Display the texture in ImGui
	ImGui::Image((void*)(intptr_t)pickingTexture, ImVec2((float)width, (float)height), ImVec2(0, 1), ImVec2(1, 0));

	ImGui::End();
}

void ImGuiEditor::DrawEntityNode(Entity entity)
{
	// Get entity name
	std::string entityName = "Entity " + std::to_string(entity);
	if (g_Coordinator.HaveComponent<MetadataComponent>(entity))
	{
		entityName = g_Coordinator.GetComponent<MetadataComponent>(entity).GetName();
	}

	ImGuiTreeNodeFlags flags = ((g_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0);
	flags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

	bool hasChildren = false;
	if (g_Coordinator.HaveComponent<HierarchyComponent>(entity))
	{
		auto& hierarchyComp = g_Coordinator.GetComponent<HierarchyComponent>(entity);
		hasChildren = !hierarchyComp.children.empty();
	}

	if (!hasChildren)
	{
		flags |= ImGuiTreeNodeFlags_Leaf;
	}

	bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)entity, flags, "%s", entityName.c_str());

	// Select entity when clicked
	if (ImGui::IsItemClicked())
	{
		g_SelectedEntity = entity;
	}

	// Drag source
	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload("DND_ENTITY", &entity, sizeof(Entity));
		ImGui::Text("%s", entityName.c_str());
		ImGui::EndDragDropSource();
	}

	// Drag target
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_ENTITY"))
		{
			Entity droppedEntity = *(Entity*)payload->Data;

			// Avoid reparenting to self or creating cyclic dependencies
			if (droppedEntity != entity && !IsAncestorOf(entity, droppedEntity))
			{
				// Capture old and new parents for undo/redo
				Entity oldParent = g_Coordinator.HaveComponent<HierarchyComponent>(droppedEntity)
					? g_Coordinator.GetComponent<HierarchyComponent>(droppedEntity).parent
					: MAX_ENTITIES;
				Entity newParent = entity;

				// Create doAction and undoAction lambdas
				auto doAction = [this, droppedEntity, newParent]() {
					SetParent(droppedEntity, newParent);
					};

				auto undoAction = [this, droppedEntity, oldParent]() {
					SetParent(droppedEntity, oldParent);
					};

				// Execute the action and add it to the UndoRedoManager
				g_UndoRedoManager.ExecuteCommand(doAction, undoAction);
			}
		}
		ImGui::EndDragDropTarget();
	}

	// Recursively draw children
	if (nodeOpen)
	{
		if (hasChildren)
		{
			auto& hierarchyComp = g_Coordinator.GetComponent<HierarchyComponent>(entity);
			for (auto child : hierarchyComp.children)
			{
				DrawEntityNode(child);
			}
		}
		ImGui::TreePop();
	}
}


void ImGuiEditor::SetParent(Entity child, Entity parent)
{
	// Prevent setting an entity as its own parent
	if (child == parent)
		return;

	// Check for cyclic dependency
	if (IsDescendantOf(parent, child))
		return;

	// Remove child from its current parent, if any
	RemoveParent(child);

	// Update child's HierarchyComponent
	if (parent != MAX_ENTITIES)
	{
		// Add or update HierarchyComponent for child
		if (!g_Coordinator.HaveComponent<HierarchyComponent>(child))
		{
			g_Coordinator.AddComponent(child, HierarchyComponent(parent));
		}
		else
		{
			auto& childHierarchy = g_Coordinator.GetComponent<HierarchyComponent>(child);
			childHierarchy.parent = parent;
		}

		// Ensure parent has HierarchyComponent
		if (!g_Coordinator.HaveComponent<HierarchyComponent>(parent))
		{
			g_Coordinator.AddComponent(parent, HierarchyComponent());
		}

		// Add child to parent's children
		auto& parentHierarchy = g_Coordinator.GetComponent<HierarchyComponent>(parent);
		parentHierarchy.AddChild(child);
	}
}

void ImGuiEditor::RemoveParent(Entity child)
{
	if (g_Coordinator.HaveComponent<HierarchyComponent>(child))
	{
		auto& childHierarchy = g_Coordinator.GetComponent<HierarchyComponent>(child);
		Entity parent = childHierarchy.parent;
		childHierarchy.parent = MAX_ENTITIES;

		if (parent != MAX_ENTITIES && g_Coordinator.HaveComponent<HierarchyComponent>(parent))
		{
			auto& parentHierarchy = g_Coordinator.GetComponent<HierarchyComponent>(parent);
			parentHierarchy.RemoveChild(child);
		}
	}
}


bool ImGuiEditor::IsDescendantOf(Entity potentialParent, Entity child)
{
	if (potentialParent == child)
		return true;

	if (g_Coordinator.HaveComponent<HierarchyComponent>(potentialParent))
	{
		auto& hierarchyComp = g_Coordinator.GetComponent<HierarchyComponent>(potentialParent);
		for (auto descendant : hierarchyComp.children)
		{
			if (IsDescendantOf(descendant, child))
				return true;
		}
	}
	return false;
}


bool ImGuiEditor::IsAncestorOf(Entity ancestor, Entity entity)
{
	if (entity == MAX_ENTITIES)
		return false;

	if (ancestor == entity)
		return true;

	if (g_Coordinator.HaveComponent<HierarchyComponent>(entity))
	{
		auto& hierarchyComp = g_Coordinator.GetComponent<HierarchyComponent>(entity);
		return IsAncestorOf(ancestor, hierarchyComp.parent);
	}

	return false;
}


void ImGuiEditor::DeleteEntity(Entity entity)
{
	// If the entity has children, delete them recursively
	if (g_Coordinator.HaveComponent<HierarchyComponent>(entity))
	{
		auto& hierarchyComp = g_Coordinator.GetComponent<HierarchyComponent>(entity);
		for (auto child : hierarchyComp.children)
		{
			DeleteEntity(child);
		}
	}

	// Remove the entity from its parent's children list
	RemoveParent(entity);

	// Destroy the entity
	g_Coordinator.DestroyEntity(entity);
}