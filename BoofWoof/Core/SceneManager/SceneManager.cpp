#include "SceneManager.h"
#include <iostream>
#include "ECS/Coordinator.hpp"  // Assuming the coordinator handles entities
#include "Serialization/Serialization.h"
#include <algorithm>

SceneManager::SceneManager() : transitioning(false), transitionTime(0.0f), transitionDuration(0.0f)
{
    std::cout << "SceneManager Initialized\n";
}

SceneManager::~SceneManager()
{
    std::cout << "SceneManager Destroyed\n";
}

void SceneManager::CreateScene(const std::string& sceneName)
{
    currentScene = sceneName;

    std::cout << "Created scene: " << sceneName << std::endl;

    // Reset the scene by clearing all entities
    ResetScene();
}

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

std::string SceneManager::GetCurrentSceneName() const
{
    return currentScene;
}

const std::vector<std::pair<std::string, std::string>>& SceneManager::GetAllScenes() const
{
    return sceneList;
}

void SceneManager::ResetScene()
{
    // Clear all entities from the current scene
    g_Coordinator.ResetEntities();
    ClearSceneList();
    std::cout << "Scene reset: Cleared all entities\n";
}

void SceneManager::TransitionToScene(const std::string& sceneName, float transitionDuration)
{
    BeginTransition(sceneName, transitionDuration);
}

void SceneManager::BeginTransition(const std::string& sceneName, float duration)
{
    targetScene = sceneName;
    transitionDuration = duration;
    transitionTime = 0.0f;
    transitioning = true;
}

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

bool SceneManager::IsTransitioning() const
{
    return transitioning;
}

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
}

void SceneManager::SetSceneLoadedCallback(std::function<void(const std::string&)> callback)
{
    onSceneLoadedCallback = callback;
}
