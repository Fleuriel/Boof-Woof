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
    glm::vec2 TexCoords{ -1.1f, -1.1f };
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


    unsigned int VAO{};
    unsigned int drawMode{};

    Mesh() {}
    //Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)//, std::vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        //this->textures = textures;

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();

    }


    unsigned int VBO{}, EBO{};

    // initializes all the buffer objects/arrays
    void setupMesh()
    {
        std::cout << "this happened here then\n";


#ifdef _DEBUG
        std::cout << "Generating Vertex Arrays\n";
#endif
        // create buffers/arrays
        glGenVertexArrays(1, &VAO);

#ifdef _DEBUG
        std::cout << "Generating Buffers for VBO and EBO\n";
#endif
        glGenBuffers(1, &VBO);

        glGenBuffers(1, &EBO);

#ifdef _DEBUG
        std::cout << "Binding Vertex Arrays for VAO\n";
#endif
        glBindVertexArray(VAO);


#ifdef _DEBUG
        std::cout << "Binding Buffer to VBO\n";
#endif
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);


#ifdef _DEBUG
        std::cout << "Binding Buffer to EBO\n";
#endif

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers

#ifdef _DEBUG
        std::cout << "Enabling Attributes for Location 0 in Vertex Shader\n";
#endif

        // vertex Positions (location = 0 in vertex shader)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

#ifdef _DEBUG
        std::cout << "Enabling Attributes for Location 1 in Vertex Shader\n";
#endif
        // vertex normals (location = 1 in vertex shader)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));


#ifdef _DEBUG
        std::cout << "Texture Coords\n";
#endif

        //check if the TexCoords are empty
        if (vertices[0].TexCoords.x != 1.1f && vertices[0].TexCoords.y != 1.1f) {
            // vertex texture coords (location = 2 in vertex shader)
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        }
        else {
            // vertex texture coords (location = 2 in vertex shader)
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        }

#ifdef _DEBUG
        std::cout << "Binding Vertex Array to 0\n";
#endif
        glBindVertexArray(0);
        //glEnableVertexAttribArray(2);
        //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        //// vertex tangent
        //glEnableVertexAttribArray(3);
        //glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        //// vertex bitangent
        //glEnableVertexAttribArray(4);
        //glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
        //// ids
        //glEnableVertexAttribArray(5);
        //glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

        //// weights
        //glEnableVertexAttribArray(6);
        //glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
       // glBindVertexArray(0);
    }

};