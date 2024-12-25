#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

struct KeyFrame {
    float timeStamp;
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
};

struct BoneAnimation {
    std::string boneName;
    std::vector<KeyFrame> keyFrames;
};

struct Bone {
    std::string name;
    glm::mat4 offsetMatrix;
};

class Animation {
public:
    Animation();

    // Loads animation data from an Assimp scene
    bool LoadFromScene(const aiScene* scene);

    // Loads animation data from a file
    bool LoadFromFile(const std::string& filePath);

    float GetDuration() const;
    float GetTicksPerSecond() const;

    // Calculates bone transforms at a specific time in the animation
    const std::vector<glm::mat4>& GetBoneTransformsAtTime(float currentTime);

private:
    void CalculateBoneTransform(float animationTime, Bone& bone, const glm::mat4& parentTransform);

    float duration;
    float ticksPerSecond;
    std::unordered_map<std::string, BoneAnimation> boneAnimations;
    std::unordered_map<std::string, Bone> bones;
    std::vector<glm::mat4> boneTransforms;
};
