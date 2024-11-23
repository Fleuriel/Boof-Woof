// AnimationComponent.h
#pragma once
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

// Represents a complete animation sequence
class AnimationClip {
public:
    std::string name;
    float duration;
    float ticksPerSecond;
    std::map<std::string, BoneInfo> bones;
    aiNode* rootNode;

    AnimationClip() : duration(0.0f), ticksPerSecond(24.0f), rootNode(nullptr) {}
};

// Main animation component class
class AnimationComponent {
public:
    AnimationComponent();
    ~AnimationComponent();

    // Core animation functions
    void loadAnimation(const std::string& filename);
    //void loadAnimation(const aiScene* scene, const std::string& animationName);
    void update(float deltaTime);

    // Animation control functions
    void play(const std::string& clipName, bool loop = true);
    void stop();
    void pause();
    void resume();
    void setSpeed(float speed) { m_playbackSpeed = speed; }
    void setTime(float time);
    void crossFadeTo(const std::string& clipName, float fadeTime);

    // Getters
    bool isPlaying() const { return m_isPlaying; }
    bool isLooping() const { return m_isLooping; }
    float getCurrentTime() const { return m_currentTime; }
    float getPlaybackSpeed() const { return m_playbackSpeed; }
    const std::string& getCurrentClipName() const;
    const std::vector<glm::mat4>& getFinalBoneMatrices() const { return m_finalBoneMatrices; }

    // Bone management
    int getBoneCount() const { return m_boneCount; }
    int getBoneId(const std::string& boneName) const;
    const glm::mat4& getBoneTransform(int boneId) const;
    const glm::mat4& getBoneTransform(const std::string& boneName) const;

private:
    // Internal animation processing functions
    void processNode(aiNode* node, const glm::mat4& parentTransform, AnimationClip* clip);
    void processBone(const aiBone* bone);
    void processAnimation(const aiNodeAnim* nodeAnim, AnimationClip* clip);
    void extractBoneKeyFrames(const aiNodeAnim* nodeAnim, std::vector<KeyFrame>& keyFrames);
    void calculateBoneTransform(const std::string& nodeName, const glm::mat4& parentTransform);
    KeyFrame interpolateKeyFrames(float animationTime, const std::vector<KeyFrame>& keyFrames);

    // Animation state
//    std::map<std::string, std::unique_ptr<AnimationClip>> m_animations;
    AnimationClip* m_currentClip;
    AnimationClip* m_nextClip;  // For crossfading
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