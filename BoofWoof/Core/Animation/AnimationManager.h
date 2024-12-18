#ifndef ANIMATION_MANAGER_H
#define ANIMATION_MANAGER_H

#include <assimp/scene.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

// Struct for keyframe data
struct KeyFrame {
    double time;
    aiVector3D position;
    aiQuaternion rotation;
    aiVector3D scale;
};

// Struct for animation channels
struct AnimationChannel {
    std::string nodeName;
    std::vector<KeyFrame> keyframes;
};

// Struct for animation
class Animation {
public:
    std::string name;
    double duration;      // Duration in ticks
    double ticksPerSecond;
    std::vector<AnimationChannel> channels;

    // Default constructor
    Animation() = default;

    Animation(const aiAnimation* assimpAnimation);
};

// Enum for animation types
enum class AnimationType {
    Idle = 0,
    Moving = 1,
    Action = 2
};

// Entity-specific animation state
struct EntityAnimationState {
    std::unordered_map<AnimationType, std::string> animations; // Mapped animations for each type
    std::string activeAnimation; // Currently playing animation
    double currentTime = 0.0;
    bool isPlaying = false;

    // Update entity animation state
    void Update(double deltaTime, const Animation& animation);

    // Set specific animation type
    void SetAnimation(AnimationType type, const std::string& animationName);
};

// Animation Manager
class AnimationManager {
public:
    // Load animations from a file
    void LoadAnimations(const std::string& filePath);

    // Check if an animation is loaded
    bool IsAnimationLoaded(const std::string& animationName) const;

    // Retrieve an animation by name
    const Animation& GetAnimation(const std::string& animationName) const;

    // Manage entity animations
    void AssignAnimation(const std::string& entityId, AnimationType type, const std::string& animationName);
    void PlayAnimation(const std::string& entityId);
    void StopAnimation(const std::string& entityId);

    // Update animations for all entities
    void Update(double deltaTime);

private:
    std::unordered_map<std::string, Animation> animations;            // All loaded animations
    std::unordered_map<std::string, EntityAnimationState> entityStates; // Entity-specific states
};

extern AnimationManager g_AnimationManager;

#endif // ANIMATION_MANAGER_H
