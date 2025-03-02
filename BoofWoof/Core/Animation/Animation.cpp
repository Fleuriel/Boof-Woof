#include "pch.h"
#include "Animation.h"
#include "ResourceManager/ResourceManager.h"

bool Animation::LoadModel(const std::string& filePath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Error: Assimp failed to load model: " << importer.GetErrorString() << std::endl;
        return false;
    }
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        meshes.push_back(scene->mMeshes[i]);
    }
    return true;
}

void Animation::ProcessMesh(aiMesh* mesh, std::vector<AnimVertex>& vertices, std::vector<unsigned int>& indices) {
    // Process vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        AnimVertex AnimVertex;
        AnimVertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        AnimVertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        if (mesh->mTextureCoords[0]) {
            AnimVertex.texCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        else {
            AnimVertex.texCoords = glm::vec2(0.0f, 0.0f);
        }
        vertices.push_back(AnimVertex);
    }

    // Process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Extract bone weights
    ExtractBoneWeights(mesh, vertices);
}

void Animation::ExtractBoneWeights(aiMesh* mesh, std::vector<AnimVertex>& vertices) {
    for (unsigned int i = 0; i < mesh->mNumBones; ++i) {
        std::string boneName = mesh->mBones[i]->mName.C_Str();
        if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
            boneInfoMap[boneName] = { glm::mat4(1.0f) };
            boneCounter++;
        }
        for (unsigned int j = 0; j < mesh->mBones[i]->mNumWeights; ++j) {
            unsigned int AnimVertexID = mesh->mBones[i]->mWeights[j].mVertexId;
            float weight = mesh->mBones[i]->mWeights[j].mWeight;
            for (int k = 0; k < 4; ++k) {
                if (vertices[AnimVertexID].boneIDs[k] == -1) {
                    vertices[AnimVertexID].boneIDs[k] = i;
                    vertices[AnimVertexID].weights[k] = weight;
                    break;
                }
            }
        }
    }
}

void Animation::BuildBoneHierarchy(const aiNode* node, const std::string& parentName) {
    std::string nodeName = node->mName.C_Str();

    // Set parent-child relationship
    boneHierarchy[nodeName] = parentName;

    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        BuildBoneHierarchy(node->mChildren[i], nodeName);
    }
}


bool Animation::LoadAnimation(const std::string& filePath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate);
    if (!scene || !scene->HasAnimations()) return false;
    ProcessAnimation(scene);
    ProcessMesh(scene);

    std::cout << "Load Animation: \t" << g_ResourceManager.boneAnimations.size() << '\n';

    Model model;
    model.meshes = meshDataMesh;
	model.name = "animation";


    std::cout << '\t' << meshDataMesh.size() << '\n';

    std::cout << "it worked?\n";
	g_ResourceManager.SetModelMap("animation", model);

    g_ResourceManager.AddModelBinary(model.name);
    std::cout << "it worked?\n";

    return true;
}


//
//void Animation::ProcessAnimation(const aiScene* scene) {
//    if (!scene->HasAnimations()) return;
//    const aiAnimation* aiAnim = scene->mAnimations[0];  // Assuming only one animation
//    duration = static_cast<float>(aiAnim->mDuration);
//    ticksPerSecond = static_cast<float>(aiAnim->mTicksPerSecond ? aiAnim->mTicksPerSecond : 25.0f);
//
//    // Process each bone animation
//    for (unsigned int i = 0; i < aiAnim->mNumChannels; ++i) {
//        aiNodeAnim* channel = aiAnim->mChannels[i];
//        BoneAnimation boneAnim;
//        boneAnim.boneName = channel->mNodeName.C_Str();
//
//        std::cout << "Bone Name: " << boneAnim.boneName << '\n';
//        // Process keyframes for position, rotation, and scale
//        for (unsigned int j = 0; j < channel->mNumPositionKeys; ++j) {
//            KeyFrame keyFrame;
//            keyFrame.timeStamp = static_cast<float>(channel->mPositionKeys[j].mTime);
//            keyFrame.position = glm::vec3(channel->mPositionKeys[j].mValue.x,
//                channel->mPositionKeys[j].mValue.y,
//                channel->mPositionKeys[j].mValue.z);
//            boneAnim.keyFrames.push_back(keyFrame);
//        }
//
//        for (unsigned int j = 0; j < channel->mNumRotationKeys; ++j) {
//            boneAnim.keyFrames[j].rotation = glm::quat(channel->mRotationKeys[j].mValue.w,
//                channel->mRotationKeys[j].mValue.x,
//                channel->mRotationKeys[j].mValue.y,
//                channel->mRotationKeys[j].mValue.z);
//        }
//
//        for (unsigned int j = 0; j < channel->mNumScalingKeys; ++j) {
//            boneAnim.keyFrames[j].scale = glm::vec3(channel->mScalingKeys[j].mValue.x,
//                channel->mScalingKeys[j].mValue.y,
//                channel->mScalingKeys[j].mValue.z);
//        }
//
//
//
//        std::cout << "Processing animation: " << aiAnim->mName.C_Str() << std::endl;
//        std::cout << "Number of animation channels: " << aiAnim->mNumChannels << std::endl;
//
//        std::cout << "Bone: " << channel->mNodeName.C_Str()
//            << " Position keys: " << channel->mNumPositionKeys
//            << " Rotation keys: " << channel->mNumRotationKeys
//            << " Scale keys: " << channel->mNumScalingKeys << std::endl;
//
//        std::cout << '\n';
//
//
//
//        g_ResourceManager.boneAnimations[boneAnim.boneName] = boneAnim;
//
//        std::cout << "Data of Bones\n";
//        std::cout << g_ResourceManager.boneAnimations[boneAnim.boneName].boneName << '\n';
//        std::cout << g_ResourceManager.boneAnimations[boneAnim.boneName].currentTransform.length() << '\n';
//        std::cout << g_ResourceManager.boneAnimations[boneAnim.boneName].keyFrames.size() << '\n';
//
//        // Store bone mapping
//        if (boneMapping.find(boneAnim.boneName) == boneMapping.end()) {
//            int newIndex = static_cast<int>(boneMapping.size());
//            boneMapping[boneAnim.boneName] = newIndex;
//
//        }
//    }
//
//    std::cout<< "BONE ANIMATION DE SIZE\t" << g_ResourceManager.boneAnimations.size() << '\n';
//
//
//    std::cout << "Bone Animation Data\n\n\n\n";
//    for (const auto& boneAnim : g_ResourceManager.boneAnimations) {
//        std::cout << "Bone: " << boneAnim.first << " has " << boneAnim.second.keyFrames.size() << " keyframes\n";
//		g_ResourceManager.boneNames.push_back(boneAnim.first);
//    }
//    
//    // Build the bone hierarchy after processing animation data
//    BuildBoneHierarchy(scene->mRootNode);
//    std::cout << "BONE ANIMATION DE SIZE\t" << g_ResourceManager.boneAnimations.size() << '\n';
//}

void Animation::ProcessAnimation(const aiScene* scene) {
    if (!scene->HasAnimations()) return;
    const aiAnimation* aiAnim = scene->mAnimations[0];  // Assuming only one animation
    duration = static_cast<float>(aiAnim->mDuration);
    ticksPerSecond = static_cast<float>(aiAnim->mTicksPerSecond ? aiAnim->mTicksPerSecond : 25.0f);

    for (unsigned int i = 0; i < aiAnim->mNumChannels; ++i) {
        aiNodeAnim* channel = aiAnim->mChannels[i];
        BoneAnimation boneAnim;
        boneAnim.boneName = channel->mNodeName.C_Str();

        std::map<float, KeyFrame> keyFrameMap;

        // Process position keys
        for (unsigned int j = 0; j < channel->mNumPositionKeys; ++j) {
            float time = static_cast<float>(channel->mPositionKeys[j].mTime);
            keyFrameMap[time].timeStamp = time;
            keyFrameMap[time].position = glm::vec3(
                channel->mPositionKeys[j].mValue.x,
                channel->mPositionKeys[j].mValue.y,
                channel->mPositionKeys[j].mValue.z
            );
        }

        // Process rotation keys
        for (unsigned int j = 0; j < channel->mNumRotationKeys; ++j) {
            float time = static_cast<float>(channel->mRotationKeys[j].mTime);
            keyFrameMap[time].timeStamp = time;
            keyFrameMap[time].rotation = glm::quat(
                channel->mRotationKeys[j].mValue.w,
                channel->mRotationKeys[j].mValue.x,
                channel->mRotationKeys[j].mValue.y,
                channel->mRotationKeys[j].mValue.z
            );
        }

        // Process scaling keys
        for (unsigned int j = 0; j < channel->mNumScalingKeys; ++j) {
            float time = static_cast<float>(channel->mScalingKeys[j].mTime);
            keyFrameMap[time].timeStamp = time;
            keyFrameMap[time].scale = glm::vec3(
                channel->mScalingKeys[j].mValue.x,
                channel->mScalingKeys[j].mValue.y,
                channel->mScalingKeys[j].mValue.z
            );
        }

        for (auto& [time, keyFrame] : keyFrameMap) {
            boneAnim.keyFrames.push_back(keyFrame);
        }

        g_ResourceManager.boneAnimations[boneAnim.boneName] = boneAnim;

        // Store bone mapping
        if (boneMapping.find(boneAnim.boneName) == boneMapping.end()) {
            int newIndex = static_cast<int>(boneMapping.size());
            boneMapping[boneAnim.boneName] = newIndex;
        }
    }

    BuildBoneHierarchy(scene->mRootNode);
}

void AddBoneData(Vertex& vertex, int boneIndex, float weight) {
    float totalWeight = 0.0f;
    bool added = false;

    for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
        if (vertex.m_BoneIDs[i] == -1) {  // Empty slot
            vertex.m_BoneIDs[i] = boneIndex;
            vertex.m_Weights[i] = weight;
            added = true;
            break;
        }
    }

    // If all slots are filled, replace the lowest weight if the new one is larger
    if (!added) {
        int minIndex = 0;
        float minWeight = vertex.m_Weights[0];

        for (int i = 1; i < MAX_BONE_INFLUENCE; ++i) {
            if (vertex.m_Weights[i] < minWeight) {
                minWeight = vertex.m_Weights[i];
                minIndex = i;
            }
        }

        if (weight > minWeight) {
            vertex.m_BoneIDs[minIndex] = boneIndex;
            vertex.m_Weights[minIndex] = weight;
        }
    }

    // Normalize weights to sum to 1.0
    for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
        totalWeight += vertex.m_Weights[i];
    }
    if (totalWeight > 0.0f) {
        for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
            vertex.m_Weights[i] /= totalWeight;
        }
    }
}

//
//void AddBoneData(Vertex& vertex, int boneIndex, float weight) {
//    for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
//        if (vertex.m_BoneIDs[i] == -1) {  // Found an empty slot
//            vertex.m_BoneIDs[i] = boneIndex;
//            vertex.m_Weights[i] = weight;
//            return;
//        }
//    }
//    // Optionally: If all slots are filled, you can choose to ignore the extra influence
//}


glm::mat4 AssimpToGLMMatrix(const aiMatrix4x4& from) {
    glm::mat4 to;

    to[0][0] = from.a1; to[0][1] = from.b1; to[0][2] = from.c1; to[0][3] = from.d1;
    to[1][0] = from.a2; to[1][1] = from.b2; to[1][2] = from.c2; to[1][3] = from.d2;
    to[2][0] = from.a3; to[2][1] = from.b3; to[2][2] = from.c3; to[2][3] = from.d3;
    to[3][0] = from.a4; to[3][1] = from.b4; to[3][2] = from.c4; to[3][3] = from.d4;

    return to;
}


void Animation::ProcessMesh(const aiScene* scene) {
    meshDataMesh.clear();

    for (unsigned int m = 0; m < scene->mNumMeshes; ++m) {
        aiMesh* mesh = scene->mMeshes[m];
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        for (unsigned int j = 0; j < mesh->mNumVertices; ++j) {
            Vertex vertex;
            vertex.Position = glm::vec3(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z);
            vertex.Normal = glm::vec3(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z);
            vertex.TexCoords = mesh->mTextureCoords[0] ? glm::vec2(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y) : glm::vec2(0.0f);
            vertices.push_back(vertex);
        }

        if (mesh->HasBones()) {
            for (unsigned int i = 0; i < mesh->mNumBones; ++i) {
                aiBone* bone = mesh->mBones[i];
                std::string boneName(bone->mName.C_Str());

                if (boneMapping.find(boneName) == boneMapping.end()) {
                    int newIndex = static_cast<int>(boneMapping.size());
                    boneMapping[boneName] = newIndex;
                    boneOffsetMatrix[boneName] = AssimpToGLMMatrix(bone->mOffsetMatrix);
                }
                int boneIndex = boneMapping[boneName];

                for (unsigned int k = 0; k < bone->mNumWeights; ++k) {
                    aiVertexWeight weight = bone->mWeights[k];
                    AddBoneData(vertices[weight.mVertexId], boneIndex, weight.mWeight);
                }
            }
        }

        for (unsigned int j = 0; j < mesh->mNumFaces; ++j) {
            aiFace face = mesh->mFaces[j];
            for (unsigned int k = 0; k < face.mNumIndices; ++k) {
                indices.push_back(face.mIndices[k]);
            }
        }

        unsigned int VAO, VBO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        glEnableVertexAttribArray(2);

        glVertexAttribIPointer(3, MAX_BONE_INFLUENCE, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(4, MAX_BONE_INFLUENCE, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
        glEnableVertexAttribArray(4);

        glBindVertexArray(0);
    }
}



//
//void Animation::ProcessMesh(const aiScene* scene) {
//    meshDataMesh.clear();  // Clear previous meshes (if any)
//
//    for (unsigned int m = 0; m < scene->mNumMeshes; ++m) {
//        aiMesh* mesh = scene->mMeshes[m];
//
//        // Temporary storage for mesh data
//        std::vector<Vertex> vertices;
//        std::vector<unsigned int> indices;
//        std::vector<Texture> textures;
//
//        // --- Load vertex data ---
//        for (unsigned int j = 0; j < mesh->mNumVertices; ++j) {
//            Vertex vertex;
//
//            // Position
//            vertex.Position = glm::vec3(
//                mesh->mVertices[j].x,
//                mesh->mVertices[j].y,
//                mesh->mVertices[j].z
//            );
//
//            // Normal
//            vertex.Normal = glm::vec3(
//                mesh->mNormals[j].x,
//                mesh->mNormals[j].y,
//                mesh->mNormals[j].z
//            );
//
//            // Texture coordinates (if available)
//            if (mesh->mTextureCoords[0]) {
//                vertex.TexCoords = glm::vec2(
//                    mesh->mTextureCoords[0][j].x,
//                    mesh->mTextureCoords[0][j].y
//                );
//            }
//            else {
//                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
//            }
//
//            // Bone influences will be processed next.
//            vertices.push_back(vertex);
//        }
//
//        // --- Process bone data, if available ---
//        if (mesh->HasBones()) {
//            for (unsigned int i = 0; i < mesh->mNumBones; ++i) {
//                aiBone* bone = mesh->mBones[i];
//                std::string boneName(bone->mName.C_Str());
//
//                // If this bone is new, assign it an index.
//                if (boneMapping.find(boneName) == boneMapping.end()) {
//                    int newIndex = static_cast<int>(boneMapping.size());
//                    boneMapping[boneName] = newIndex;
//                }
//                int boneIndex = boneMapping[boneName];
//
//                std::cout << "Processing Bone: " << boneName << " (Index: " << boneIndex << ")" << std::endl;
//
//                // For each vertex influenced by this bone...
//                for (unsigned int k = 0; k < bone->mNumWeights; ++k) {
//                    aiVertexWeight weight = bone->mWeights[k];
//                    int vertexID = weight.mVertexId;    // The index of the vertex affected
//                    float boneWeight = weight.mWeight;    // The weight of this influence
//
//                    std::cout << "  -> Vertex " << vertexID << " influenced by bone " << boneName
//                        << " with weight " << boneWeight << std::endl;
//
//                    // Add this bone influence to the vertex
//                    AddBoneData(vertices[vertexID], boneIndex, boneWeight);
//                }
//            }
//        }
//
//        // --- Extract indices ---
//        for (unsigned int j = 0; j < mesh->mNumFaces; ++j) {
//            aiFace face = mesh->mFaces[j];
//            for (unsigned int k = 0; k < face.mNumIndices; ++k) {
//                indices.push_back(face.mIndices[k]);
//            }
//        }
//
//        // --- Set up OpenGL buffers (VAO, VBO, EBO) ---
//        unsigned int VAO, VBO, EBO;
//        glGenVertexArrays(1, &VAO);
//        glGenBuffers(1, &VBO);
//        glGenBuffers(1, &EBO);
//
//        glBindVertexArray(VAO);
//
//        // Vertex Buffer (VBO)
//        glBindBuffer(GL_ARRAY_BUFFER, VBO);
//        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_DYNAMIC_DRAW);
//
//        // Element Buffer (EBO)
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_DYNAMIC_DRAW);
//
//        // Set up vertex attribute pointers:
//        // Position attribute (location 0)
//        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
//        glEnableVertexAttribArray(0);
//
//        // Normal attribute (location 1)
//        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
//        glEnableVertexAttribArray(1);
//
//        // Texture coordinates attribute (location 2)
//        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
//        glEnableVertexAttribArray(2);
//
//        glBindVertexArray(0);  // Unbind VAO
//
//
//        std::cout << "=== FINAL BONE ASSIGNMENTS ===" << std::endl;
//        for (size_t i = 0; i < vertices.size(); ++i) {
//            std::cout << "Vertex " << i << " Bone IDs: ";
//            for (int j = 0; j < MAX_BONE_INFLUENCE; ++j) {
//                std::cout << vertices[i].m_BoneIDs[j] << " (W: " << vertices[i].m_Weights[j] << ") ";
//            }
//            std::cout << std::endl;
//        }
//
//        // --- Create a Mesh instance and store it ---
//        Mesh processedMesh;
//        processedMesh.VAO = VAO;
//        processedMesh.VBO = VBO;
//        processedMesh.EBO = EBO;
//        processedMesh.vertices = vertices;
//        processedMesh.bindPoseVertices = vertices;  // Store original vertex data for skinning.
//        processedMesh.indices = indices;
//        meshDataMesh.push_back(processedMesh);
//    }
//}


glm::vec3 Animation::InterpolatePosition(float animationTime, const BoneAnimation& boneAnim) {
    if (boneAnim.keyFrames.size() == 1)
        return boneAnim.keyFrames[0].position;

    // Find the two keyframes for interpolation
    for (size_t i = 0; i < boneAnim.keyFrames.size() - 1; ++i) {
        const KeyFrame& start = boneAnim.keyFrames[i];
        const KeyFrame& end = boneAnim.keyFrames[i + 1];

        if (animationTime < end.timeStamp) {
            float deltaTime = end.timeStamp - start.timeStamp;
            float factor = (animationTime - start.timeStamp) / deltaTime;
            return glm::mix(start.position, end.position, factor);
        }
    }

    return boneAnim.keyFrames.back().position;
}

glm::quat Animation::InterpolateRotation(float animationTime, const BoneAnimation& boneAnim) {
    if (boneAnim.keyFrames.size() == 1)
        return boneAnim.keyFrames[0].rotation;

    // Same approach as position, but use quaternion slerp
    for (size_t i = 0; i < boneAnim.keyFrames.size() - 1; ++i) {
        const KeyFrame& start = boneAnim.keyFrames[i];
        const KeyFrame& end = boneAnim.keyFrames[i + 1];

        if (animationTime < end.timeStamp) {
            float deltaTime = end.timeStamp - start.timeStamp;
            float factor = (animationTime - start.timeStamp) / deltaTime;
            return glm::slerp(start.rotation, end.rotation, factor);
        }
    }

    return boneAnim.keyFrames.back().rotation;
}

glm::vec3 Animation::InterpolateScale(float animationTime, const BoneAnimation& boneAnim) {
    if (boneAnim.keyFrames.size() == 1)
        return boneAnim.keyFrames[0].scale;

    // Same interpolation logic as position
    for (size_t i = 0; i < boneAnim.keyFrames.size() - 1; ++i) {
        const KeyFrame& start = boneAnim.keyFrames[i];
        const KeyFrame& end = boneAnim.keyFrames[i + 1];

        if (animationTime < end.timeStamp) {
            float deltaTime = end.timeStamp - start.timeStamp;
            float factor = (animationTime - start.timeStamp) / deltaTime;
            return glm::mix(start.scale, end.scale, factor);
        }
    }

    return boneAnim.keyFrames.back().scale;
}

const std::vector<glm::mat4>& Animation::GetBoneTransformsAtTime(float currentTime) {
    boneTransforms.clear();
    boneTransforms.resize(boneMapping.size(), glm::mat4(1.0f)); // Ensure correct size

    for (const auto& [boneName, boneInfo] : boneMapping) {
        if (boneHierarchy.find(boneName) == boneHierarchy.end() || boneHierarchy[boneName] == "") {
            CalculateBoneTransform(currentTime, boneName, glm::mat4(1.0f)); // Root transformation
        }
    }

    return boneTransforms;
}

void Animation::CalculateBoneTransform(float animationTime, const std::string& boneName, glm::mat4 parentTransform) {
    // Find bone animation data

    std::cout << "Calculating Bone Transform Size: " << g_ResourceManager.boneAnimations.size() << '\n';

    auto boneAnimIt = g_ResourceManager.boneAnimations.find(boneName);
    if (boneAnimIt == g_ResourceManager.boneAnimations.end()) return;

    const BoneAnimation& boneAnim = boneAnimIt->second;

    // Interpolate position, rotation, scale
    glm::vec3 position = InterpolatePosition(animationTime, boneAnim);
    glm::quat rotation = InterpolateRotation(animationTime, boneAnim);
    glm::vec3 scale = InterpolateScale(animationTime, boneAnim);

    // Compute local transform
    glm::mat4 localTransform = glm::translate(glm::mat4(1.0f), position) *
        glm::mat4_cast(rotation) *
        glm::scale(glm::mat4(1.0f), scale);

    // Compute global transform
    glm::mat4 globalTransform = parentTransform * localTransform;

    // Store bone transformation
    if (boneMapping.find(boneName) != boneMapping.end()) {
        int boneIndex = boneMapping[boneName];
        boneTransforms[boneIndex] = globalTransform * boneInfoMap[boneName].offsetMatrix;
    }

    // Process child bones
    for (const auto& [childName, parent] : boneHierarchy) {
        if (parent == boneName) { // If this bone is a parent
            CalculateBoneTransform(animationTime, childName, globalTransform);
        }
    }
}


void Animation::UpdateAnimation(float deltaTime)
{
    currentTime += deltaTime * ticksPerSecond;
    currentTime = fmod(currentTime, duration); // Loop animation

    std::cout << "Current Time: " << currentTime << std::endl;

    for (auto& bonePair : g_ResourceManager.boneAnimations) {
        int i = 0;

        std::string boneName = bonePair.first;
        BoneAnimation& boneAnim = bonePair.second;

        if (boneAnim.keyFrames.size() < 2) continue; // Skip if there's no interpolation

        // Initialize with default values (first two keyframes)
        KeyFrame keyFrameBefore = boneAnim.keyFrames[0];
        KeyFrame keyFrameAfter = boneAnim.keyFrames[1];

        // Try to find the valid interval for the current time
        bool foundInterval = false;
        for (size_t i = 0; i < boneAnim.keyFrames.size() - 1; ++i) {
            if (currentTime >= boneAnim.keyFrames[i].timeStamp && currentTime < boneAnim.keyFrames[i + 1].timeStamp) {
                keyFrameBefore = boneAnim.keyFrames[i];
                keyFrameAfter = boneAnim.keyFrames[i + 1];
                foundInterval = true;
                break;
            }
        }

        // Optional: If not found, you might want to choose a fallback.
        // For instance, if currentTime is beyond the last keyframe, you might choose the last two keyframes:
        if (!foundInterval) {
            keyFrameBefore = boneAnim.keyFrames[boneAnim.keyFrames.size() - 2];
            keyFrameAfter = boneAnim.keyFrames[boneAnim.keyFrames.size() - 1];
        }

        // Avoid division by zero (ensure the timestamps differ)
        if (fabs(keyFrameAfter.timeStamp - keyFrameBefore.timeStamp) < 1e-5f) {
            std::cerr << "Warning: Keyframes have identical timestamps for bone " << boneName << std::endl;
            continue;
        }

        // Interpolate
        float alpha = (currentTime - keyFrameBefore.timeStamp) / (keyFrameAfter.timeStamp - keyFrameBefore.timeStamp);
        glm::vec3 interpolatedPosition = glm::mix(keyFrameBefore.position, keyFrameAfter.position, alpha);
        glm::quat interpolatedRotation = glm::slerp(keyFrameBefore.rotation, keyFrameAfter.rotation, alpha);
        glm::vec3 interpolatedScale = glm::mix(keyFrameBefore.scale, keyFrameAfter.scale, alpha);

        // Debug output for interpolation
        std::cout << "Bone: " << boneName << std::endl;
        std::cout << "Interpolated Position: " << interpolatedPosition.x << ", "
            << interpolatedPosition.y << ", " << interpolatedPosition.z << std::endl;
        std::cout << "Interpolated Rotation: " << interpolatedRotation.x << ", "
            << interpolatedRotation.y << ", " << interpolatedRotation.z << ", "
            << interpolatedRotation.w << std::endl;

        // Store the transformation matrix for this bone
        boneAnim.currentTransform = glm::translate(glm::mat4(1.0f), interpolatedPosition) *
            glm::mat4_cast(interpolatedRotation) *
            glm::scale(glm::mat4(1.0f), interpolatedScale);

        // Optionally, update the boneNames vector if needed
        if (std::find(g_ResourceManager.boneNames.begin(), g_ResourceManager.boneNames.end(), boneAnim.boneName) == g_ResourceManager.boneNames.end()) {
            g_ResourceManager.boneNames.push_back(boneAnim.boneName);
        }


     //   meshDataMesh[i].UpdateMesh(updatedVertices, indices);
    }

    // If you want to update the mesh with the new vertex data based on the bone transforms,
    // you would call your mesh update function here.
    // For example:
}