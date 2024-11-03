
#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>      // For matrix math
#include <glm/gtx/quaternion.hpp> // For quaternions

// Assimp library
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

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
    Animation(const std::string& filePath);
    bool LoadAnimationData();            // Loads the animation from an .fbx file
    void Update(float deltaTime);        // Updates animation state
    std::unordered_map<std::string, glm::mat4> GetBoneTransforms() const; // Gets interpolated bone transforms

private:
    std::string filePath;
    float duration;
    float currentTime;
    std::unordered_map<std::string, BoneTransform> boneTransforms;

    void ProcessNode(const aiNode* node, const aiScene* scene);
    void ProcessAnimation(const aiAnimation* animation);

    glm::mat4 InterpolateKeyframes(const Keyframe& kf1, const Keyframe& kf2, float t);
};
