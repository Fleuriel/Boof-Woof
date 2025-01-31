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
        // Check if the background thread is done
        if (m_AsyncLoader.IsDone())
        {
            // Retrieve the parsed data
            m_PendingSceneData = m_AsyncLoader.GetLoadedData();

            // Now we have the JSON data, we must finalize ECS creation
            m_IsAsyncLoading = false;   // No longer loading in background
            m_NeedsFinalize = true;     // We need to finalize
        }
        else
        {
            // We’re still loading in the background. Render "Loading..." or update a progress bar
            // Possibly you can poll partial progress. 
            // e.g. float progress = approximate / total
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
    // 1. Clear the current scene if needed
    g_Coordinator.ResetEntities();

    // 2. Start the background parse
    m_AsyncLoader.BeginLoad(filepath);
    m_IsAsyncLoading = true;
    m_NeedsFinalize = false;
    m_ChunkFinalize = true; // or false if you want instant finalize
    m_FinalizeIndex = 0;

    // If you want to show a “loading” UI, set transitioning = true or something similar
    // Or set up a "LoadingLevel" with a separate flow
}


void SceneManager::FinalizeSceneData(const SceneData& data)
{
    // If you want to do the entire finalize in one function call:
    Serialization::FinalizeEntitiesFromSceneData(data);
    // Also handle any post-processing
}

void SceneManager::FinalizeSceneDataChunked(const SceneData& data, size_t& index, int chunkSize)
{
    // Step through entityList from index -> index+chunkSize
    for (int i = 0; i < chunkSize && index < data.entityList.size(); ++i, ++index)
    {
        const auto& ed = data.entityList[index];
        // If you haven’t created the entity yet, do it earlier or do it lazily
        // We can do that in two passes. For simplicity, do a single pass example:

        // 1) If index == 0, first pass create all ECS entities
        //    But that’s complicated. Let's assume we already created them in BeginAsyncLoad
        //    or do partial approach. It's up to you.

        // The simpler approach is a 2-pass chunk approach:
        //   pass 1: create all ECS entities
        //   pass 2: attach components in chunks
        // For brevity, let's do everything at once if they exist:
        Entity newE = g_Coordinator.CreateEntity();
        

        // oldEntityID? If we truly need oldEntityID -> newE mapping, we store it in a container
        // But let's assume we do not have references for this example or we do a second pass after.

        auto& jsonObj = ed.entityJSON;
        // Attach each component as you do in your “LoadScene” code:
        if (jsonObj.HasMember("TransformComponent"))
        {
            // ...
        }
        // ... etc ...
    }
}
