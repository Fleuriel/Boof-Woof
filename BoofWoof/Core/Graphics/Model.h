/**************************************************************************
 * @file Model.h
 * @author 	Guo Chen
 * @param DP email: g.chen@digipen.edu [2200518]
 * @param Course: CS 3401
 * @param Course: Game Project 3
 * @date  10/06/2024 (06 OCTOBER 2024)
 * @brief
 *
 * This file contains the definition of the Model class, which is used to
 * store the meshes of a model. It also provides functions to load and render
 * the model.
 *************************************************************************/

#pragma once

#ifndef MODEL_H
#define MODEL_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Shader.h"
#include "Mesh.h"
#include "../../BoofWoof/Core/Animation/AnimData.h"

#include "AssetManager/AssetManager.h"


unsigned int TextureFromFile(const char* path, const std::string& directory);
//extern std::vector<Model2D> models;
struct ShaderParams;  // Forward declaration of ShaderParams

class Model
{
public:
    // model data 
    std::vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<Mesh>    meshes;
    std::string directory;
    std::string name;
    int count = 0;

    //// for now 
    // store number of texture
    int texture_cnt = 0;

    // 2D
    GLenum primitive_type{};		// Primitive Type
    size_t primitive_cnt{};		// Primitive Count
    GLuint vaoid{};				// Vaoid of the Model
    GLsizei draw_cnt{};			// Draw Count of the model
    size_t idx_elem_cnt{};		// Index Element Count of the Model

    Model() {}

	Model(std::string const& path, bool gamma = false) 
	{
		loadModel(path);
	}


    std::map<std::string, BoneInfo> m_BoneInfoMap;
    int m_BoneCounter = 0;

    void SetVertexBoneDataToDefault(Vertex& vertex)
    {
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
        {
            vertex.m_BoneIDs[i] = -1;
            vertex.m_Weights[i] = 0.0f;
        }
    }

    auto& GetBoneInfoMap() { return m_BoneInfoMap; }
    int& GetBoneCount() { return m_BoneCounter; }


    void Draw2D(OpenGLShader& shader) const;

    void DrawCollisionBox2D(Model outlineModel);

    void DrawCollisionBox3D(glm::vec3 position, glm::vec3 halfExtents, glm::vec3 color, float lineWidth = 1.0f) const;


        // draws the model, and thus all its meshes
    void Draw(OpenGLShader& shader)
    {
        //std::cout << "Mesh Size: " << meshes.size() << '\n';
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }
    

    void DrawForPicking()
    {
        for (Mesh& mesh : meshes)
        {
            mesh.DrawForPicking();
        }
    }

	void DrawWireFrame()
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].DrawWireFrame();
	}

    void DrawMaterial(OpenGLShader& shader)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].DrawMaterial(shader);

    }

    void DrawForShadow() const
    {
        for (const Mesh& mesh : meshes)
        {
            mesh.DrawForShadow();
        }
    }

    // draw with line
    void DrawLine()
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].DrawLines();
	}

    void DrawPoint(OpenGLShader& shader)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].DrawPoints(shader);
	}

   

    void addMesh(const Mesh& mesh)
    {
        meshes.push_back(mesh);
    }


    void drawOBB(const glm::vec3& position, const glm::vec3& rotationRadians, const glm::vec3& halfextents, glm::vec3 color = glm::vec3(1.0f,0.0f,0.0f), float lineWidth = 2.0f) {

        // Create transformation matrix
        glm::mat4 transform = glm::mat4(1.0f);

        // Apply rotations (order: X, Y, Z)
        transform = glm::rotate(transform, glm::radians(rotationRadians.x), glm::vec3(1.0f, 0.0f, 0.0f));
        transform = glm::rotate(transform, glm::radians(rotationRadians.y), glm::vec3(0.0f, 1.0f, 0.0f));
        transform = glm::rotate(transform, glm::radians(rotationRadians.z), glm::vec3(0.0f, 0.0f, 1.0f));

        // Apply translation to position
        transform = glm::translate(transform, position);

        // Vertices of the OBB in its local space, now with color
        struct Vertex {
            glm::vec3 position;
            glm::vec3 color;
        };

        Vertex obbVertices[8] = {
            {glm::vec3(halfextents.x, halfextents.y, halfextents.z), color},
            {glm::vec3(-halfextents.x, halfextents.y, halfextents.z), color},
            {glm::vec3(-halfextents.x, -halfextents.y, halfextents.z), color},
            {glm::vec3(halfextents.x, -halfextents.y, halfextents.z), color},
            {glm::vec3(halfextents.x, halfextents.y, -halfextents.z), color},
            {glm::vec3(-halfextents.x, halfextents.y, -halfextents.z), color},
            {glm::vec3(-halfextents.x, -halfextents.y, -halfextents.z), color},
            {glm::vec3(halfextents.x, -halfextents.y, -halfextents.z), color}
        };

        // Apply rotation to each vertex position
        glm::mat3 rotationMatrix = glm::mat3(transform); // Extract rotation part from the transform matrix

        for (auto& vertex : obbVertices) {
            vertex.position = glm::vec3(rotationMatrix * vertex.position);  // Apply rotation to each vertex
        }

        // Indices for drawing lines (connect vertices to form edges of the box)
        GLuint indices[24] = {
            0, 1, 1, 2, 2, 3, 3, 0,  // Bottom face
            4, 5, 5, 6, 6, 7, 7, 4,  // Top face
            0, 4, 1, 5, 2, 6, 3, 7   // Connecting vertical lines
        };

        // Generate and bind VAO and VBO
        GLuint VAO, VBO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        // Vertex buffer: Send the transformed vertices to the GPU
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(obbVertices), obbVertices, GL_STATIC_DRAW);

        // Element buffer: Send the indices for the lines
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);

        // Color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
        glEnableVertexAttribArray(1);


        glLineWidth(lineWidth);  // Set line width to make the AABB more visible
        glColor3f(1.0f, 1.0f, 1.0f);

        // Draw the OBB as a wireframe
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // Cleanup
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

//    void RenderAABB(const glm::vec3& center, const glm::vec3& halfExtents, const glm::vec3& color);

    void setMesh()
    {
        for (int i = 0; i < meshes.size(); ++i)
        {
            meshes[i].setupMesh();
        }
    }

    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(std::string const& path)
    {
        // read file via ASSIMP
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
        // check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return;
        }
        // retrieve the directory path of the filepath
        directory = path.substr(0, path.find_last_of('/'));

        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene);
    }

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene)
    {
        // process each mesh located at the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }

    }
    void loadLine()
    {
        // data to fill
        std::vector<Vertex> vertices;
        //vector<unsigned int> indices;
   
        vertices = { {glm::vec3(0.f, 0.f, 0.f)}, {glm::vec3(1.f, 0.f, 0.f)} };
   
        std::vector<unsigned int> indices = { 0, 1 };
        Mesh lineMesh;
        lineMesh.vertices = vertices;
        lineMesh.indices = indices;
        lineMesh.drawMode = GL_LINES;
      //  lineMesh.setupGenericMesh();
       
        meshes.push_back(lineMesh);
    }
   
   //// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
   //void processNode(aiNode* node, const aiScene* scene, unsigned int draw_mode)
   //{
   //    // process each mesh located at the current node
   //    for (unsigned int i = 0; i < node->mNumMeshes; i++)
   //    {
   //        // the node object only contains indices to index the actual objects in the scene. 
   //        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
   //        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
   //        meshes.push_back(processMesh(mesh, scene, draw_mode));
   //    }
   //    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
   //    for (unsigned int i = 0; i < node->mNumChildren; i++)
   //    {
   //        processNode(node->mChildren[i], scene, draw_mode);
   //    }
   //
   //}
   //
    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        using namespace std;

        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            SetVertexBoneDataToDefault(vertex);
            vertex.Position =glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

            if (mesh->mTextureCoords[0])
            {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        ExtractBoneWeightForVertices(vertices, mesh, scene);

        return Mesh(vertices, indices, textures);
    }

    void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
    {
        auto& boneInfoMap = m_BoneInfoMap;
        int& boneCount = m_BoneCounter;

        auto ConvertMatrixToGLM = [](const aiMatrix4x4& from) -> glm::mat4 {
            return glm::mat4{
                { from.a1, from.b1, from.c1, from.d1 },
                { from.a2, from.b2, from.c2, from.d2 },
                { from.a3, from.b3, from.c3, from.d3 },
                { from.a4, from.b4, from.c4, from.d4 }
            };
            };

        for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
        {
            int boneID = -1;
            std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();

            // Check if bone is new, otherwise use existing bone ID
            if (boneInfoMap.find(boneName) == boneInfoMap.end())
            {
                BoneInfo newBoneInfo;
                newBoneInfo.id = boneCount;
                newBoneInfo.offset = ConvertMatrixToGLM(mesh->mBones[boneIndex]->mOffsetMatrix);
                boneInfoMap[boneName] = newBoneInfo;
                boneID = boneCount;
                boneCount++;
            }
            else
            {
                boneID = boneInfoMap[boneName].id;
            }

            assert(boneID != -1);

            auto weights = mesh->mBones[boneIndex]->mWeights;
            int numWeights = mesh->mBones[boneIndex]->mNumWeights;

            for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
            {
                int vertexId = weights[weightIndex].mVertexId;
                float weight = weights[weightIndex].mWeight;

                assert(vertexId < vertices.size());

                // Assign the bone influence
                SetVertexBoneData(vertices[vertexId], boneID, weight, vertexId);
            }
        }
    }



    void SetVertexBoneData(Vertex& vertex, int boneID, float weight, int vertexId)
    {
        bool inserted = false;

        for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
        {
            if (vertex.m_BoneIDs[i] < 0) // Find an empty slot
            {
                vertex.m_Weights[i] = weight; // Assign weight
                vertex.m_BoneIDs[i] = boneID; // Assign Bone ID
                inserted = true;
                break;
            }
        }

        if (!inserted) // If all slots are full, warn
        {
            std::cerr << " WARNING: Vertex " << vertexId
                << " already has max bone influences! Extra influences ignored.\n";
        }

        // Debugging print
        std::cout << "Vertex " << vertexId << " Bone IDs: ("
            << vertex.m_BoneIDs[0] << ", "
            << vertex.m_BoneIDs[1] << ", "
            << vertex.m_BoneIDs[2] << ", "
            << vertex.m_BoneIDs[3] << ") Weights: ("
            << vertex.m_Weights[0] << ", "
            << vertex.m_Weights[1] << ", "
            << vertex.m_Weights[2] << ", "
            << vertex.m_Weights[3] << ")\n";
    }


   //unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false)
   //{
   //    std::string filename =std::string(path);
   //    filename = directory + '/' + filename;
   //
   //    unsigned int textureID;
   //    glGenTextures(1, &textureID);
   //
   //    int width, height, nrComponents;
   //    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
   //    if (data)
   //    {
   //        GLenum format;
   //        if (nrComponents == 1)
   //            format = GL_RED;
   //        else if (nrComponents == 3)
   //            format = GL_RGB;
   //        else if (nrComponents == 4)
   //            format = GL_RGBA;
   //
   //        glBindTexture(GL_TEXTURE_2D, textureID);
   //        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
   //        glGenerateMipmap(GL_TEXTURE_2D);
   //
   //        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   //        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   //        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
   //        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   //
   //        stbi_image_free(data);
   //    }
   //    else
   //    {
   //        std::cout << "Texture failed to load at path: " << path << std::endl;
   //        stbi_image_free(data);
   //    }
   //
   //    return textureID;
   //}


     /*checks all material textures of a given type and loads the textures if they're not loaded yet.
     the required info is returned as a Texture struct.*/
       std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
       {
           std::vector<Texture> textures;
           for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
           {
               aiString str;
               mat->GetTexture(type, i, &str);
               // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
               bool skip = false;
               for (unsigned int j = 0; j < textures_loaded.size(); j++)
               {
                   if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                   {
                       textures.push_back(textures_loaded[j]);
                       skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                       break;
                   }
               }
               if (!skip)
               {   // if texture hasn't been loaded already, load it
                   Texture texture;
                   texture.id = TextureFromFile(str.C_Str(), this->directory);
                   texture.type = typeName;
                   texture.path = str.C_Str();
                   textures.push_back(texture);
                   textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
               }
           }
           return textures;
       }
};



//{
//    std::string filename = std::string(path);
//    filename = directory + '/' + filename;
//
//      unsigned int textureID;
//    glGenTextures(1, &textureID);
//
//    int width, height, nrComponents;
//    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
//    if (data)
//    {
//        GLenum format;
//        if (nrComponents == 1)
//            format = GL_RED;
//        else if (nrComponents == 3)
//            format = GL_RGB;
//        else if (nrComponents == 4)
//            format = GL_RGBA;
//
//        glBindTexture(GL_TEXTURE_2D, textureID);
//        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
//        glGenerateMipmap(GL_TEXTURE_2D);
//
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//        stbi_image_free(data);
//    }
//    else
//    {
//       std::cout << "Texture failed to load at path: " << path << std::endl;
//        stbi_image_free(data);
//    }
//
//    return textureID;
//};



Model SquareModel(glm::vec3 color);
Model CubeModel(glm::vec3 color);

Model SquareModelOutline(glm::vec3 color);
Model AABB(glm::vec3 position, glm::vec3 halfextents = glm::vec3(1.0f), glm::vec3 color = glm::vec3(1.0f));
Model OBB(glm::vec3 position, glm::vec3 halfExtents, glm::vec3 rotation, glm::vec3 color = glm::vec3(1.0f));

#endif // !MODEL_H