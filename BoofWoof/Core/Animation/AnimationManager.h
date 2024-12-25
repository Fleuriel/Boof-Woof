#ifndef ANIMATION_MANAGER_H
#define ANIMATION_MANAGER_H

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include "../Core/Animation/Animation.h"


class AnimationManager {
public:
    // Loads an animation from a file and associates it with a name
    bool LoadAnimation(const std::string& animationName, const std::string& filePath);

    // Checks if an animation is loaded
    bool IsAnimationLoaded(const std::string& animationName) const;

    // Retrieves an animation by name
    const Animation& GetAnimation(const std::string& animationName) const;

    // Updates all animation components
    void Update(double deltaTime);

private:
    std::unordered_map<std::string, Animation> animations; // Stores loaded animations
    std::vector<Entity*> entitiesWithAnimationComponents;  // Tracks entities with AnimationComponents
};

#endif // ANIMATION_MANAGER_H
