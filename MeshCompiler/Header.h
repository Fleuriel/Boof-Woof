#pragma once
#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>



#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <../../../lib/stb-master/stb_image.h>


struct Texture {
    unsigned int id{};
    std::string type;
    std::string path;
};

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    //// tangent
    //glm::vec3 Tangent;
    //// bitangent
    //glm::vec3 Bitangent;
    ////bone indexes which will influence this vertex
    //int m_BoneIDs[MAX_BONE_INFLUENCE];
    ////weights from each bone
    //float m_Weights[MAX_BONE_INFLUENCE];
};



class Mesh {
public:
    // mesh Data
    std::vector<Vertex>       vertices{};
    std::vector<unsigned int> indices{};
    std::vector<Texture>      textures{};


    unsigned int drawMode{};

    Mesh() {}
    //Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)//, std::vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
    }

};