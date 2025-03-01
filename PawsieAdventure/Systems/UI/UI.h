#pragma once
#include <EngineCore.h>

class UI
{
public:
    void OnInitialize();
    void OnUpdate(double deltaTime);
    void OnShutdown();
    void Sniff(const std::vector<Entity>& particles, float dt);

private:
    Entity P1{}, P2{}, P3{}, P4{}, P5{};
    Entity CDSniff{};

    // Cooldown and Animation State (Must Persist Across Scenes)
    mutable bool canPressE = true;
    mutable float particleTimer = 0.0f;
    mutable float sniffAnimationTimer = 0.0f;

    // New Variables to Store State Before Scene Unloads
    static bool savedE;
    static float savedParticleTimer;
    static float savedSniffAnimationTimer;
    static int savedCurrCol;

protected:
    std::vector<Entity> storage;
};


extern UI g_UI;