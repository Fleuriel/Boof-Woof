#ifndef ANIMATION_H
#define ANIMATION_H

#include "pch.h"
#include <assimp/scene.h>      // For aiScene, aiNode, aiAnimation, etc.
#include <assimp/Importer.hpp> // For Assimp::Importer
#include <assimp/postprocess.h> // For aiProcess flags (e.g., aiProcess_Triangulate)


// Structure for each keyframe in the animation
struct Keyframe {
    float time;
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
};

// Stores transform data for each bone
struct BoneTransform {
    std::string boneName;
    std::vector<Keyframe> keyframes;
};

// Animation class to load and handle animation
class Animation {
public:
    Animation() : filePath(""), duration(0.0f), currentTime(0.0f) {} // Default constructor
    Animation(const std::string& filePath);

    bool LoadAnimationData();            // Loads all animations from the file
    void Update(float deltaTime);        // Updates the animation state
    std::unordered_map<std::string, glm::mat4> GetBoneTransforms() const; // Retrieves the interpolated bone transforms

private:
    std::string filePath;
    float duration;                      // Duration of the longest animation
    float currentTime;                   // Current time in the animation
    std::unordered_map<std::string, BoneTransform> boneTransforms; // Stores bone transformations

    void ProcessNode(const aiNode* node, const aiScene* scene);
    void ProcessAnimation(const aiAnimation* animation);

    glm::mat4 InterpolateKeyframes(const Keyframe& kf1, const Keyframe& kf2, float t);
};

#endif // ANIMATION_H
