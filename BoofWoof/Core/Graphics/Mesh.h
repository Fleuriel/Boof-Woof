/**************************************************************************
 * @file Mesh.h
 * @author 	Guo Chen
 * @param DP email: g.chen@digipen.edu [2200518]
 * @param Course: CS 3401
 * @param Course: Game Project 3
 * @date  10/06/2024 (06 OCTOBER 2024)
 * @brief
 *
 * This file contains the definition of the Mesh class, which is used to
 * store the vertices, indices, and textures of a mesh. It also provides
 * functions to render the mesh.
 *
 *************************************************************************/

#pragma once
#ifndef MESH_H
#define MESH_H

#include "Shader.h"
//#include "ResourceManager/ResourceManager.h"

constexpr auto MAX_BONE_INFLUENCE = 4;
#define UNREFERENCED_PARAMETER(P)          (P)

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
	glm::vec2 TexCoords;
    // tangent
  //  glm::vec3 Tangent;
    // bitangent
//    glm::vec3 Bitangent;
    ////bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    ////weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture {
    int id{};
    std::string type;
    std::string path;
};

class Mesh {
public:
    // mesh Data
    std::vector<Vertex>       vertices{};
    std::vector<unsigned int> indices{};
	std::vector<Texture>      textures{};
    unsigned int VAO{};
    unsigned int drawMode{};
    int count = 0;

    // constructor
    Mesh() {}
    //Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;


// #ifdef _DEBUG
//         std::cout << "Mesh.h\n";
// 
//         for (int i = 0; i < textures.size(); ++i)
//         {
//             std::cout << textures[i].id << '\t' << textures[i].path << '\t' << textures[i].type << '\n';
//         }
// 
// 
//         std::cout << "Vertices Size " << vertices.size() << " Indices Size " << indices.size() << " Tex Size " << textures.size() << '\n';
// 
// #endif
       // AddTexture("Bed");
        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();       
    
    
    }

    // render the mesh
    void Draw(OpenGLShader& shader)
    {
        
        // bind appropriate textures
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            std::string number;
            std::string name = textures[i].type;
            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++); // transfer unsigned int to string
            else if (name == "texture_normal")
                number = std::to_string(normalNr++); // transfer unsigned int to string
            else if (name == "texture_height")
                number = std::to_string(heightNr++); // transfer unsigned int to string


       //     std::cout << name + number << '\t' << i << '\n';

            // now set the sampler to the correct texture unit
            glUniform1i(glGetUniformLocation(shader.GetHandle(), (name + number).c_str()), i);
            // and finally bind the texture
            glBindTexture(GL_TEXTURE_2D, textures[i].id);

		//	std::cout << "Uniform name: " << (name + number).c_str() << " withn value: " << i << " texture id: " << textures[i].id << std::endl;
        }

        // draw mesh
        //glPolygonMode(GL_FRONT_AND_BACK, drawMode);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }


    void DrawForPicking() const
    {
        // Bind the VAO
        glBindVertexArray(VAO);

        // Draw the mesh without textures
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);

        // Unbind the VAO
        glBindVertexArray(0);
    }

	void DrawWireFrame() const
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}


    void DrawMaterial(OpenGLShader& shader) const
    {
        UNREFERENCED_PARAMETER(shader);
        // draw mesh
        glPolygonMode(GL_FRONT_AND_BACK, GL_TRIANGLES);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));
        glBindVertexArray(0);

    }

    // render the mesh with lines
    void DrawLines() const
    {
        //glPolygonMode(GL_FRONT_AND_BACK, drawMode);

		glBindVertexArray(VAO);

        glDrawArrays(GL_LINES, 0, static_cast<unsigned int>(indices.size()));
		//glDrawElements(GL_LINES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

	}
    
    // render the mesh with point
    void DrawPoints(OpenGLShader& shader) const
	{
        (void)shader;
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        // change the point size
        glPointSize(50.0f);
		glBindVertexArray(VAO);
		glDrawElements(GL_POINTS, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

	}


    void setupGenericMesh()
    {
        // create buffers/arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_DYNAMIC_DRAW);


        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    }

    void updateGenericMesh()
    {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_DYNAMIC_DRAW);
    }


  // void AddTexture(const std::string& textureName)
  // {
  //     Texture texture;
  //     texture.id = g_ResourceManager.GetTextureDDS(textureName); // Assuming you have a function to load the texture
  //     texture.type = "texture_diffuse";
  //     //texture.path = path;
  //     textures.push_back(texture); // Add the texture to the vector
  // }



    //void setupGenericMesh()
    //{
    //    // create buffers/arrays
    //    glGenVertexArrays(1, &VAO);
    //    glGenBuffers(1, &VBO);
    //
    //    glBindVertexArray(VAO);
    //    // load data into vertex buffers
    //    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //    // A great thing about structs is that their memory layout is sequential for all its items.
    //    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    //    // again translates to 3/2 floats which translates to a byte array.
    //    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_DYNAMIC_DRAW);
    //
    //
    //    // set the vertex attribute pointers
    //    // vertex Positions
    //    glEnableVertexAttribArray(0);
    //    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    //
    //}

    //void updateGenericMesh()
    //{
	//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_DYNAMIC_DRAW);
	//}

//private:
    // render data 
   unsigned int VBO{}, EBO{};
   //
   // // initializes all the buffer objects/arrays
   void setupMesh()
   {


       // create buffers/arrays
       glGenVertexArrays(1, &VAO);
       glGenBuffers(1, &VBO);
       glGenBuffers(1, &EBO);

       glBindVertexArray(VAO);
       // load data into vertex buffers
       glBindBuffer(GL_ARRAY_BUFFER, VBO);
       // A great thing about structs is that their memory layout is sequential for all its items.
       // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
       // again translates to 3/2 floats which translates to a byte array.
       glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

       glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
       glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

       // set the vertex attribute pointers
       // vertex Positions
       glEnableVertexAttribArray(0);
       glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
       // vertex normals
       glEnableVertexAttribArray(1);
       glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
       // vertex texture coords
       glEnableVertexAttribArray(2);
       glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
      
  
       // vertex tangent
      // glEnableVertexAttribArray(3);
      // glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
      // // vertex bitangent
      // glEnableVertexAttribArray(4);
      // glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
       //// ids
       //glEnableVertexAttribArray(5);
       //glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
  
       //// weights
       //glEnableVertexAttribArray(6);
       //glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
        glBindVertexArray(0);
   }

   
   void DrawForShadow() const
   {
       // Bind the vertex array object.
       glBindVertexArray(VAO);
       // Draw the mesh using the index buffer. (Assumes triangles.)
       glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
       // Unbind the VAO.
       glBindVertexArray(0);
   }
    
    

};

#endif