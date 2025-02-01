#pragma once

#include "Level Manager/Level.h"            // Base class "Level"
#include "SceneManager/SceneManager.h"      // For g_SceneManager, if you have it
#include "Serialization/SerializationAsync.h" // If you use an async loader
#include "ECS/Coordinator.hpp"
#include "../BoofWoof/Core/AssetManager/FilePaths.h"
#include <string>
#include <iostream>
#include <chrono>   // For steady_clock

// (Assuming that "Entity" and MAX_ENTITIES are defined in your ECS system.)
// For example, you might have: using Entity = unsigned int; and a constant MAX_ENTITIES.

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
    bool m_LoadingStarted = false;  ///< Tracks whether we've initiated async load
    bool m_LoadComplete = false;      ///< Tracks whether async load is done
    std::chrono::steady_clock::time_point m_StartTime; ///< Time point when loading started
    const double m_MinimumLoadingDuration = 2.0; ///< Minimum time (in seconds) for the loading screen

    // Cache the "Loading" entity that should rotate.
    // We use MAX_ENTITIES as an invalid/sentinel value.
    Entity m_LoadingEntity = MAX_ENTITIES;

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
        // Load the loading screen scene.
        g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES + "/LoadingScreen.json");

        // Reset flags, timer, and cached loading entity.
        m_LoadingStarted = false;
        m_LoadComplete = false;
        m_LoadingEntity = MAX_ENTITIES;

        // Optionally, print a debug message.
        // std::cout << "[LoadingLevel] LoadLevel: Loaded the loading screen.\n";
    }

    /**
     * @brief Optionally init anything. Usually empty.
     */
    void InitLevel() override
    {
        // Optionally, you can initiate further setup here.
        // std::cout << "[LoadingLevel] InitLevel: Optionally do something.\n";
    }

    /**
     * @brief Per-frame update: handle asynchronous loading logic,
     *        enforce minimum display duration, and update the "Loading" rotation.
     *
     * @param deltaTime The time elapsed since last frame, in seconds.
     */
    void UpdateLevel(double deltaTime) override
    {
        // (1) Start async load if not started yet.
        if (!m_LoadingStarted)
        {
            std::cout << "[LoadingLevel] Update: Kick off async load for: " << m_NextScene << "\n";
            g_SceneManager.BeginAsyncLoad(FILEPATH_ASSET_SCENES + "/" + m_NextScene + ".json");
            m_LoadingStarted = true;
            m_StartTime = std::chrono::steady_clock::now();
            return;
        }

        // (2) Check if asynchronous load is complete.
        if (!m_LoadComplete && g_SceneManager.AsyncLoadIsComplete())
        {
            m_LoadComplete = true;
            std::cout << "[LoadingLevel] Update: Async load complete.\n";
        }

        // (3) Compute elapsed time in seconds since async load started.
        auto now = std::chrono::steady_clock::now();
        double elapsedSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(now - m_StartTime).count();

        // (4) Only transition if async load is complete and the minimum duration has passed.
        if (m_LoadComplete && elapsedSeconds >= m_MinimumLoadingDuration)
        {
            std::cout << "[LoadingLevel] Loading done (elapsed " << elapsedSeconds << "s), transitioning to: " << m_NextScene << "\n";
            g_LevelManager.SetNextLevel(m_NextScene);
        }

        // (5) Animate the "Loading" entity:
        // First, if we haven't already found the "Loading" entity, search for it.
        if (m_LoadingEntity == MAX_ENTITIES)
        {
            auto entities = g_Coordinator.GetAliveEntitiesSet();
            for (auto entity : entities)
            {
                if (g_Coordinator.HaveComponent<MetadataComponent>(entity))
                {
                    auto& metadata = g_Coordinator.GetComponent<MetadataComponent>(entity);
                    if (metadata.GetName() == "Loading")
                    {
                        m_LoadingEntity = entity;
                        break;
                    }
                }
            }
        }

        if (m_LoadingEntity != MAX_ENTITIES && g_Coordinator.HaveComponent<UIComponent>(m_LoadingEntity))
        {
            auto& ui = g_Coordinator.GetComponent<UIComponent>(m_LoadingEntity);
            float rotationSpeed = 90.0f; // degrees per second
            float currentRotation = ui.get_rotation();
            ui.set_rotation(currentRotation - rotationSpeed * static_cast<float>(deltaTime));
        }
    }

    /**
     * @brief Clean up any allocated resources for this level.
     */
    void FreeLevel() override
    {
        std::cout << "[LoadingLevel] FreeLevel.\n";
    }

    /**
     * @brief Reset entities or state used by the loading screen.
     */
    void UnloadLevel() override
    {
        // Destroy the loading screen UI entities.
        g_Coordinator.ResetEntities();

        // Reset state in case we come back to this loading screen.
        m_LoadingStarted = false;
        m_LoadComplete = false;
        m_LoadingEntity = MAX_ENTITIES;

        std::cout << "[LoadingLevel] UnloadLevel: Cleanup done.\n";
    }
};
