#ifndef ANIMATION_MANAGER_H
#define ANIMATION_MANAGER_H

#include <assimp/scene.h>
#include <string>
#include <unordered_map>
#include <vector>

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

// Class for animation
class Animation {
public:
    std::string name;
    double duration;      // Duration in ticks
    double ticksPerSecond;
    std::vector<AnimationChannel> channels;

    // Default constructor
    Animation() = default;

    // Constructor to initialize from Assimp animation
    Animation(const aiAnimation* assimpAnimation);
};

// Enum for animation types
enum class AnimationType {
    Idle,
    Moving,
    Action
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

    // Assign an animation to an entity
    void AssignAnimation(const std::string& entityId, AnimationType type, const std::string& animationName);

    // Play animation for an entity
    void PlayAnimation(const std::string& entityId);

    // Stop animation for an entity
    void StopAnimation(const std::string& entityId);

    // Update animations for all entities
    void Update(double deltaTime);

    // Get the index of an animation by its name
    int GetAnimationIndex(const std::string& animationName) const;

    std::vector<std::string> animationNames;							//Container to store animation file names

private:
    std::unordered_map<std::string, Animation> animations;              // All loaded animations
    std::unordered_map<std::string, EntityAnimationState> entityStates; // Entity-specific states
};


extern AnimationManager g_AnimationManager;
#endif // ANIMATION_MANAGER_H
