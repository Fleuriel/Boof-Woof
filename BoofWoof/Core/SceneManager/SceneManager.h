#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <vector>
#include <string>
#include <unordered_map> // To map GUIDs to scene file paths
#include <functional> // For callbacks
#include <chrono>     // For timing transitions

#define g_SceneManager SceneManager::GetInstance()

class SceneManager
{
public:
    static SceneManager& GetInstance()
    {
        static SceneManager instance{};
        return instance;
    }

    SceneManager();
    ~SceneManager();

    // Create a new empty scene
    void CreateScene(const std::string& sceneName);

    // Load a scene from a file
    bool LoadScene(const std::string& filepath);

    // Load a scene based on its GUID
    bool LoadSceneByGUID(const std::string& sceneGUID);

    // Save the current scene to a file
    bool SaveScene(const std::string& filepath);

    // Get the current scene name
    std::string GetCurrentSceneName() const;

    // Get all the loaded scenes
    const std::vector<std::pair<std::string, std::string>>& GetAllScenes() const;

    // Reset the scene (clear all entities)
    void ResetScene();

    // Transition to another scene
    void TransitionToScene(const std::string& sceneName, float transitionDuration = 1.0f);

    // Update scene manager for transitions
    void Update(float deltaTime);

    // Check if a transition is in progress
    bool IsTransitioning() const;

    // Optionally, register a callback for when a scene is fully loaded
    void SetSceneLoadedCallback(std::function<void(const std::string&)> callback);

    void AddSceneToList(const std::string& sceneName, const std::string& sceneGUID);
    inline void ClearSceneList() { sceneList.clear(); };

private:
    // Internally handle loading scenes with transition
    void HandleSceneLoading();

    // Timing for transitions
    bool transitioning;
    float transitionTime;
    float transitionDuration;
    std::string targetScene;
    std::string currentScene;

    // Updated to store a pair of scene file paths and GUIDs
    std::vector<std::pair<std::string, std::string>> sceneList; // Pair of (GUID, scene file path)

    // Map GUIDs to file paths for easy lookup
    std::unordered_map<std::string, std::string> guidToFileMap;

    std::function<void(const std::string&)> onSceneLoadedCallback;

    // Helper functions to control transitions
    void BeginTransition(const std::string& sceneName, float duration);
    void CompleteTransition();
};

#endif
