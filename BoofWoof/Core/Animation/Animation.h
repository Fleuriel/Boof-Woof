#ifndef ANIMATION
#define ANIMATION

#include <vector>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <iostream>
#include "../Core/Graphics/Mesh.h"


struct AnimVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    int boneIDs[4] = { -1, -1, -1, -1 };
    float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
};

struct BoneInfo {
    glm::mat4 offsetMatrix;
};

struct KeyFrame {
    float timeStamp;
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
};

struct BoneAnimation {
    std::string boneName;
    std::vector<KeyFrame> keyFrames;
    glm::mat4 currentTransform = glm::mat4(1.0f); // Default to identity matrix
};



struct MeshData {
    std::vector<Vertex> vertices;  // Stores all vertex data
    std::vector<unsigned int> indices;  // Stores indices for rendering
};

class Animation {
public:
    bool LoadModel(const std::string& filePath);
    bool LoadAnimation(const std::string& filePath);
    float GetDuration() const { return duration; };
    float GetTicksPerSecond() const { return ticksPerSecond; };
    const std::vector<glm::mat4>& GetBoneTransformsAtTime(float currentTime);
    const std::vector<aiMesh*>& GetMeshes() const { return meshes; }
    const std::unordered_map<std::string, BoneInfo>& GetBoneInfoMap() const { return boneInfoMap; }
    void ProcessMesh(const aiScene* scene);

    void UpdateAnimation(float deltaTime);


    void BuildBoneHierarchy(const aiNode* node, const std::string& parentName = "");

//    std::unordered_map<std::string, BoneAnimation> boneAnimations;
    std::vector<Mesh> meshDataMesh;
    std::vector<aiMesh*> meshes;
private:
    void ProcessMesh(aiMesh* mesh, std::vector<AnimVertex>& vertices, std::vector<unsigned int>& indices);
    void ExtractBoneWeights(aiMesh* mesh, std::vector<AnimVertex>& vertices);
    void ProcessAnimation(const aiScene* scene);
    void CalculateBoneTransform(float animationTime, const std::string& boneName, glm::mat4 parentTransform);
    glm::vec3 InterpolatePosition(float animationTime, const BoneAnimation& boneAnim);
    glm::quat InterpolateRotation(float animationTime, const BoneAnimation& boneAnim);
    glm::vec3 InterpolateScale(float animationTime, const BoneAnimation& boneAnim);

    std::unordered_map<std::string, std::string> boneHierarchy;
    std::unordered_map<std::string, int> boneMapping;  // Maps bone names to their index
    std::vector<glm::mat4> boneTransforms;            // Stores final bone matrices
    std::unordered_map<std::string, BoneInfo> boneInfoMap;
//    std::vector<glm::mat4> boneTransforms;
    float duration = 10.0f;
    float ticksPerSecond = 25.0f;
    float currentTime = 0.0f;  // <--- Declare currentTime here
    int boneCounter = 0;
};

#endif