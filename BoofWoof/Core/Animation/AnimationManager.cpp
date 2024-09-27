// AnimationManager.cpp
#include "AnimationManager.h"

void AnimationManager::AddAnimation(std::shared_ptr<IAnimation> animation) {
    animations.push_back(animation);
}

void AnimationManager::PlayAll() {
    for (auto& anim : animations) {
        anim->Play();
    }
}

void AnimationManager::PauseAll() {
    for (auto& anim : animations) {
        anim->Pause();
    }
}

void AnimationManager::StopAll() {
    for (auto& anim : animations) {
        anim->Stop();
    }
}
