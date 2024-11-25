// AnimationComponent.h

#ifndef ANIMATIONCOMPONENT_H
#define ANIMATIONCOMPONENT_H


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

// Forward declarations
class Model;
class Mesh;
struct aiNode;
struct aiNodeAnim;
struct aiAnimation;

// Represents a single keyframe in the animation
struct KeyFrame {
    float timeStamp;
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;

    KeyFrame() : timeStamp(0.0f),
        position(0.0f),
        rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
        scale(1.0f) {}
};

// Stores information about a specific bone
struct BoneInfo {
    int id;                     // Index in final transformation matrices array
    std::string name;          // Name of the bone
    glm::mat4 offset;          // Offset matrix
    glm::mat4 finalTransform;  // Final transformation matrix
    std::vector<KeyFrame> keyFrames;  // Keyframes for this bone

    BoneInfo() : id(-1), offset(1.0f), finalTransform(1.0f) {}
};

// Enum for animation states
enum class AnimationState {
    IDLE,
    MOVING,
    ACTION1,
    ACTION2
};

// Represents a complete animation sequence
class AnimationClip {
public:
    std::string name;
    float duration;
    float ticksPerSecond;
    std::map<std::string, BoneInfo> bones;
    aiNode* rootNode;

    AnimationClip() : duration(0.0f), ticksPerSecond(24.0f), rootNode(nullptr) {}
    ~AnimationClip() = default;
};

// Main animation component class
class AnimationComponent {
public:
    AnimationComponent();
    ~AnimationComponent();

    // Core animation functions
    void update(float deltaTime);

    // Animation control functions
    void playState(AnimationState state, bool loop = true);
    void stop();
    void pause();
    void resume();
    void setSpeed(float speed) { m_playbackSpeed = speed; }
    void setTime(float time);
    void crossFadeTo(AnimationState state, float fadeTime);

    // Getters
    bool isPlaying() const { return m_isPlaying; }
    bool isLooping() const { return m_isLooping; }
    float getCurrentTime() const { return m_currentTime; }
    float getPlaybackSpeed() const { return m_playbackSpeed; }
    AnimationState getCurrentState() const { return m_currentState; }
    const std::vector<glm::mat4>& getFinalBoneMatrices() const { return m_finalBoneMatrices; }
    bool hasAnimation(AnimationState state) const;
    const std::string& getCurrentClipName() const;

    // Bone management
    int getBoneCount() const { return m_boneCount; }
    int getBoneId(const std::string& boneName) const;
    const glm::mat4& getBoneTransform(int boneId) const;
    const glm::mat4& getBoneTransform(const std::string& boneName) const;

private:
    // Internal animation processing functions
    void processNode(aiNode* node, const glm::mat4& parentTransform, AnimationClip* clip);
    void processAnimation(const aiNodeAnim* nodeAnim, AnimationClip* clip);
    void calculateBoneTransform(const std::string& nodeName, const glm::mat4& parentTransform);
    KeyFrame interpolateKeyFrames(float animationTime, const std::vector<KeyFrame>& keyFrames);

    glm::mat4 calculateTransformMatrix(const KeyFrame& frame) const {
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), frame.position);
        glm::mat4 rotation = glm::mat4_cast(frame.rotation);
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), frame.scale);
        return translation * rotation * scale;
    }

    // Interpolate between two matrices with proper decomposition
    glm::mat4 interpolateMatrix(const glm::mat4& m1, const glm::mat4& m2, float t) const {
        // Extract translation
        glm::vec3 pos1 = glm::vec3(m1[3]);
        glm::vec3 pos2 = glm::vec3(m2[3]);

        // Extract rotation
        glm::quat rot1 = glm::quat_cast(m1);
        glm::quat rot2 = glm::quat_cast(m2);

        // Extract scale
        glm::vec3 scale1 = glm::vec3(
            glm::length(glm::vec3(m1[0])),
            glm::length(glm::vec3(m1[1])),
            glm::length(glm::vec3(m1[2]))
        );
        glm::vec3 scale2 = glm::vec3(
            glm::length(glm::vec3(m2[0])),
            glm::length(glm::vec3(m2[1])),
            glm::length(glm::vec3(m2[2]))
        );

        // Interpolate components
        glm::vec3 pos = glm::mix(pos1, pos2, t);
        glm::quat rot = glm::slerp(rot1, rot2, t);
        glm::vec3 scale = glm::mix(scale1, scale2, t);

        // Reconstruct matrix
        glm::mat4 result(1.0f);
        result = glm::translate(result, pos);
        result = result * glm::mat4_cast(rot);
        result = glm::scale(result, scale);

        return result;
    }

    // Animation state
    std::map<AnimationState, AnimationClip*> m_animations;
    AnimationClip* m_currentClip;
    AnimationClip* m_nextClip;  // For crossfading
    AnimationState m_currentState;
    AnimationState m_nextState;  // For crossfading
    float m_fadeTime;
    float m_currentFadeTime;

    // Playback state
    float m_currentTime;
    float m_playbackSpeed;
    bool m_isPlaying;
    bool m_isLooping;

    // Bone data
    std::vector<glm::mat4> m_finalBoneMatrices;
    std::map<std::string, int> m_boneMapping;
    int m_boneCount;

    // Constants
    static const int MAX_BONES = 100;
    static const float ANIMATION_BLEND_TIME;
};

// Helper functions for state conversion
namespace AnimationUtils {
    inline const char* getStateName(AnimationState state) {
        switch (state) {
        case AnimationState::IDLE: return "Idle";
        case AnimationState::MOVING: return "Moving";
        case AnimationState::ACTION1: return "Action1";
        case AnimationState::ACTION2: return "Action2";
        default: return "Unknown";
        }
    }

    inline AnimationState getStateFromString(const std::string& stateName) {
        if (stateName == "Idle") return AnimationState::IDLE;
        if (stateName == "Moving") return AnimationState::MOVING;
        if (stateName == "Action1") return AnimationState::ACTION1;
        if (stateName == "Action2") return AnimationState::ACTION2;
        return AnimationState::IDLE; // Default to IDLE if unknown
    }
}

#endif // !ANIMATIONCOMPONENT_H