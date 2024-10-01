#include "SceneManager.h"
#include <iostream>
#include "ECS/Coordinator.hpp"  // Assuming the coordinator handles entities
#include "Serialization/Serialization.h"

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
    sceneList.push_back(sceneName);

    std::cout << "Created scene: " << sceneName << std::endl;

    // Reset the scene by clearing all entities
    g_Coordinator.ResetEntities();
}

bool SceneManager::LoadScene(const std::string& filepath)
{
    // Use the Serialization system to load a scene from a file
    ResetScene();
    if (Serialization::LoadScene(filepath))
    {
        currentScene = filepath;
        sceneList.push_back(filepath);
        std::cout << "Loaded scene: " << filepath << std::endl;
        return true;
    }
    else
    {
        std::cerr << "Failed to load scene: " << filepath << std::endl;
        return false;
    }
}

bool SceneManager::SaveScene(const std::string& filepath)
{
    // Use the Serialization system to save the current scene
    if (Serialization::SaveScene(filepath))
    {
        std::cout << "Saved scene to: " << filepath << std::endl;
        return true;
    }
    else
    {
        std::cerr << "Failed to save scene: " << filepath << std::endl;
        return false;
    }
}

std::string SceneManager::GetCurrentSceneName() const
{
    return currentScene;
}

const std::vector<std::string>& SceneManager::GetAllScenes() const
{
    return sceneList;
}

void SceneManager::ResetScene()
{
    // Clear all entities from the current scene
    g_Coordinator.ResetEntities();
    std::cout << "Scene reset: Cleared all entities\n";
}

void SceneManager::TransitionToScene(const std::string& sceneName, float transitionDuration)
{
    // Start the transition process
    BeginTransition(sceneName, transitionDuration);
}

void SceneManager::BeginTransition(const std::string& sceneName, float duration)
{
    // Set the target scene and transition settings
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

        // Simple fade-out and fade-in or other animation logic can go here

        if (transitionTime >= transitionDuration)
        {
            CompleteTransition();
        }
    }
}

void SceneManager::SetSceneLoadedCallback(std::function<void(const std::string&)> callback)
{
    onSceneLoadedCallback = callback;
}

void SceneManager::AddSceneToList(const std::string& sceneName)
{
    // Check if the scene is already in the scene list
    auto it = std::find(sceneList.begin(), sceneList.end(), sceneName);
    if (it == sceneList.end()) {
        // If it's not in the list, add it
        sceneList.push_back(sceneName);
        std::cout << "Added scene to list: " << sceneName << std::endl;
    }
    else {
        // If the scene is already in the list, inform the user
        std::cout << "Scene already exists in the list: " << sceneName << std::endl;
    }
}