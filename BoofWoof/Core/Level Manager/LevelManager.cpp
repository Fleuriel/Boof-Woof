#include "LevelManager.h"

LevelManager& LevelManager::GetInstance()
{
	static LevelManager instance;
	return instance;
}

void LevelManager::RegisterLevel(const std::string& levelName, Level* level)
{
	m_levels[levelName] = level;
}

void LevelManager::Initialize(const std::string& startingLevelName)
{
	SetCurrentLevel(startingLevelName);
}

void LevelManager::LoadLevel()
{
	m_currentLevel->LoadLevel();
}

void LevelManager::InitLevel()
{
	m_currentLevel->InitLevel();
}

void LevelManager::UpdateLevel(double deltaTime)
{
	m_currentLevel->UpdateLevel(deltaTime);
}

void LevelManager::FreeLevel()
{
	m_currentLevel->FreeLevel();
}

void LevelManager::UnloadLevel()
{
	m_currentLevel->UnloadLevel();
}

void LevelManager::SetCurrentLevel(const std::string& levelName)
{
	auto it = m_levels.find(levelName);
	if (it != m_levels.end())
	{
		m_currentLevel = it->second;
	}
}

void LevelManager::SetNextLevel(const std::string& levelName)
{
	auto it = m_levels.find(levelName);
	if (it != m_levels.end())
	{
		m_nextLevel = it->second;
	}
}

void LevelManager::SetPreviousLevel(const std::string& levelName)
{
	auto it = m_levels.find(levelName);
	if (it != m_levels.end()) {
		m_previousLevel = it->second;
	}
}

void LevelManager::SetCurrentLevel(Level* level) 
{
	m_currentLevel = level;
}

void LevelManager::SetNextLevel(Level* level) 
{
	m_nextLevel = level;
}

void LevelManager::SetPreviousLevel(Level* level) 
{
	m_previousLevel = level;
}

Level* LevelManager::GetCurrentLevel() 
{
	return m_currentLevel;
}

Level* LevelManager::GetNextLevel() 
{
	return m_nextLevel;
}

Level* LevelManager::GetPreviousLevel() 
{
	return m_previousLevel;
}

std::string LevelManager::GetCurrentLevelName() const
{
	if (m_currentLevel)
	{
		for (const auto& pair : m_levels)
		{
			if (pair.second == m_currentLevel)
			{
				return pair.first;
			}
		}
	}
	return "";
}

std::string LevelManager::GetNextLevelName() const
{
	if (m_nextLevel)
	{
		for (const auto& pair : m_levels)
		{
			if (pair.second == m_nextLevel)
			{
				return pair.first;
			}
		}
	}
	return "";
}

std::string LevelManager::GetPreviousLevelName() const 
{
	if (m_previousLevel) {
		for (const auto& pair : m_levels) {
			if (pair.second == m_previousLevel) {
				return pair.first;
			}
		}
	}
	return "";
}

// Destructor
LevelManager::~LevelManager() 
{
	// Clean up allocated Level objects
	for (auto& pair : m_levels) {
		delete pair.second;
	}
}