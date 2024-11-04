#ifndef ANIMATIONMANAGER_H
#define ANIMATIONMANAGER_H

#include "pch.h"
#include "Animation.h"

class AnimationManager {
public:
    void AddAnimation(const std::string& name, const Animation& animation);
    void UpdateAnimations(float deltaTime);
    std::unordered_map<std::string, glm::mat4> GetBoneTransforms(const std::string& animationName) const;

    void PrintAnimationNames() const;

private:
    std::unordered_map<std::string, Animation> animations;
};

#endif // !ANIMATIONMANAGER_H
