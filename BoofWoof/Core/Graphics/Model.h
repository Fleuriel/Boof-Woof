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
#include "Animation/Animation.h"

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
    
    void DrawAnimation(OpenGLShader& shader)
    {

    }


    void UpdateMesh()
    {
        for (unsigned int i = 0; i < meshes.size(); ++i)
        {
           // meshes[i].UpdateMesh();
        }

    }

    void DrawForPicking()
    {
        for (Mesh& mesh : meshes)
        {
            mesh.DrawForPicking();
        }
    }

    void DrawMaterial(OpenGLShader& shader)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].DrawMaterial(shader);

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


};






Model SquareModel(glm::vec3 color);
Model CubeModel(glm::vec3 color);

Model SquareModelOutline(glm::vec3 color);
Model AABB(glm::vec3 position, glm::vec3 halfextents = glm::vec3(1.0f), glm::vec3 color = glm::vec3(1.0f));
Model OBB(glm::vec3 position, glm::vec3 halfExtents, glm::vec3 rotation, glm::vec3 color = glm::vec3(1.0f));

#endif // !MODEL_H