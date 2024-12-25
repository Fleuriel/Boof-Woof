#include "pch.h"

#include <iostream>
#include <stdexcept>
#include "Coordinator.hpp"
#include "../Utilities/Components/AnimationComponent.h"
#include "AnimationManager.h"

bool AnimationManager::LoadAnimation(const std::string& animationName, const std::string& filePath) {
    if (animations.find(animationName) != animations.end()) {
        std::cerr << "Animation already loaded: " << animationName << std::endl;
        return false;
    }

    Animation animation;
    if (!animation.LoadFromFile(filePath)) {
        std::cerr << "Failed to load animation: " << filePath << std::endl;
        return false;
    }

    animations[animationName] = std::move(animation);
    return true;
}

bool AnimationManager::IsAnimationLoaded(const std::string& animationName) const {
    return animations.find(animationName) != animations.end();
}

const Animation& AnimationManager::GetAnimation(const std::string& animationName) const {
    auto it = animations.find(animationName);
    if (it == animations.end()) {
        throw std::runtime_error("Animation not found: " + animationName);
    }
    return it->second;
}

void AnimationManager::Update(double deltaTime) {
    auto allEntities = g_Coordinator.GetAliveEntitiesSet();
    for (auto& entity : allEntities)
        if (g_Coordinator.HaveComponent<AnimationComponent>(entity))
            g_Coordinator.GetComponent<AnimationComponent>(entity).Update(deltaTime);
}
