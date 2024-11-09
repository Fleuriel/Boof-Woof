#include "pch.h"
#include "AnimationManager.h"

// Add an animation to the manager
void AnimationManager::AddAnimation(const std::string& name, const Animation& animation) {
    animations[name] = animation;
}

// Print the names of all animations
void AnimationManager::PrintAnimationNames() const {
    //std::cout << "Available Animations:\n";
    for (const auto& pair : animations) {
        (void)pair;
      //  std::cout << " - " << pair.first << std::endl;  // pair.first is the animation name
    }
}

void AnimationManager::UpdateAnimations(float deltaTime) {
    for (auto& [name, animation] : animations) {
        animation.Update(deltaTime);
    }
}

std::unordered_map<std::string, glm::mat4> AnimationManager::GetBoneTransforms(const std::string& animationName) const {
    auto it = animations.find(animationName); // Use find to check for existence
    if (it != animations.end()) {
        return it->second.GetBoneTransforms(); // Access the Animation object
    }
    std::cerr << "Animation not found: " << animationName << std::endl; // Optional error logging
    return {}; // Return an empty map if not found
}
