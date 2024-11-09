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


bool GraphicsSystem::debug = false;


namespace fs = std::filesystem;

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
				g_Coordinator.GetSystem<GraphicsSystem>()->clearAllEntityTextures();
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
					if (ImGui::Selectable("Graphics Component"))
					{
						auto componentData = g_Coordinator.GetComponent<GraphicsComponent>(g_SelectedEntity);

						g_UndoRedoManager.ExecuteCommand(
							[this]() {
								if (g_Coordinator.HaveComponent<GraphicsComponent>(g_SelectedEntity))
									g_Coordinator.RemoveComponent<GraphicsComponent>(g_SelectedEntity);
							},
							[this, componentData]() {
								if (!g_Coordinator.HaveComponent<GraphicsComponent>(g_SelectedEntity))
									g_Coordinator.AddComponent<GraphicsComponent>(g_SelectedEntity, componentData);
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
							auto& graphicsComponent = g_Coordinator.GetComponent<GraphicsComponent>(g_SelectedEntity);
							graphicsComponent.RegisterProperties();

							const auto& properties = ReflectionManager::Instance().GetProperties("GraphicsComponent");

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

										g_UndoRedoManager.ExecuteCommand(
											[entity, newTextureName]() {
												auto& component = g_Coordinator.GetComponent<GraphicsComponent>(entity);
												component.setTexture(newTextureName);
											},
											[entity, oldValue]() {
												auto& component = g_Coordinator.GetComponent<GraphicsComponent>(entity);
												component.setTexture(oldValue);
											}
										);
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

							// light position
							ImGui::PushItemWidth(250.0f);
							ImGui::Text("LightPos"); ImGui::SameLine();

							// Fetch the current light position from the Graphics System
							glm::vec3 lightPos = g_Coordinator.GetSystem<GraphicsSystem>()->GetLightPos();

							if (ImGui::DragFloat3("##Light Pos", &lightPos.x, 0.1f))
							{
								g_Coordinator.GetSystem<GraphicsSystem>()->SetLightPos(lightPos);
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
				}
			}
		}
		else
		{
			ImGui::Text("No entity selected or invalid entity ID.");
		}
	}
	ImGui::End();
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
			int iconDDS = g_ResourceManager.GetTextureDDS(icon);

			ImGui::ImageButton((ImTextureID)(uintptr_t)(iconDDS != -1 ? iconDDS : g_ResourceManager.GetTextureDDS("BlackScreen")),
				{ 60, 60 }, { 0, 0 }, { 1, 1 });


			// drag from assets to components
			if (ImGui::BeginDragDropSource())
			{
				fs::path outerRelativePath = path.c_str();
				const wchar_t* itemPath = outerRelativePath.c_str();
				ImGui::SetDragDropPayload("Ass", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();
			}

			ImGui::PopStyleColor();

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
		g_Coordinator.GetSystem<GraphicsSystem>()->GetCamera().ProcessMouseMovement(static_cast<float>(offset.x), static_cast<float>(-offset.y),1);
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

		if (ImGui::ImageButton((ImTextureID)(uintptr_t)g_ResourceManager.GetTextureDDS("RunScene"), {size,size}, ImVec2(0, 0), ImVec2(1, 1), 0))
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
		ImGui::Combo("##UpdateArch", &selectedArchetypeIndex, archetypeNames.data(), archetypeNames.size());

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