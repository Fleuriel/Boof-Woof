#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include "Level.h"

#define g_LevelManager LevelManager::GetInstance()

class LevelManager 
{
public:

    static LevelManager& GetInstance();

    void RegisterLevel(const std::string& levelName, Level* level);     // Register all levels to be used
    void Initialize(const std::string& startingLevelName);              // Initialize level manager by setting first level
    void LoadLevel();                                                   // Loads current level
    void InitLevel();                                                   // Initialize the current level
    void UpdateLevel(double deltaTime);                                 // Update current level
    void FreeLevel();                                                   // Free current Level
    void UnloadLevel();                                                 // Unload current level
    ~LevelManager();                                                    // Destructor

    // Setting the levels
    void SetCurrentLevel(const std::string& levelName);
    void SetNextLevel(const std::string& levelName);
    void SetPreviousLevel(const std::string& levelName);
    void SetCurrentLevel(Level* level); 
    void SetNextLevel(Level* level); 
    void SetPreviousLevel(Level* level); 

    // Get the levels itself
    Level* GetCurrentLevel(); 
    Level* GetNextLevel();
    Level* GetPreviousLevel();

    // Get names of the levels
    std::string GetCurrentLevelName() const;
    std::string GetNextLevelName() const;   
    std::string GetPreviousLevelName() const; 

private:
    std::unordered_map<std::string, Level*> m_levels;
    Level* m_currentLevel = nullptr;
    Level* m_nextLevel = nullptr;
    Level* m_previousLevel = nullptr;
    inline static std::unique_ptr<LevelManager> m_Instance{};
};