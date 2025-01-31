#pragma once

// Adjust includes to your project's structure:
#include "Level Manager/Level.h"            // Base class "Level"
#include "SceneManager/SceneManager.h"      // For g_SceneManager, if you have it
#include "Serialization/SerializationAsync.h" // If you use an async loader
#include "ECS/Coordinator.hpp"
#include "../BoofWoof/Core/AssetManager/FilePaths.h"
#include <string>
#include <iostream>

/**
 * @brief A special Level that displays a "Loading..." screen and asynchronously
 *        loads the next real scene in the background.
 *
 * Usage:
 *   1) Register "LoadingLevel" with the LevelManager:
 *      g_LevelManager.RegisterLevel("LoadingLevel", new LoadingLevel());
 *   2) When you want to load "Cutscene" or another big scene:
 *      auto* loading = dynamic_cast<LoadingLevel*>(g_LevelManager.GetLevel("LoadingLevel"));
 *      loading->m_NextScene = "Cutscene"; // or a file path
 *      g_LevelManager.SetNextLevel("LoadingLevel");   // Switch to the loading screen
 *   3) The LoadingLevel will show a simple UI or text while it calls SceneManager's
 *      BeginAsyncLoad() or chunk-based finalization. Once done, it transitions
 *      to the real scene.
 */
class LoadingLevel : public Level
{
public:
    /**
     * @brief The name or path of the next scene to load.
     *
     * In your engine, this could be the ID you pass to SetNextLevel(...)
     * or a file path used by SceneManager for asynchronous loading.
     */
    std::string m_NextScene;

private:
    bool m_LoadingStarted = false; ///< Tracks whether we've initiated async load
    bool m_LoadComplete = false; ///< Tracks whether load is done

public:
    LoadingLevel() = default;
    ~LoadingLevel() = default;

    /**
     * @brief Loads the "Loading Screen" scene.
     *
     * Typically, you create a small JSON scene (e.g. "LoadingScreen.json")
     * with minimal UI (text, spinner, etc.) so the user sees "Loading..."
     */
    void LoadLevel() override
    {
        // Load a minimal loading screen scene:
        // e.g. g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES + "/LoadingScreen.json");

        // If you have a small scene with "LoadingUI" or "Spinner" entities,
        // you can load them here. Something like:
        g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES + "/LoadingScreen.json");

        // Reset flags
        m_LoadingStarted = false;
        m_LoadComplete = false;

        std::cout << "[LoadingLevel] LoadLevel: Loaded the loading screen.\n";
    }

    /**
     * @brief Optionally init anything. Usually empty.
     */
    void InitLevel() override
    {
        // You might do nothing here. Or if you prefer to start the async load now,
        // you could. For example:
        // g_SceneManager.BeginAsyncLoad(m_NextScene);
        // m_LoadingStarted = true;
        std::cout << "[LoadingLevel] InitLevel: Optionally do something.\n";
    }

    /**
     * @brief Per-frame update: handle asynchronous loading logic,
     *        display "Loading..." UI, etc.
     *
     * @param deltaTime The time elapsed since last frame, in seconds.
     */
    void UpdateLevel(double deltaTime) override
    {
        if (!m_LoadingStarted)
        {
            std::cout << "[LoadingLevel] Update: Kick off async load for: " << m_NextScene << "\n";
            g_SceneManager.BeginAsyncLoad(FILEPATH_ASSET_SCENES + "/" + m_NextScene + ".json");
            m_LoadingStarted = true;
            return;
        }

        // Check for async load completion (assuming SceneManager sets this flag when done)
        if (!m_LoadComplete)
        {
            if (g_SceneManager.AsyncLoadIsComplete())  // <-- You might add such a function
            {
                m_LoadComplete = true;
            }
        }
        else
        {
            // Once done, transition to the next level
            std::cout << "[LoadingLevel] Loading done, transitioning to: " << m_NextScene << "\n";
            g_LevelManager.SetNextLevel(m_NextScene);
        }

        // (Optional) update any loading animations here.
    }


    /**
     * @brief Clean up any allocated resources for this level.
     */
    void FreeLevel() override
    {
        std::cout << "[LoadingLevel] FreeLevel.\n";
        // Typically empty or you can do cleanup. 
        // The engine might call UnloadLevel() next.
    }

    /**
     * @brief Reset entities or state used by the loading screen.
     */
    void UnloadLevel() override
    {
        // For example, destroy the "Loading..." UI entities:
        g_Coordinator.ResetEntities();

        // Reset states in case we come back to this loading screen in the future
        m_LoadingStarted = false;
        m_LoadComplete = false;

        std::cout << "[LoadingLevel] UnloadLevel: Cleanup done.\n";
    }
};
