#pragma once

#include <thread>
#include <atomic>
#include <mutex>
#include <string>
#include "../SceneManager/SceneData.h"

/**
 * @class SerializationAsync
 * @brief Handles asynchronous loading (parsing) of a scene JSON file.
 *
 * Usage:
 *   1. Call `BeginLoad(filepath)` to start a background thread.
 *   2. In your main loop, poll `IsDone()` to see if it’s finished.
 *   3. Once done, call `GetLoadedData()` to retrieve the JSON data,
 *      then finalize the ECS entities on the main thread.
 */
class SerializationAsync
{
public:
    /**
     * @brief Start loading/parsing a scene file in a background thread.
     * @param filepath The full or relative path to the scene JSON file.
     */
    void BeginLoad(const std::string& filepath);

    /**
     * @return True if the background thread has finished (success or fail).
     */
    bool IsDone() const;

    /**
     * @brief Get the loaded SceneData. Only valid after IsDone() is true.
     * @return A copy of the scene data parsed from the JSON.
     */
    SceneData GetLoadedData();
private:
    // The worker thread
    std::thread m_Thread;

    // Atomic flags to track loading state
    std::atomic<bool> m_Loading{ false };
    std::atomic<bool> m_Done{ false };

    // Protect shared data
    mutable std::mutex m_Mutex;

    // Holds the result (parsed JSON) from the background thread
    SceneData m_SceneData;

    // The actual worker function
    void ThreadLoadFunction(const std::string& filepath);
};
