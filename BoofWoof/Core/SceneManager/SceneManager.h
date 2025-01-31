/**************************************************************************
 * @file SceneManager.cpp
 * @author 	Liu Xujie
 * @param DP email: l.xujie@digipen.edu [2203183]
 * @param Course: CS 3401
 * @param Course: Game Project 3
 * @date  09/25/2024 (25 SEPT 2024)
 * @brief
 *
 * This file contains the declaration of member functions of SceneManager
 * Class
 *************************************************************************/
#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#define g_SceneManager SceneManager::GetInstance()

#include "../Serialization/SerializationAsync.h" 

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

    // Called to begin asynchronous loading (instead of direct LoadScene)
    void BeginAsyncLoad(const std::string& filepath);

    // Called each frame to update loading logic, finalize if done
    void Update(float deltaTime);

    // Check if a transition is in progress
    bool IsTransitioning() const;

    // Optionally, register a callback for when a scene is fully loaded
    void SetSceneLoadedCallback(std::function<void(const std::string&)> callback);

    void AddSceneToList(const std::string& sceneName, const std::string& sceneGUID);
    inline void ClearSceneList() { sceneList.clear(); };

private:
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

    // The new async loader
    SerializationAsync m_AsyncLoader;

    // Are we currently loading a scene in the background?
    bool m_IsAsyncLoading = false;

    // Did we finish background loading but need to finalize ECS?
    bool m_NeedsFinalize = false;

    // We'll store the scene data once the thread is done
    SceneData m_PendingSceneData;

    // Optionally, are we chunking finalization?
    bool m_ChunkFinalize = false;
    size_t m_FinalizeIndex = 0;

    // A function to do chunk-based or immediate finalization
    void FinalizeSceneData(const SceneData& data);
    void FinalizeSceneDataChunked(const SceneData& data, size_t& index, int chunkSize);

};

#endif
