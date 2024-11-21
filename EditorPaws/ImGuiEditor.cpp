#define MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS 0
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
#include "../Utilities/Components/AnimationComponent.h"
#include "Animation/AnimationManager.h"
#include "../Utilities/Components/MaterialComponent.hpp"

namespace fs = std::filesystem;

constexpr float EPSILON = 1e-6f;

AnimationManager g_AnimationManager;

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
	WorldHierarchy();
	InspectorWindow();
	AssetWindow();
	Settings();
	Scenes();
	PlayStopRunBtn();
	//ShowPickingDebugWindow();


	ArchetypeTest();

	if (m_ShowAudio)
	{
		Audio();
	}

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

			// Get the object's transform matrix
			glm::mat4 modelMatrix = transformComp.GetWorldMatrix();

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
				// Decompose the manipulated matrix to get position, rotation, scale
				glm::vec3 position, rotationDegrees, scale;
				ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(modelMatrix),
					glm::value_ptr(position), glm::value_ptr(rotationDegrees), glm::value_ptr(scale));

				// Convert rotation from degrees to radians
				glm::vec3 rotationRadians = glm::radians(rotationDegrees);

				// If manipulation has just started
				if (!m_WasUsingGizmo)
				{
					// Store old transform values
					m_OldPosition = transformComp.GetPosition();
					m_OldRotationRadians = transformComp.GetRotation();
					m_OldScale = transformComp.GetScale();
				}

				// Update the TransformComponent with new values
				transformComp.SetPosition(position);
				transformComp.SetRotation(rotationRadians);
				transformComp.SetScale(scale);
			}
			else if (m_WasUsingGizmo) // Manipulation has just ended
			{
				// Retrieve the new transform values
				glm::vec3 newPosition = transformComp.GetPosition();
				glm::vec3 newRotationRadians = transformComp.GetRotation();
				glm::vec3 newScale = transformComp.GetScale();

				Entity entity = g_SelectedEntity;

				// Store copies of the old values to capture in the lambda
				glm::vec3 oldPosition = m_OldPosition;
				glm::vec3 oldRotationRadians = m_OldRotationRadians;
				glm::vec3 oldScale = m_OldScale;

				// Check if any of the transform components have changed
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
				}
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
						g_Coordinator.GetSystem<MyPhysicsSystem>()->RemoveEntityBody(g_GettingDeletedEntity);
						g_Coordinator.DestroyEntity(g_GettingDeletedEntity);

					}
				}
				if (m_IsSelected)
				{
					if (ImGui::Selectable("Delete Selected Object"))
					{
						if (g_SelectedEntity != MAX_ENTITIES)
						{
							g_Coordinator.GetSystem<MyPhysicsSystem>()->RemoveEntityBody(g_SelectedEntity);
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

			if (ImGui::Button("Clear all entities")) {
				g_Coordinator.GetSystem<MyPhysicsSystem>()->ClearAllBodies();
				//g_Coordinator.GetSystem<GraphicsSystem>()->clearAllEntityTextures();
				g_Coordinator.ResetEntities();
				g_SceneManager.ClearSceneList();


			}
		}

		ImGui::Spacing(); ImGui::Spacing(); // to insert a gap so it looks visually nicer

		// Displaying hierarchy game objects - have to check for player next time
		const std::vector<Entity>& allEntities = g_Coordinator.GetAliveEntitiesSet();
		m_PlayerExist = false;

		for (const auto& entity : allEntities)
		{
			// looping through to get EntityID, use the entityID to get the name of object		
			auto& name = g_Coordinator.GetComponent<MetadataComponent>(entity).GetName();

			// checking based on whether game object has a name called "Player"
			if (name == "Player") m_PlayerExist = true;

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
					}
					if (ImGui::Selectable("Animation Component")) {
						if (!g_Coordinator.HaveComponent<AnimationComponent>(g_SelectedEntity)) {
							std::cout << "enter1\n";


							g_Coordinator.AddComponent<AnimationComponent>(g_SelectedEntity, AnimationComponent(std::make_shared<AnimationManager>(g_AnimationManager)));


							g_UndoRedoManager.ExecuteCommand(
								[this]() {
									if (!g_Coordinator.HaveComponent<AnimationComponent>(g_SelectedEntity)) {
										g_Coordinator.AddComponent<AnimationComponent>(g_SelectedEntity, AnimationComponent(std::make_shared<AnimationManager>(g_AnimationManager)));
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

					if (ImGui::Selectable("Material Component"))
					{
						if (!g_Coordinator.HaveComponent<MaterialComponent>(g_SelectedEntity))
						{
							g_Coordinator.AddComponent<MaterialComponent>(g_SelectedEntity, MaterialComponent());
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
							auto componentData = g_Coordinator.GetComponent<TransformComponent>(g_SelectedEntity);

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

						auto graphics = g_Coordinator.GetComponent<GraphicsComponent>(g_SelectedEntity);
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
						}







						//if (ImGui::Selectable("Add Animation")) {
						//	Animation newAnimation{/*initialize animation properties*/ };
						//	AddAnimation(graphics, newAnimation);
						//}
						//
						//if (graphics.hasAnimation && ImGui::Selectable("Remove Animation")) {
						//	RemoveAnimation(componentData);
						//}




					}

					if (g_Coordinator.HaveComponent<AnimationComponent>(g_SelectedEntity))
					{
						if (ImGui::Selectable("Animation Component"))
						{
							auto componentData = g_Coordinator.GetComponent<AnimationComponent>(g_SelectedEntity);

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
							auto componentData = g_Coordinator.GetComponent<AudioComponent>(g_SelectedEntity);

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
							auto componentData = g_Coordinator.GetComponent<BehaviourComponent>(g_SelectedEntity);

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
							auto componentData = g_Coordinator.GetComponent<CollisionComponent>(g_SelectedEntity);

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
							auto componentData = g_Coordinator.GetComponent<CameraComponent>(g_SelectedEntity);

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
							auto componentData = g_Coordinator.GetComponent<ParticleComponent>(g_SelectedEntity);

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
											property->SetValue(&transformComponent, SerializationHelpers::SerializeVec3(vecValue));
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
												},
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
												},
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
												}
											);
										}
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

								auto graphicsComponent = g_Coordinator.GetComponent<GraphicsComponent>(g_SelectedEntity);
								const auto& properties = ReflectionManager::Instance().GetProperties("GraphicsComponent");
								//if (ImGui::Button("Add Sub Components"))
								//{
								//	ImGui::OpenPopup("SComponents");
								//}


								//if (ImGui::BeginPopupContextItem("SComponents"))
								//{
								//	
								//
								//	if (ImGui::Selectable("Material Component"))
								//	{
								//		if (!g_Coordinator.HaveComponent<MaterialComponent>(g_SelectedEntity))
								//		{
								//			g_Coordinator.AddComponent<MaterialComponent>(g_SelectedEntity, MaterialComponent());
								//			graphicsComponent.hasMaterial = true;
								//			g_UndoRedoManager.ExecuteCommand(
								//				[this]() {
								//					if (!g_Coordinator.HaveComponent<MaterialComponent>(g_SelectedEntity))
								//						g_Coordinator.AddComponent<MaterialComponent>(g_SelectedEntity, MaterialComponent());
								//				},
								//				[this]() {
								//					if (g_Coordinator.HaveComponent<MaterialComponent>(g_SelectedEntity))
								//						g_Coordinator.RemoveComponent<MaterialComponent>(g_SelectedEntity);
								//				}
								//			);
								//	
								//		}
								//	}
								//
								//	ImGui::EndPopup();
								//}

								//if (graphicsComponent.hasMaterial) {
								//	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_None)) {
								//		auto& material = graphicsComponent.material;
								//
								//		// Display MaterialComponent properties
								//		ImGui::Text("Material Name: %s", material.name.c_str());
								//		ImGui::SliderFloat("Shininess", &material.shininess, 0.0f, 100.0f);
								//		ImGui::ColorEdit3("Diffuse Color", material.diffuseColor);
								//	}
								//}
								// Access subcomponents

							//std::cout <<"ths be material true\t" << graphicsComponent.hasMaterial << '\n';
							//if (graphicsComponent.hasMaterial) {
							//	if(ImGui::CollapsingHeader("Material Component", ImGuiTreeNodeFlags_None))
							//	{
							//		
							//
							//		
							//		const MaterialComponent& material = graphicsComponent.material;
							//		ImGui::Text("Material: %s", material.GetShaderName().c_str());
							//		ImGui::Text("Shininess %.2f", material.GetShininess());
							//
							//		std::cout << material.GetShaderName().c_str() << '\t' << material.GetShininess() << '\n';
							//
							//	}
							//}

								//if (ImGui::Selectable("Add Material")) {
								//	MaterialComponent newMaterial{/* Initialize material properties */ };
								//	graphics.AddMaterial(newMaterial);
								//}
								//
								//if (graphics.hasMaterial && ImGui::Selectable("Remove Material")) {
								//	graphics.RemoveMaterial();
								//}


								graphicsComponent.RegisterProperties();
								if (ImGui::TreeNode("Mesh Properties")) {

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
											// Selection changed
											std::string newModelName = modelNames[currentItem];
											(*modelNameProperty)->SetValue(&graphicsComponent, newModelName);

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

									ImGui::TreePop();
								}

								if (ImGui::TreeNode("old Texture"))
								{
									// Handle TextureName Property
									auto textureNameProperty = std::find_if(properties.begin(), properties.end(),
										[](const ReflectionPropertyBase* prop) { return prop->GetName() == "TextureName"; });

									if (textureNameProperty != properties.end())
									{
										std::string propertyName = "TextureName";
										std::string currentTextureName = (*textureNameProperty)->GetValue(&graphicsComponent);
										std::string newTextureName = currentTextureName;

										ImGui::Text("Texture ");
										ImGui::SameLine();
										ImGui::PushID(propertyName.c_str());

										char buffer[256];
										memset(buffer, 0, sizeof(buffer));
										strcpy_s(buffer, sizeof(buffer), currentTextureName.c_str());

										ImGui::InputText("##TextureName", buffer, sizeof(buffer), ImGuiInputTextFlags_ReadOnly);

										ImGui::SameLine();

										// Store old value before opening the file dialog
										static std::string oldTextureName = "";
										if (ImGui::Button("Set Texture"))
										{
											oldTextureName = currentTextureName; // Capture the old value
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

												newTextureName = selectedFile;
												(*textureNameProperty)->SetValue(&graphicsComponent, newTextureName);
												int textureId = g_ResourceManager.GetTextureDDS(newTextureName);
												graphicsComponent.AddTexture(textureId);




												// Execute undo/redo command
												std::string oldValue = oldTextureName;
												Entity entity = g_SelectedEntity;

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

									ImGui::TreePop();
								}
								// camera follow settings
								bool followCamera = graphicsComponent.getFollowCamera();
								ImGui::Checkbox("Follow Camera", &followCamera);
								graphicsComponent.SetFollowCamera(followCamera);

							}
						}
						else if (className == "AnimationComponent") {
							// Show the dropdown menu if there are any files
							if (!g_AnimationManager.animationNames.empty()) {

								auto& animationComponent = g_Coordinator.GetComponent<AnimationComponent>(g_SelectedEntity);

								if (ImGui::CollapsingHeader("Animation", ImGuiTreeNodeFlags_None)) {

									// Get the current animations from the component
									int idleAnimationIndex = g_AnimationManager.GetAnimationIndex(animationComponent.GetAnimation(AnimationType::Idle));
									int movementAnimationIndex = g_AnimationManager.GetAnimationIndex(animationComponent.GetAnimation(AnimationType::Moving));
									int actionAnimationIndex = g_AnimationManager.GetAnimationIndex(animationComponent.GetAnimation(AnimationType::Action));

									ImGui::Text("Animation Selection:"); // Add a label before the combo boxes
									ImGui::NewLine();

									float comboBoxWidth = 200.0f; // Set a consistent width for the combo boxes
									float labelWidth = 200.0f;   // Reserve space for the labels

									// Idle Animation
									ImGui::Text("Idle Animation:");
									ImGui::SameLine(labelWidth);
									ImGui::SetNextItemWidth(comboBoxWidth);
									if (ImGui::BeginCombo("##IdleAnimation",
										idleAnimationIndex == -1 ? "None" : g_AnimationManager.animationNames[idleAnimationIndex].c_str())) {
										// "None" option
										bool isSelected = (idleAnimationIndex == -1);
										if (ImGui::Selectable("None", isSelected)) {
											idleAnimationIndex = -1; // Set to "None"
										}
										if (isSelected) {
											ImGui::SetItemDefaultFocus();
										}

										// File options
										for (int i = 0; i < g_AnimationManager.animationNames.size(); ++i) {
											isSelected = (idleAnimationIndex == i);
											if (ImGui::Selectable(g_AnimationManager.animationNames[i].c_str(), isSelected)) {
												idleAnimationIndex = i; // Update the idle animation index
											}
											if (isSelected) {
												ImGui::SetItemDefaultFocus();
											}
										}
										ImGui::EndCombo();
									}

									ImGui::NewLine();

									// Movement Animation
									ImGui::Text("Movement Animation:");
									ImGui::SameLine(labelWidth);
									ImGui::SetNextItemWidth(comboBoxWidth);
									if (ImGui::BeginCombo("##MovementAnimation",
										movementAnimationIndex == -1 ? "None" : g_AnimationManager.animationNames[movementAnimationIndex].c_str())) {
										// "None" option
										bool isSelected = (movementAnimationIndex == -1);
										if (ImGui::Selectable("None", isSelected)) {
											movementAnimationIndex = -1; // Set to "None"
										}
										if (isSelected) {
											ImGui::SetItemDefaultFocus();
										}

										// File options
										for (int i = 0; i < g_AnimationManager.animationNames.size(); ++i) {
											isSelected = (movementAnimationIndex == i);
											if (ImGui::Selectable(g_AnimationManager.animationNames[i].c_str(), isSelected)) {
												movementAnimationIndex = i; // Update the movement animation index
											}
											if (isSelected) {
												ImGui::SetItemDefaultFocus();
											}
										}
										ImGui::EndCombo();
									}

									ImGui::NewLine();

									// Action Animation
									ImGui::Text("Action Animation:");
									ImGui::SameLine(labelWidth);
									ImGui::SetNextItemWidth(comboBoxWidth);
									if (ImGui::BeginCombo("##ActionAnimation",
										actionAnimationIndex == -1 ? "None" : g_AnimationManager.animationNames[actionAnimationIndex].c_str())) {
										// "None" option
										bool isSelected = (actionAnimationIndex == -1);
										if (ImGui::Selectable("None", isSelected)) {
											actionAnimationIndex = -1; // Set to "None"
										}
										if (isSelected) {
											ImGui::SetItemDefaultFocus();
										}

										// File options
										for (int i = 0; i < g_AnimationManager.animationNames.size(); ++i) {
											isSelected = (actionAnimationIndex == i);
											if (ImGui::Selectable(g_AnimationManager.animationNames[i].c_str(), isSelected)) {
												actionAnimationIndex = i; // Update the action animation index
											}
											if (isSelected) {
												ImGui::SetItemDefaultFocus();
											}
										}
										ImGui::EndCombo();
									}

									ImGui::NewLine();

									// Buttons
									if (ImGui::Button("Apply")) {
										auto animationComponent = g_Coordinator.GetComponent<AnimationComponent>(g_SelectedEntity);

										// Apply or remove the Idle animation
										if (idleAnimationIndex == -1) {
											animationComponent.SetAnimation(AnimationType::Idle, ""); // Remove animation
										}
										else {
											std::string idleAnimation = g_AnimationManager.animationNames[idleAnimationIndex];
											animationComponent.SetAnimation(AnimationType::Idle, idleAnimation);
										}

										// Apply or remove the Movement animation
										if (movementAnimationIndex == -1) {
											animationComponent.SetAnimation(AnimationType::Moving, ""); // Remove animation
										}
										else {
											std::string movementAnimation = g_AnimationManager.animationNames[movementAnimationIndex];
											animationComponent.SetAnimation(AnimationType::Moving, movementAnimation);
										}

										// Apply or remove the Action animation
										if (actionAnimationIndex == -1) {
											animationComponent.SetAnimation(AnimationType::Action, ""); // Remove animation
										}
										else {
											std::string actionAnimation = g_AnimationManager.animationNames[actionAnimationIndex];
											animationComponent.SetAnimation(AnimationType::Action, actionAnimation);
										}

										std::cout << "Animations applied successfully to entity " << g_SelectedEntity << "." << std::endl;
									}

									ImGui::SameLine();
									if (ImGui::Button("Revert To Default")) {
										// Revert logic here
									}
								}
							}
							else {
								if (ImGui::CollapsingHeader("Animation", ImGuiTreeNodeFlags_None)) {
									ImGui::Text("No animations found.");
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

									const char* behaviourNames[] = { "Null", "Player" };
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

										Entity entity = g_SelectedEntity;

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

									ImGui::PopID();
								}
							}
						}
						else if (className == "CollisionComponent")
						{
							// Custom UI for CollisionComponent
							if (ImGui::CollapsingHeader("Collision", ImGuiTreeNodeFlags_None))
							{
								// Add custom collision component UI here
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
										ImGui::PushID(i);
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


							}
						}

						else if (className == "MaterialComponent")
						{
							if (ImGui::CollapsingHeader("Materials", ImGuiTreeNodeFlags_None))
							{
								auto graphicsComponent = g_Coordinator.GetComponent<GraphicsComponent>(g_SelectedEntity);
								const MaterialComponent& material = graphicsComponent.material;
								ImGui::Text("Material: %s", material.GetShaderName().c_str());
								ImGui::Text("Shininess %.2f", material.GetShininess());





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
				int maxWidth = 32;


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

				ImGui::Image((ImTextureID)(uintptr_t)(pictureIcon != -1 ? pictureIcon : g_ResourceManager.GetTextureDDS("BlackScreen")), ImVec2(newWidth, newHeight));
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
						ImGui::Image((ImTextureID)(uintptr_t)(pictureIcon != -1 ? pictureIcon : g_ResourceManager.GetTextureDDS("BlackScreen")), ImVec2(newWidth, newHeight));

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
				properties.AddMember("shader", rapidjson::Value("Shader3D", allocator), allocator);

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
					std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
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
		ImGui::SeparatorText("Configurations");

		// Window Size, Frame Rate, Frame Count, Camera Position (future)
		ImGui::Text("Window Size: %d x %d", g_Window->GetWindowWidth(), g_Window->GetWindowHeight());
		ImGui::Text("Frame Rate: %f", g_Window->GetFPS());
		ImGui::Text("Frame Count: %d", g_Core->m_CurrNumSteps);

		// light position
		ImGui::PushItemWidth(250.0f);
		ImGui::Text("LightPos"); ImGui::SameLine();

		// Fetch the current light position from the Graphics System
		glm::vec3 lightPos = g_Coordinator.GetSystem<GraphicsSystem>()->GetLightPos();

		if (ImGui::DragFloat3("##Light Pos", &lightPos.x, 0.1f))
		{
			g_Coordinator.GetSystem<GraphicsSystem>()->SetLightPos(lightPos);
		}

		ImGui::Checkbox("Light On", &GraphicsSystem::lightOn);
		if (GraphicsSystem::lightOn)
		{
			GraphicsSystem::lightOn = true;
		}
		else
		{
			GraphicsSystem::lightOn = false;
		}

		ImGui::Spacing();

		ImGui::SeparatorText("System DT (% of Total Game Loop)");

		//ImGui::Text("Graphics DT: %f", ((g_Core->m_GraphicsDT / g_Core->m_ElapsedDT) * 100));
		PlotSystemDT("Physics DT", static_cast<float>((g_Core->m_PhysicsDT / g_Core->m_ElapsedDT) * 100), static_cast<float>(g_Core->m_ElapsedDT));
		PlotSystemDT("GameLogic DT", static_cast<float>((g_Core->m_LogicDT / g_Core->m_ElapsedDT) * 100), static_cast<float>(g_Core->m_ElapsedDT));
		PlotSystemDT("Graphics DT", static_cast<float>((g_Core->m_GraphicsDT / g_Core->m_ElapsedDT) * 100), static_cast<float>(g_Core->m_ElapsedDT));
	}

	ImGui::End();


	/// camera control
	if (g_Input.GetKeyState(GLFW_KEY_UP))
	{
		g_Coordinator.GetSystem<GraphicsSystem>()->GetCamera().ProcessKeyboard(Camera_Movement::FORWARD, static_cast<float>(g_Core->m_DeltaTime));
	}
	if (g_Input.GetKeyState(GLFW_KEY_DOWN))
	{
		g_Coordinator.GetSystem<GraphicsSystem>()->GetCamera().ProcessKeyboard(Camera_Movement::BACKWARD, static_cast<float>(g_Core->m_DeltaTime));
	}
	if (g_Input.GetKeyState(GLFW_KEY_LEFT))
	{
		g_Coordinator.GetSystem<GraphicsSystem>()->GetCamera().ProcessKeyboard(Camera_Movement::LEFT, static_cast<float>(g_Core->m_DeltaTime));
	}
	if (g_Input.GetKeyState(GLFW_KEY_RIGHT))
	{
		g_Coordinator.GetSystem<GraphicsSystem>()->GetCamera().ProcessKeyboard(Camera_Movement::RIGHT, static_cast<float>(g_Core->m_DeltaTime));
	}
	if (g_Input.GetKeyState(GLFW_KEY_RIGHT_SHIFT))
	{
		g_Coordinator.GetSystem<GraphicsSystem>()->GetCamera().ProcessKeyboard(Camera_Movement::UP, static_cast<float>(g_Core->m_DeltaTime));
	}
	if (g_Input.GetKeyState(GLFW_KEY_RIGHT_CONTROL))
	{
		g_Coordinator.GetSystem<GraphicsSystem>()->GetCamera().ProcessKeyboard(Camera_Movement::DOWN, static_cast<float>(g_Core->m_DeltaTime));
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