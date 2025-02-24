/**************************************************************************
 * @file SceneManager.cpp
 * @author 	Liu Xujie
 * @param DP email: l.xujie@digipen.edu [2203183]
 * @param Course: CS 3401
 * @param Course: Game Project 3
 * @date  09/25/2024 (25 SEPT 2024)
 * @brief
 *
 * This file contains the definitions of member functions of SceneManager
 * Class
 *************************************************************************/
#include "pch.h"

#include "SceneManager.h"
#include "ECS/Coordinator.hpp"  // Assuming the coordinator handles entities
#include "Serialization/Serialization.h"
#include "../EngineCore.h"
#include "ResourceManager/ResourceManager.h"
#include "../Core/AssetManager/FilePaths.h"

 /**************************************************************************
  * @brief Constructor for the SceneManager class.
  *
  * This constructor initializes the SceneManager class with default values.
  * It sets the transitioning flag to `false`, and initializes the transition
  * time and duration to 0.0f. Upon construction, a message is printed
  * to indicate that the SceneManager has been initialized.
  *
  * @note This constructor sets up the scene manager for future scene
  * transitions and management.
  *
  * @param None
  * @return None
  *************************************************************************/

SceneManager::SceneManager() : transitioning(false), transitionTime(0.0f), transitionDuration(0.0f)
{
    std::cout << "SceneManager Initialized\n";
}


/**************************************************************************
 * @brief Destructor for the SceneManager class.
 *
 * This destructor handles the cleanup and destruction of the SceneManager
 * instance. Upon destruction, a message is printed to indicate that the
 * SceneManager has been destroyed.
 *
 * @note Any necessary cleanup related to scene management should be handled
 * here, ensuring that resources are properly released.
 *
 * @param None
 * @return None
 *************************************************************************/

SceneManager::~SceneManager()
{
    std::cout << "SceneManager Destroyed\n";
}

/**************************************************************************
 * @brief Creates a new scene with the specified name.
 *
 * This function creates a new scene by setting the current scene to
 * the provided scene name. It also resets the scene by clearing all
 * entities and performs any necessary scene initialization.
 *
 * @param sceneName The name of the scene to be created.
 *
 * @note This function ensures that the scene is reset before starting
 * any new operations on it.
 *
 * @return None
 *************************************************************************/

void SceneManager::CreateScene(const std::string& sceneName)
{
    currentScene = sceneName;

    std::cout << "Created scene: " << sceneName << std::endl;

    // Reset the scene by clearing all entities
    ResetScene();
}

/**************************************************************************
 * @brief Loads a scene from the specified file path.
 *
 * This function loads a scene using the Serialization system from the
 * provided file path. Upon successful loading, it sets the current scene
 * to the loaded file, registers the scene's GUID, and adds the scene to
 * the list of loaded scenes.
 *
 * @param filepath The path to the scene file to be loaded.
 *
 * @return bool Returns `true` if the scene was successfully loaded,
 * `false` otherwise.
 *
 * @note If the scene fails to load, an error message is logged and
 * the function returns `false`.
 *************************************************************************/

bool SceneManager::LoadScene(const std::string& filepath)
{
    // Use the Serialization system to load a scene from a file
    if (Serialization::LoadScene(filepath))
    {
        currentScene = filepath;

        // After loading, register the scene's GUID
        std::string sceneGUID = Serialization::GetSceneGUID();
        AddSceneToList(filepath, sceneGUID);  // Add to list only on load

        std::cout << "Loaded scene: " << filepath << " with GUID: " << sceneGUID << std::endl;
        return true;
    }
    else
    {
        std::cerr << "Failed to load scene: " << filepath << std::endl;
        return false;
    }
}

/**************************************************************************
 * @brief Loads a scene using its GUID (Globally Unique Identifier).
 *
 * This function attempts to load a scene by its GUID. It checks if the GUID
 * exists in the map and, if found, retrieves the associated file path and
 * loads the scene. If the GUID is not found, an error message is logged.
 *
 * @param sceneGUID The GUID of the scene to be loaded.
 *
 * @return bool Returns `true` if the scene was successfully loaded,
 * `false` otherwise.
 *
 * @note The scene must be registered with a GUID in the map for this function
 * to successfully load the scene.
 *************************************************************************/

bool SceneManager::LoadSceneByGUID(const std::string& sceneGUID)
{
    // Check if the GUID exists in the map
    if (guidToFileMap.find(sceneGUID) != guidToFileMap.end())
    {
        std::string filepath = guidToFileMap[sceneGUID];
        return LoadScene(filepath);  // Add to list only on load
    }
    else
    {
        std::cerr << "Scene with GUID " << sceneGUID << " not found." << std::endl;
        return false;
    }
}

/**************************************************************************
 * @brief Saves the current scene to the specified file path.
 *
 * This function saves the current scene using the Serialization system
 * to the provided file path. Upon successful saving, it logs the save
 * operation. If saving fails, an error message is logged.
 *
 * @param filepath The path to the file where the scene should be saved.
 *
 * @return bool Returns `true` if the scene was successfully saved,
 * `false` otherwise.
 *
 * @note The scene is not added to any list after saving; this function
 * only performs the save operation.
 *************************************************************************/

bool SceneManager::SaveScene(const std::string& filepath)
{
    // Use the Serialization system to save the current scene
    if (Serialization::SaveScene(filepath))
    {
        // After saving, just log the save. Do not add to list.
        std::cout << "Saved scene to: " << filepath << std::endl;
        return true;
    }
    else
    {
        std::cerr << "Failed to save scene: " << filepath << std::endl;
        return false;
    }
}

/**************************************************************************
 * @brief Adds a scene to the scene list if it's not already present.
 *
 * This function adds a scene to the scene list using its name and GUID.
 * It first checks if the scene is already present in the list by comparing
 * the GUID. If the scene is not in the list, it is added, and the GUID
 * is mapped to the scene name. If the scene is already in the list,
 * a message is logged indicating that it already exists.
 *
 * @param sceneName The name of the scene to be added.
 * @param sceneGUID The GUID of the scene to be added.
 *
 * @return None
 *
 * @note The scene is only added if it does not already exist in the list.
 *************************************************************************/

void SceneManager::AddSceneToList(const std::string& sceneName, const std::string& sceneGUID)
{
    // Check if the scene is already in the list
    auto it = std::find_if(sceneList.begin(), sceneList.end(),
        [&](const std::pair<std::string, std::string>& pair) { return pair.first == sceneGUID; });

    if (it == sceneList.end()) {
        // If it's not in the list, add it
        sceneList.emplace_back(sceneGUID, sceneName);
        guidToFileMap[sceneGUID] = sceneName;  // Add GUID to map
        std::cout << "Added scene to list: " << sceneName << " with GUID: " << sceneGUID << std::endl;
    }
    else {
        std::cout << "Scene with GUID " << sceneGUID << " already exists in the list." << std::endl;
    }
}


/**************************************************************************
 * @brief Retrieves the name of the current scene.
 *
 * This function returns the name of the currently active scene.
 *
 * @return std::string The name of the current scene.
 *************************************************************************/

std::string SceneManager::GetCurrentSceneName() const
{
    return currentScene;
}

/**************************************************************************
 * @brief Retrieves a list of all scenes and their corresponding GUIDs.
 *
 * This function returns a reference to a vector containing all the scenes
 * currently managed by the SceneManager, along with their corresponding GUIDs.
 *
 * @return const std::vector<std::pair<std::string, std::string>>&
 *         A reference to the list of scenes and their GUIDs.
 *************************************************************************/

const std::vector<std::pair<std::string, std::string>>& SceneManager::GetAllScenes() const
{
    return sceneList;
}

/**************************************************************************
 * @brief Resets the current scene by clearing all entities and the scene list.
 *
 * This function clears all entities from the current scene using the
 * Coordinator's `ResetEntities()` method. It also clears the scene list
 * to ensure that the scene is fully reset.
 *
 * @return None
 *
 * @note After the reset, a message is logged indicating that the scene has
 * been cleared of all entities.
 *************************************************************************/

void SceneManager::ResetScene()
{
    // Clear all entities from the current scene
    g_Coordinator.ResetEntities();
    ClearSceneList();
    std::cout << "Scene reset: Cleared all entities\n";
}

/**************************************************************************
 * @brief Initiates a transition to a new scene over a specified duration.
 *
 * This function starts the process of transitioning to a new scene by
 * calling `BeginTransition` with the target scene name and transition
 * duration. The transition effect allows for smooth changes between scenes.
 *
 * @param sceneName The name of the scene to transition to.
 * @param duration The duration of the transition in seconds.
 *
 * @return None
 *
 * @note The transition effect is managed by the `BeginTransition` method.
 *************************************************************************/

void SceneManager::TransitionToScene(const std::string& sceneName, float duration)
{
    BeginTransition(sceneName, duration);
}

/**************************************************************************
 * @brief Begins the process of transitioning to a target scene.
 *
 * This function initiates a transition to the specified target scene. It sets
 * the transition duration and resets the transition time. The transitioning
 * flag is set to `true` to indicate that the transition process is in progress.
 *
 * @param sceneName The name of the target scene to transition to.
 * @param duration The duration of the transition in seconds.
 *
 * @return None
 *************************************************************************/

void SceneManager::BeginTransition(const std::string& sceneName, float duration)
{
    targetScene = sceneName;
    transitionDuration = duration;
    transitionTime = 0.0f;
    transitioning = true;
}

/**************************************************************************
 * @brief Completes the transition to the target scene.
 *
 * This function completes the transition process by loading the target
 * scene. If the scene is successfully loaded, it updates the current scene
 * and calls the `onSceneLoadedCallback` if it is set. A message is logged
 * indicating that the transition to the new scene is complete. The
 * transitioning flag is then set to `false`.
 *
 * @return None
 *
 * @note This function should be called when the transition duration has ended.
 *************************************************************************/

void SceneManager::CompleteTransition()
{
    if (LoadScene(targetScene))
    {
        currentScene = targetScene;
        if (onSceneLoadedCallback)
        {
            onSceneLoadedCallback(currentScene);
        }
        std::cout << "Transitioned to scene: " << currentScene << std::endl;
    }
    transitioning = false;
}

/**************************************************************************
 * @brief Checks if a scene transition is currently in progress.
 *
 * This function returns the state of the `transitioning` flag, indicating
 * whether a scene transition is currently in progress.
 *
 * @return bool `true` if a transition is in progress, `false` otherwise.
 *************************************************************************/

bool SceneManager::IsTransitioning() const
{
    return transitioning;
}

/**************************************************************************
 * @brief Updates the SceneManager, managing scene transitions over time.
 *
 * This function updates the SceneManager by incrementing the transition
 * time if a scene transition is in progress. Once the transition time
 * exceeds or equals the transition duration, the current scene is reset,
 * and the transition is completed.
 *
 * @param deltaTime The time elapsed since the last update, used to track
 *        transition progress.
 *
 * @return None
 *
 * @note This function should be called regularly (e.g., once per frame)
 * to manage the scene transition timing.
 *************************************************************************/

void SceneManager::Update(float deltaTime)
{
    if (transitioning)
    {
        transitionTime += deltaTime;

        if (transitionTime >= transitionDuration)
        {
            ResetScene(); //for now i guess.
            CompleteTransition();
        }
    }

    // Handle asynchronous scene loading
    if (m_IsAsyncLoading)
    {
        if (AsyncLoadIsComplete())
        {
            m_PendingSceneData = m_AsyncLoader.GetLoadedData();
            m_IsAsyncLoading = false;   // No longer loading in background
            m_NeedsFinalize = true;     // We need to finalize the scene
        }
    }

    // If we have data ready to finalize...
    if (m_NeedsFinalize)
    {
        if (!m_ChunkFinalize)
        {
            // One-shot finalize
            FinalizeSceneData(m_PendingSceneData);
            m_NeedsFinalize = false;

            // If you have a callback or something
            currentScene = m_PendingSceneData.sceneGUID; // Or the filepath
            if (onSceneLoadedCallback)
                onSceneLoadedCallback(currentScene);
        }
        else
        {
            // Chunk-based finalize each frame
            const int chunkSize = 10; // e.g., finalize 10 entities per frame
            FinalizeSceneDataChunked(m_PendingSceneData, m_FinalizeIndex, chunkSize);

            // If we’re done finalizing
            if (m_FinalizeIndex >= m_PendingSceneData.entityList.size())
            {
                m_NeedsFinalize = false;
                m_ChunkFinalize = false;

                // e.g. set currentScene to something
                currentScene = m_PendingSceneData.sceneGUID;
                if (onSceneLoadedCallback)
                    onSceneLoadedCallback(currentScene);
            }
        }
    }
}

/**************************************************************************
 * @brief Sets the callback function to be invoked when a scene is loaded.
 *
 * This function sets a callback that will be triggered whenever a new scene
 * is successfully loaded. The callback function takes the name of the
 * loaded scene as its parameter.
 *
 * @param callback A `std::function` that accepts a `std::string` representing
 *        the name of the loaded scene.
 *
 * @return None
 *
 * @note The callback will be invoked during the transition process once
 * the scene is fully loaded.
 *************************************************************************/

void SceneManager::SetSceneLoadedCallback(std::function<void(const std::string&)> callback)
{
    onSceneLoadedCallback = callback;
}


void SceneManager::BeginAsyncLoad(const std::string& filepath)
{
    if (currentScene != FILEPATH_ASSET_SCENES + "/LoadingScreen.json")
    {
        g_Coordinator.ResetEntities();
    }

    // Start the background parse
    m_AsyncLoader.BeginLoad(filepath);
    m_IsAsyncLoading = true;
    m_NeedsFinalize = false;
    m_ChunkFinalize = true; // or false if you want instant finalize
    m_FinalizeIndex = 0;
}

void SceneManager::FinalizeSceneData(const SceneData& data)
{
    // If you want to do the entire finalize in one function call:
    Serialization::FinalizeEntitiesFromSceneData(data);
    // Also handle any post-processing
}

void SceneManager::FinalizeSceneDataChunked(const SceneData& data, size_t& index, int chunkSize)
{
    // Step through entityList from index to index+chunkSize
    for (int i = 0; i < chunkSize && index < data.entityList.size(); ++i, ++index)
    {
        const auto& ed = data.entityList[index];
        auto& jsonObj = ed.entityJSON;
        int oldID = ed.oldEntityID;

        // 1) Create new ECS entity
        Entity newE = g_Coordinator.CreateEntity();

        // 2) Store the mapping from oldID to newE (for references later)
        m_OldToNewMap[oldID] = newE;

        // 2) Attach components (single pass). 
        //    This is basically a subset of your LoadScene logic.

        // -- MetadataComponent --
        if (jsonObj.HasMember("MetadataComponent"))
        {
            const auto& metaData = jsonObj["MetadataComponent"];
            if (metaData.HasMember("EntityName"))
            {
                std::string name = metaData["EntityName"].GetString();
                MetadataComponent metaComp(name, newE);
                g_Coordinator.AddComponent(newE, metaComp);
            }
        }

        // -- TransformComponent --
        if (jsonObj.HasMember("TransformComponent"))
        {
            const auto& TData = jsonObj["TransformComponent"];
            glm::vec3 position(0.0f), scale(1.0f), rotation(0.0f), rotationPivot(0.0f);

            if (TData.HasMember("Position"))
            {
                position.x = TData["Position"]["x"].GetFloat();
                position.y = TData["Position"]["y"].GetFloat();
                position.z = TData["Position"]["z"].GetFloat();
            }

            if (TData.HasMember("Scale"))
            {
                scale.x = TData["Scale"]["x"].GetFloat();
                scale.y = TData["Scale"]["y"].GetFloat();
                scale.z = TData["Scale"]["z"].GetFloat();
            }

            if (TData.HasMember("Rotation"))
            {
                rotation.x = TData["Rotation"]["x"].GetFloat();
                rotation.y = TData["Rotation"]["y"].GetFloat();
                rotation.z = TData["Rotation"]["z"].GetFloat();
            }

            if (TData.HasMember("RotationPivotOffset"))
            {
                rotationPivot.x = TData["RotationPivotOffset"]["x"].GetFloat();
                rotationPivot.y = TData["RotationPivotOffset"]["y"].GetFloat();
                rotationPivot.z = TData["RotationPivotOffset"]["z"].GetFloat();
            }

            TransformComponent transComp(position, scale, rotation, rotationPivot, newE);
            g_Coordinator.AddComponent(newE, transComp);
        }

        // -- GraphicsComponent --
        if (jsonObj.HasMember("GraphicsComponent"))
        {
            const auto& GData = jsonObj["GraphicsComponent"];
            std::string modelName, textureName;
            bool followCam = false;

            if (GData.HasMember("ModelName"))
                modelName = GData["ModelName"].GetString();
            if (GData.HasMember("Texture"))
                textureName = GData["Texture"].GetString();
            if (GData.HasMember("FollowCamera"))
                followCam = GData["FollowCamera"].GetBool();

            GraphicsComponent gComp(modelName, newE, followCam);

            // Load the texture from ResourceManager
            int texID = g_ResourceManager.GetTextureDDS(textureName);
            if (texID > 0)
                gComp.AddTexture(texID);

            g_Coordinator.AddComponent(newE, gComp);
        }

        // -- AudioComponent --
        if (jsonObj.HasMember("AudioComponent"))
        {
            const auto& AData = jsonObj["AudioComponent"];
            if (AData.HasMember("AudioFilePath"))
            {
                std::string audioPath = AData["AudioFilePath"].GetString();
                float volume = AData["Volume"].GetFloat();
                bool loop = AData["ShouldLoop"].GetBool();

                AudioComponent audioC(audioPath, volume, loop, newE, nullptr, AudioType::SFX);
                g_Coordinator.AddComponent(newE, audioC);
            }
        }

        // -- BehaviourComponent --
        if (jsonObj.HasMember("BehaviourComponent"))
        {
            const auto& BData = jsonObj["BehaviourComponent"];
            if (BData.HasMember("BehaviourName"))
            {
                std::string behaviourName = BData["BehaviourName"].GetString();
                BehaviourComponent bc(behaviourName, newE);
                g_Coordinator.AddComponent(newE, bc);
            }
        }

        // -- CollisionComponent --
        if (jsonObj.HasMember("CollisionComponent"))
        {
            const auto& CData = jsonObj["CollisionComponent"];
            int layer = CData["CollisionLayer"].GetInt();
            bool isDynamic = CData["IsDynamic"].GetBool();
            bool isPlayer = CData["IsPlayer"].GetBool();

            CollisionComponent colComp(layer);
            colComp.SetIsDynamic(isDynamic);
            colComp.SetIsPlayer(isPlayer);

            if (CData.HasMember("AABBSize"))
            {
                glm::vec3 sz;
                sz.x = CData["AABBSize"]["x"].GetFloat();
                sz.y = CData["AABBSize"]["y"].GetFloat();
                sz.z = CData["AABBSize"]["z"].GetFloat();
                colComp.SetAABBSize(sz);
            }

            if (CData.HasMember("AABBOffset"))
            {
                glm::vec3 off;
                off.x = CData["AABBOffset"]["x"].GetFloat();
                off.y = CData["AABBOffset"]["y"].GetFloat();
                off.z = CData["AABBOffset"]["z"].GetFloat();
                colComp.SetAABBOffset(off);
            }

            if (CData.HasMember("Mass"))
            {
                float mass = CData["Mass"].GetFloat();
                colComp.SetMass(mass);
            }

            g_Coordinator.AddComponent(newE, colComp);
        }

        // -- CameraComponent --
        if (jsonObj.HasMember("CameraComponent"))
        {
            const auto& CCData = jsonObj["CameraComponent"];
            glm::vec3 camPos(0.f), camUp(0.f, 1.f, 0.f);
            float yaw = 0.f, pitch = 0.f;
            bool active = false;

            if (CCData.HasMember("Position"))
            {
                camPos.x = CCData["Position"]["x"].GetFloat();
                camPos.y = CCData["Position"]["y"].GetFloat();
                camPos.z = CCData["Position"]["z"].GetFloat();
            }

            if (CCData.HasMember("Up"))
            {
                camUp.x = CCData["Up"]["x"].GetFloat();
                camUp.y = CCData["Up"]["y"].GetFloat();
                camUp.z = CCData["Up"]["z"].GetFloat();
            }
            if (CCData.HasMember("Yaw"))
                yaw = CCData["Yaw"].GetFloat();
            if (CCData.HasMember("Pitch"))
                pitch = CCData["Pitch"].GetFloat();
            if (CCData.HasMember("Active"))
                active = CCData["Active"].GetBool();

            CameraComponent camC(camPos, camUp, yaw, pitch, active);
            g_Coordinator.AddComponent(newE, camC);
        }

        // -- ParticleComponent --
        if (jsonObj.HasMember("ParticleComponent"))
        {
            const auto& PData = jsonObj["ParticleComponent"];
            glm::vec3 posMin(0), posMax(0);
            float density = 1.0f;
            float velMin = 0.f;
            float velMax = 0.f;
            float size = 1.f;
            glm::vec4 color(1.f);

            std::vector<glm::vec3> targetPositions;

            if (PData.HasMember("PositionMin"))
            {
                posMin.x = PData["PositionMin"]["x"].GetFloat();
                posMin.y = PData["PositionMin"]["y"].GetFloat();
                posMin.z = PData["PositionMin"]["z"].GetFloat();
            }
            if (PData.HasMember("PositionMax"))
            {
                posMax.x = PData["PositionMax"]["x"].GetFloat();
                posMax.y = PData["PositionMax"]["y"].GetFloat();
                posMax.z = PData["PositionMax"]["z"].GetFloat();
            }
            if (PData.HasMember("Density"))
            {
                density = PData["Density"].GetFloat();
            }
            if (PData.HasMember("VelocityMin"))
            {
                velMin = PData["VelocityMin"].GetFloat();
            }
            if (PData.HasMember("VelocityMax"))
            {
                velMax = PData["VelocityMax"].GetFloat();
            }
            if (PData.HasMember("TargetPositions"))
            {
                const auto& arr = PData["TargetPositions"].GetArray();
                for (auto& item : arr)
                {
                    glm::vec3 temp(
                        item["x"].GetFloat(),
                        item["y"].GetFloat(),
                        item["z"].GetFloat()
                    );
                    targetPositions.push_back(temp);
                }
            }
            if (PData.HasMember("ParticleSize"))
            {
                size = PData["ParticleSize"].GetFloat();
            }
            if (PData.HasMember("ParticleColor"))
            {
                const auto& cObj = PData["ParticleColor"];
                color.r = cObj["r"].GetFloat();
                color.g = cObj["g"].GetFloat();
                color.b = cObj["b"].GetFloat();
                color.a = cObj["a"].GetFloat();
            }
            std::string particle_type = "POINT";
            if (PData.HasMember("ParticleType")) particle_type = PData["ParticleType"].GetString();

            ParticleType type = ParticleComponent::stringToParticleType(particle_type);

            std::string particle_model = "Bed";
            if (PData.HasMember("ParticleModel")) particle_model = PData["ParticleModel"].GetString();


            std::string particle_texture = "Bed";
            if (PData.HasMember("ParticleTexture")) particle_texture = PData["ParticleTexture"].GetString();

			ParticleComponent partC(density, posMin, posMax, velMin, velMax, targetPositions, size, color, type, particle_model, particle_texture);
            g_Coordinator.AddComponent(newE, partC);
        }

        // -- LightComponent --
        if (jsonObj.HasMember("LightComponent"))
        {
            const auto& LData = jsonObj["LightComponent"];
            float intensity = 1.f;
            glm::vec3 col(1.f);
			bool shadow = false;
			glm::vec3 dir(1.f, 0.f, 0.f);
			float range = 10.f;

            if (LData.HasMember("LightIntensity"))
                intensity = LData["LightIntensity"].GetFloat();
            if (LData.HasMember("LightColor"))
            {
                col.x = LData["LightColor"]["r"].GetFloat();
                col.y = LData["LightColor"]["g"].GetFloat();
                col.z = LData["LightColor"]["b"].GetFloat();
            }
			if (LData.HasMember("Shadow"))
			{
				shadow = LData["Shadow"].GetBool();
			}
			if (LData.HasMember("Direction"))
			{
				dir.x = LData["Direction"]["x"].GetFloat();
				dir.y = LData["Direction"]["y"].GetFloat();
				dir.z = LData["Direction"]["z"].GetFloat();
			}
			if (LData.HasMember("Range"))
			{
				range = LData["Range"].GetFloat();
			}


            LightComponent lightC(intensity, col, shadow,dir,range);
            g_Coordinator.AddComponent(newE, lightC);
        }

        // -- MaterialComponent --
        if (jsonObj.HasMember("MaterialComponent"))
        {
            const auto& MatData = jsonObj["MaterialComponent"];
            std::string matName, shaderName;
            int shaderIdx = 0;

            if (MatData.HasMember("name"))
                matName = MatData["name"].GetString();
            if (MatData.HasMember("shader"))
                shaderName = MatData["shader"].GetString();
            if (MatData.HasMember("shaderIdx"))
                shaderIdx = MatData["shaderIdx"].GetInt();

            MaterialComponent matC;
            matC.SetMaterialName(matName);
            matC.SetShaderName(shaderName);
            matC.SetShaderIndex(shaderIdx);

            if (MatData.HasMember("properties"))
            {
                const auto& props = MatData["properties"];

                // color
                if (props.HasMember("color"))
                {
                    const auto& cArr = props["color"].GetArray();
                    glm::vec4 col(
                        cArr[0].GetFloat(),
                        cArr[1].GetFloat(),
                        cArr[2].GetFloat(),
                        cArr[3].GetFloat()
                    );
                    matC.SetColor(col);
                }
                // finalAlpha
                if (props.HasMember("finalAlpha"))
                {
                    matC.SetFinalAlpha(props["finalAlpha"].GetFloat());
                }
                // Diffuse / Normal / Height
                if (props.HasMember("Diffuse"))
                {
                    std::string diff = props["Diffuse"].GetString();
                    matC.SetDiffuseName(diff);
                    matC.SetDiffuseID(g_ResourceManager.GetTextureDDS(diff));
                }
                if (props.HasMember("Normal"))
                {
                    std::string norm = props["Normal"].GetString();
                    matC.SetNormalName(norm);
                }
                if (props.HasMember("Height"))
                {
                    std::string hgt = props["Height"].GetString();
                    matC.SetHeightName(hgt);
                }
                // metallic
                if (props.HasMember("metallic"))
                {
                    matC.SetMetallic(props["metallic"].GetFloat());
                }
                // shininess
                if (props.HasMember("shininess"))
                {
                    matC.SetSmoothness(props["shininess"].GetFloat());
                }
            }
            if (g_Coordinator.HaveComponent<GraphicsComponent>(newE))
            {
                auto& gComp = g_Coordinator.GetComponent<GraphicsComponent>(newE);
                gComp.material = matC;
                // If there's a diffuse name, set the texture
                gComp.setTexture(matC.GetDiffuseName());
                gComp.SetDiffuse(matC.GetDiffuseID());
            }

            g_Coordinator.AddComponent(newE, matC);
        }

        // -- UIComponent --
        if (jsonObj.HasMember("UIComponent"))
        {
            const auto& UIData = jsonObj["UIComponent"];
            int textureID = 0;
            glm::vec2 pos(0.f), scl(1.f);
            float layer = 0.f;
            bool selectable = false;
            float opacity = 1.f;
            bool animated = false;
            int rows = 1;
            int cols = 1;
            float frameInt = 0.f;

            if (UIData.HasMember("TextureID"))
                textureID = UIData["TextureID"].GetInt();
            if (UIData.HasMember("PositionX") && UIData.HasMember("PositionY"))
            {
                pos.x = UIData["PositionX"].GetFloat();
                pos.y = UIData["PositionY"].GetFloat();
            }
            if (UIData.HasMember("ScaleX") && UIData.HasMember("ScaleY"))
            {
                scl.x = UIData["ScaleX"].GetFloat();
                scl.y = UIData["ScaleY"].GetFloat();
            }
            if (UIData.HasMember("Layer"))
                layer = UIData["Layer"].GetFloat();
            if (UIData.HasMember("Selectable"))
                selectable = UIData["Selectable"].GetBool();
            if (UIData.HasMember("Opcaity"))
                opacity = UIData["Opcaity"].GetFloat();
            if (UIData.HasMember("Animated"))
                animated = UIData["Animated"].GetBool();
            if (UIData.HasMember("Rows"))
                rows = UIData["Rows"].GetInt();
            if (UIData.HasMember("Cols"))
                cols = UIData["Cols"].GetInt();
            if (UIData.HasMember("FrameInterval"))
                frameInt = UIData["FrameInterval"].GetFloat();

            std::string textureName = g_ResourceManager.GetTextureDDSFileName(textureID);

            UIComponent uiC(textureName, pos, scl, layer, selectable, opacity);
            uiC.set_animate(animated);
            uiC.set_rows(rows);
            uiC.set_cols(cols);
            uiC.set_frame_interval(frameInt);

            g_Coordinator.AddComponent(newE, uiC);
        }

        if (jsonObj.HasMember("HierarchyComponent"))
        {
            // Instead of attaching it now, we store it for the final pass
            DeferredHierarchy deferred;
            deferred.oldEntityID = oldID;

            // Copy the subobject into a new rapidjson::Document
            deferred.hierarchyJson.SetObject();
            deferred.hierarchyJson.CopyFrom(jsonObj["HierarchyComponent"], deferred.hierarchyJson.GetAllocator());

            m_DeferredRefData.push_back(std::move(deferred));
        }
    }

    if (index >= data.entityList.size())
    {
        // Done creating chunked entities, now do final references
        FinalizeReferences();
    }

}

void SceneManager::FinalizeReferences()
{
    // For each deferred Hierarchy reference
    for (auto& deferred : m_DeferredRefData)
    {
        int oldEntID = deferred.oldEntityID;
        // The new ECS entity
        Entity newE = m_OldToNewMap[oldEntID];

        // The subobject describing "HierarchyComponent"
        const rapidjson::Document& hJson = deferred.hierarchyJson;

        // Parse the "Parent"
        Entity parentEntity = MAX_ENTITIES;
        if (hJson.HasMember("Parent"))
        {
            int oldParentID = hJson["Parent"].GetInt();
            if (oldParentID != MAX_ENTITIES)
            {
                parentEntity = m_OldToNewMap[oldParentID];
            }
        }

        // Parse the "Children" array
        std::vector<Entity> children;
        if (hJson.HasMember("Children") && hJson["Children"].IsArray())
        {
            for (auto& childIDValue : hJson["Children"].GetArray())
            {
                int oldChildID = childIDValue.GetInt();
                Entity childE = m_OldToNewMap[oldChildID];
                children.push_back(childE);
            }
        }

        // Now create the HierarchyComponent with correct references
        HierarchyComponent hComp;
        hComp.parent = parentEntity;
        hComp.children = children;

        // Attach to the entity
        g_Coordinator.AddComponent(newE, hComp);
    }

    // Done. Clear the reference data and mapping
    m_DeferredRefData.clear();
    m_OldToNewMap.clear();
}

bool SceneManager::AsyncLoadIsComplete() const
{
    // This simply returns the done status of the async loader.
    return m_AsyncLoader.IsDone();
}
