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

#include "AssetManager/AssetManager.h"


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



    void Draw2D(OpenGLShader& shader);

    void DrawCollisionBox2D(Model outlineModel);

    void DrawCollisionBox3D(Model outlineModel) const;


        // draws the model, and thus all its meshes
    void Draw(OpenGLShader& shader)
    {
        //std::cout << "Mesh Size: " << meshes.size() << '\n';
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }
    

    void DrawForPicking(OpenGLShader& shader)
    {
        for (Mesh& mesh : meshes)
        {
            mesh.DrawForPicking(shader);
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

    //void setMesh()
    //{
    //    for (int i = 0; i < meshes.size(); ++i)
    //    {
    //        meshes[i].setupMesh();
    //    }
    //}

   // // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
   // void loadModel(std::string const& path, unsigned int draw_mode)
   // {
   //     // read file via ASSIMP
   //     Assimp::Importer importer;
   //     const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
   //     // check for errors
   //     if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
   //     {
   //         std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
   //         return;
   //     }
   //     // retrieve the directory path of the filepath
   //     directory = path.substr(0, path.find_last_of('/'));
   //
   //     // process ASSIMP's root node recursively
   //     processNode(scene->mRootNode, scene, draw_mode);
   //    
   // }
   //
   // void loadLine()
   // {
   //     // data to fill
   //     std::vector<Vertex> vertices;
   //     //vector<unsigned int> indices;
   //
   //     vertices = { {glm::vec3(0.f, 0.f, 0.f)}, {glm::vec3(1.f, 0.f, 0.f)} };
   //
   //     std::vector<unsigned int> indices = { 0, 1 };
   //     Mesh lineMesh;
   //     lineMesh.vertices = vertices;
   //     lineMesh.indices = indices;
   //     lineMesh.drawMode = GL_LINES;
   //   //  lineMesh.setupGenericMesh();
   //    
   //     meshes.push_back(lineMesh);
   // }
   //
   // // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
   // void processNode(aiNode* node, const aiScene* scene, unsigned int draw_mode)
   // {
   //     // process each mesh located at the current node
   //     for (unsigned int i = 0; i < node->mNumMeshes; i++)
   //     {
   //         // the node object only contains indices to index the actual objects in the scene. 
   //         // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
   //         aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
   //         meshes.push_back(processMesh(mesh, scene, draw_mode));
   //     }
   //     // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
   //     for (unsigned int i = 0; i < node->mNumChildren; i++)
   //     {
   //         processNode(node->mChildren[i], scene, draw_mode);
   //     }
   //
   // }
   //
   // Mesh processMesh(aiMesh* mesh, const aiScene* scene, unsigned int draw_mode)
   // {
   //     (void)scene;
   //     // data to fill
   //     std::vector<Vertex> vertices;
   //     std::vector<unsigned int> indices;
   //     std::vector<Texture> textures;
   //
   //
   //     // walk through each of the mesh's vertices
   //     for (unsigned int i = 0; i < mesh->mNumVertices; i++)
   //     {
   //         Vertex vertex;
   //         glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
   //         // positions
   //         vector.x = mesh->mVertices[i].x;
   //         vector.y = mesh->mVertices[i].y;
   //         vector.z = mesh->mVertices[i].z;
   //         vertex.Position = vector;
   //         // normals
   //         if (mesh->HasNormals())
   //         {
   //             vector.x = mesh->mNormals[i].x;
   //             vector.y = mesh->mNormals[i].y;
   //             vector.z = mesh->mNormals[i].z;
   //             vertex.Normal = vector;
   //         }
   //         // texture coordinates
   //         if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
   //         {
   //           // glm::vec2 vec;
   //           // // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
   //           // // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
   //           // vec.x = mesh->mTextureCoords[0][i].x;
   //           // vec.y = mesh->mTextureCoords[0][i].y;
   //           // vertex.TexCoords = vec;
   //           // // tangent
   //           // vector.x = mesh->mTangents[i].x;
   //           // vector.y = mesh->mTangents[i].y;
   //           // vector.z = mesh->mTangents[i].z;
   //           // vertex.Tangent = vector;
   //           // // bitangent
   //           // vector.x = mesh->mBitangents[i].x;
   //           // vector.y = mesh->mBitangents[i].y;
   //           // vector.z = mesh->mBitangents[i].z;
   //           // vertex.Bitangent = vector;
   //         }
   //         else
   //             vertex.TexCoords = glm::vec2(0.0f, 0.0f);
   //
   //         vertices.push_back(vertex);
   //     }
   //     // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
   //     for (unsigned int i = 0; i < mesh->mNumFaces; i++)
   //     {
   //         aiFace face = mesh->mFaces[i];
   //         // retrieve all indices of the face and store them in the indices vector
   //         for (unsigned int j = 0; j < face.mNumIndices; j++)
   //             indices.push_back(face.mIndices[j]);
   //     }
   //     //// process materials
   //     // aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
   //     // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
   //     // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
   //     // Same applies to other texture as the following list summarizes:
   //     // diffuse: texture_diffuseN
   //     // specular: texture_specularN
   //     // normal: texture_normalN
   //
   //     // 1. diffuse maps
   //    // std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
   //    // textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
   //    // // 2. specular maps
   //    // std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
   //    // textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
   //    // // 3. normal maps
   //    // std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
   //    // textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
   //    // // 4. height maps
   //    // std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
   //    // textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
   //
   //     // return a mesh object created from the extracted mesh data
   //     //return Mesh(vertices, indices, textures);
   //     Mesh out(vertices, indices);//, glm::vec3(1.0f,1.0f,1.0f));
   //     out.drawMode = draw_mode;
   //     return out;
   // }

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
   //std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
   //{
   //    std::vector<Texture> textures;
   //    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
   //    {
   //        aiString str;
   //        mat->GetTexture(type, i, &str);
   //        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
   //        bool skip = false;
   //        for (unsigned int j = 0; j < textures_loaded.size(); j++)
   //        {
   //            if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
   //            {
   //                textures.push_back(textures_loaded[j]);
   //                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
   //                break;
   //            }
   //        }
   //        if (!skip)
   //        {   // if texture hasn't been loaded already, load it
   //            Texture texture;
   //            texture.id = TextureFromFile(str.C_Str(), this->directory);
   //            texture.type = typeName;
   //            texture.path = str.C_Str();
   //            textures.push_back(texture);
   //            textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
   //        }
   //    }
   //    return textures;
   //}
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
Model AABB(glm::vec3 color);


#endif // !MODEL_H