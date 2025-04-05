#pragma once
#include <EngineCore.h>

class UI
{
public:
    void OnInitialize();
    void OnUpdate(double deltaTime);
    void OnShutdown();
    void Sniff(const std::vector<Entity>& particles, int goalnode, float dt, int startnode = -1);
    void Sprint(float dt);

private:
    Entity P1{}, P2{}, P3{}, P4{}, P5{};
    Entity CDSniff{}, Sniffa{}, Sprinto{};
    std::vector<Entity> pellets;

    // Cooldown and Animation State (Must Persist Across Scenes)
    mutable bool canPressE = true;
    mutable float particleTimer = 0.0f;
    mutable float sniffAnimationTimer = 0.0f;

    mutable int staminaIndex = 4;
    mutable float sprintTimer = 0.0f;
    mutable float replenishTimer = 0.0f;
    mutable float pelletOpacities[5] = { 1.0f,1.0f,1.0f,1.0f,1.0f };

    // Sprint states
    static int savedStaminaIndex;
    static float savedSprintTimer;
    static float savedReplenishTimer;
    static float savedPelletOpacities[5];

    // Sniffing states
    static bool savedE;
    static float savedParticleTimer;
    static float savedSniffAnimationTimer;
    static int savedCurrCol;

public:
    static bool isExhausted;
    static bool isStunned;
    bool finishCaged{ false };
    bool inStartingRoom{ false };

protected:
    std::vector<Entity> storage;
};


extern UI g_UI;