#pragma once
class Level
{
public:

    // Functions designed to be overridden by derived classes later on for each Level
    virtual void LoadLevel() = 0;
    virtual void InitLevel() = 0;
    virtual void UpdateLevel(double deltaTime) = 0;
    virtual void FreeLevel() = 0;
    virtual void UnloadLevel() = 0;
    virtual ~Level() {}
    Level() {}
};