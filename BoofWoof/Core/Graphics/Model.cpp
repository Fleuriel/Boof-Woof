/**************************************************************************
 * @file Model.cpp
 * @author 	Guo Chen
 * @param DP email: g.chen@digipen.edu [2200518]
 * @param Course: CS 3401
 * @param Course: Game Project 3
 * @date  10/06/2024 (06 OCTOBER 2024)
 * @brief
 *
 * This file contains the definition of the Model class, which is used to
 * store the vertices, indices, and textures of a model. It also provides
 * functions to render the model.
 *************************************************************************/
#include "Model.h"
#include <array>
#include <set>
#include <vector>
#include "ResourceManager/ResourceManager.h"


int GraphicsSystem::set_Texture_ = 0;

//
//
//

//unsigned int TextureFromFile(const char* path, const std::string& directory)
//{
//	std::string filename = std::string(path);
//    filename = directory + '/' + filename;
//
//      unsigned int textureID;
//    glGenTextures(1, &textureID);
//
//    int width, height, nrComponents;
//    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
//    if (data)
//    {
//		GLenum format{};
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
//}

Model SquareModel(glm::vec3 color)
{
	struct Vertex {
		glm::vec2 position;        // Vertex position
		glm::vec3 color;           // Vertex color
		glm::vec2 textureCoord;    // Texture coordinates
	};

	// Define the vertices of a textured square
	std::vector<Vertex> vertices{
		 { glm::vec2(0.5f,  -0.5f), color, glm::vec2(1.0f, 1.0f) }, // Bottom-right
		 { glm::vec2(0.5f,   0.5f), color, glm::vec2(1.0f, 0.0f) }, // Top-right
		 { glm::vec2(-0.5f,  0.5f), color, glm::vec2(0.0f, 0.0f) }, // Top-left
		 { glm::vec2(-0.5f, -0.5f), color, glm::vec2(0.0f, 1.0f) }  // Bottom-left
	};

	// Create Model.
	Model mdl;
	
	// Create and bind a buffer for vertex data
	GLuint vbo_hdl;
	GLuint vaoid;
	GLuint ebo_hdl;

	glCreateBuffers(1, &vbo_hdl);

	// Allocate and fill data store for vertices
	glNamedBufferStorage(vbo_hdl, sizeof(Vertex) * vertices.size(), vertices.data(), GL_DYNAMIC_STORAGE_BIT);

	// Create and bind a vertex array object (VAO)

	glCreateVertexArrays(1, &vaoid);

	// Vertex position attribute
	glEnableVertexArrayAttrib(vaoid, 0);
	glVertexArrayVertexBuffer(vaoid, 0, vbo_hdl, offsetof(Vertex, position), sizeof(Vertex));
	glVertexArrayAttribFormat(vaoid, 0, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoid, 0, 0);

	// Vertex color attribute
	glEnableVertexArrayAttrib(vaoid, 1);
	glVertexArrayVertexBuffer(vaoid, 1, vbo_hdl, offsetof(Vertex, color), sizeof(Vertex));
	glVertexArrayAttribFormat(vaoid, 1, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoid, 1, 1);

	// Texture coordinates attribute
	glEnableVertexArrayAttrib(vaoid, 2);
	glVertexArrayVertexBuffer(vaoid, 2, vbo_hdl, offsetof(Vertex, textureCoord), sizeof(Vertex));
	glVertexArrayAttribFormat(vaoid, 2, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoid, 2, 2);

	// Set up index buffer for rendering
	std::array<GLushort, 4> idx_vtx = { 0, 1, 2, 3 };
	glCreateBuffers(1, &ebo_hdl);
	glNamedBufferStorage(ebo_hdl, sizeof(GLushort) * idx_vtx.size(), idx_vtx.data(), GL_DYNAMIC_STORAGE_BIT);
	glVertexArrayElementBuffer(vaoid, ebo_hdl);

	// Store information in the model structure
	mdl.name = "Square";
	mdl.vaoid = vaoid;
	mdl.primitive_type = GL_TRIANGLE_FAN; // Use GL_TRIANGLE_FAN for a square
	mdl.draw_cnt = static_cast<GLsizei>(idx_vtx.size());
	mdl.primitive_cnt = vertices.size();


	// return model
	return mdl;
}



Model CubeModel(glm::vec3 color)
{
	struct Vertex {
		glm::vec3 position;        // Vertex position (3D now)
		glm::vec3 color;           // Vertex color
		glm::vec2 textureCoord;    // Texture coordinates
	};

	// Define the vertices for each face of a cube with texture coordinates
	std::vector<Vertex> vertices{
		// Front face
		{ glm::vec3(0.5f,  0.5f,  0.5f), color, glm::vec2(1.0f, 0.0f) }, // Top-right
		{ glm::vec3(0.5f, -0.5f,  0.5f), color, glm::vec2(1.0f, 1.0f) }, // Bottom-right
		{ glm::vec3(-0.5f, -0.5f,  0.5f), color, glm::vec2(0.0f, 1.0f) }, // Bottom-left
		{ glm::vec3(-0.5f,  0.5f,  0.5f), color, glm::vec2(0.0f, 0.0f) }, // Top-left

		// Back face
		{ glm::vec3(0.5f,  0.5f, -0.5f), color, glm::vec2(1.0f, 0.0f) }, // Top-right
		{ glm::vec3(0.5f, -0.5f, -0.5f), color, glm::vec2(1.0f, 1.0f) }, // Bottom-right
		{ glm::vec3(-0.5f, -0.5f, -0.5f), color, glm::vec2(0.0f, 1.0f) }, // Bottom-left
		{ glm::vec3(-0.5f,  0.5f, -0.5f), color, glm::vec2(0.0f, 0.0f) }, // Top-left

		// Left face
		{ glm::vec3(-0.5f,  0.5f,  0.5f), color, glm::vec2(1.0f, 0.0f) }, // Top-right
		{ glm::vec3(-0.5f, -0.5f,  0.5f), color, glm::vec2(1.0f, 1.0f) }, // Bottom-right
		{ glm::vec3(-0.5f, -0.5f, -0.5f), color, glm::vec2(0.0f, 1.0f) }, // Bottom-left
		{ glm::vec3(-0.5f,  0.5f, -0.5f), color, glm::vec2(0.0f, 0.0f) }, // Top-left

		// Right face
		{ glm::vec3(0.5f,  0.5f,  0.5f), color, glm::vec2(1.0f, 0.0f) }, // Top-right
		{ glm::vec3(0.5f, -0.5f,  0.5f), color, glm::vec2(1.0f, 1.0f) }, // Bottom-right
		{ glm::vec3(0.5f, -0.5f, -0.5f), color, glm::vec2(0.0f, 1.0f) }, // Bottom-left
		{ glm::vec3(0.5f,  0.5f, -0.5f), color, glm::vec2(0.0f, 0.0f) }, // Top-left

		// Top face
		{ glm::vec3(0.5f,  0.5f, -0.5f), color, glm::vec2(1.0f, 0.0f) }, // Top-right
		{ glm::vec3(0.5f,  0.5f,  0.5f), color, glm::vec2(1.0f, 1.0f) }, // Bottom-right
		{ glm::vec3(-0.5f,  0.5f,  0.5f), color, glm::vec2(0.0f, 1.0f) }, // Bottom-left
		{ glm::vec3(-0.5f,  0.5f, -0.5f), color, glm::vec2(0.0f, 0.0f) }, // Top-left

		// Bottom face
		{ glm::vec3(0.5f, -0.5f, -0.5f), color, glm::vec2(1.0f, 1.0f) }, // Top-right
		{ glm::vec3(0.5f, -0.5f,  0.5f), color, glm::vec2(1.0f, 0.0f) }, // Bottom-right
		{ glm::vec3(-0.5f, -0.5f,  0.5f), color, glm::vec2(0.0f, 0.0f) }, // Bottom-left
		{ glm::vec3(-0.5f, -0.5f, -0.5f), color, glm::vec2(0.0f, 1.0f) }  // Top-left
	};

	// Indices for the cube (each face consists of two triangles)
	std::vector<GLushort> indices = {
		// Front face
		0, 3, 1, 3, 2, 1,
		// Back face
		4, 5, 7, 5, 6, 7,
		// Left face
		8, 11, 9, 11, 10, 9,
		// Right face
		12, 13, 15, 13, 14, 15,
		// Top face
		16, 19, 17, 19, 18, 17,
		// Bottom face
		20, 21, 23, 21, 22, 23
	};

	// Create Model.
	Model mdl;

	// Create and bind buffers for vertex data and indices
	GLuint vbo_hdl;
	GLuint vaoid;
	GLuint ebo_hdl;

	glCreateBuffers(1, &vbo_hdl);

	// Allocate and fill data store for vertices
	glNamedBufferStorage(vbo_hdl, sizeof(Vertex) * vertices.size(), vertices.data(), GL_DYNAMIC_STORAGE_BIT);

	// Create and bind a vertex array object (VAO)
	glCreateVertexArrays(1, &vaoid);

	// Vertex position attribute
	glEnableVertexArrayAttrib(vaoid, 0);
	glVertexArrayVertexBuffer(vaoid, 0, vbo_hdl, offsetof(Vertex, position), sizeof(Vertex));
	glVertexArrayAttribFormat(vaoid, 0, 3, GL_FLOAT, GL_FALSE, 0); // 3 floats for position
	glVertexArrayAttribBinding(vaoid, 0, 0);

	// Vertex color attribute
	glEnableVertexArrayAttrib(vaoid, 1);
	glVertexArrayVertexBuffer(vaoid, 1, vbo_hdl, offsetof(Vertex, color), sizeof(Vertex));
	glVertexArrayAttribFormat(vaoid, 1, 3, GL_FLOAT, GL_FALSE, 0); // 3 floats for color
	glVertexArrayAttribBinding(vaoid, 1, 1);

	// Texture coordinates attribute
	glEnableVertexArrayAttrib(vaoid, 2);
	glVertexArrayVertexBuffer(vaoid, 2, vbo_hdl, offsetof(Vertex, textureCoord), sizeof(Vertex));
	glVertexArrayAttribFormat(vaoid, 2, 2, GL_FLOAT, GL_FALSE, 0); // 2 floats for texture coordinates
	glVertexArrayAttribBinding(vaoid, 2, 2);

	// Set up index buffer for rendering
	glCreateBuffers(1, &ebo_hdl);
	glNamedBufferStorage(ebo_hdl, sizeof(GLushort) * indices.size(), indices.data(), GL_DYNAMIC_STORAGE_BIT);
	glVertexArrayElementBuffer(vaoid, ebo_hdl);

	// Store information in the model structure

	mdl.name = "cubeModel";
	mdl.vaoid = vaoid;
	mdl.primitive_type = GL_TRIANGLES; // Use GL_TRIANGLES for rendering the cube
	mdl.draw_cnt = static_cast<GLsizei>(indices.size());
	mdl.primitive_cnt = vertices.size();

	// Return model
	return mdl;
}











/*Outlines */

Model SquareModelOutline(glm::vec3 color)
{
	struct Vertex {
		glm::vec2 position;
		glm::vec3 color;
	};

	std::vector<Vertex> vertices{
		{ glm::vec2(0.5f,  -0.5f), color }, // Bottom-right
		{ glm::vec2(0.5f,   0.5f), color }, // Top-right
		{ glm::vec2(-0.5f,  0.5f), color }, // Top-left
		{ glm::vec2(-0.5f, -0.5f), color }  // Bottom-left
	};

	Model mdl;
	GLuint vbo_hdl;
	GLuint vaoid;
	GLuint ebo_hdl;

	glCreateBuffers(1, &vbo_hdl);
	glNamedBufferStorage(vbo_hdl, sizeof(Vertex) * vertices.size(), vertices.data(), GL_DYNAMIC_STORAGE_BIT);

	glCreateVertexArrays(1, &vaoid);
	glEnableVertexArrayAttrib(vaoid, 0);
	glVertexArrayVertexBuffer(vaoid, 0, vbo_hdl, offsetof(Vertex, position), sizeof(Vertex));
	glVertexArrayAttribFormat(vaoid, 0, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoid, 0, 0);

	glEnableVertexArrayAttrib(vaoid, 1);
	glVertexArrayVertexBuffer(vaoid, 1, vbo_hdl, offsetof(Vertex, color), sizeof(Vertex));
	glVertexArrayAttribFormat(vaoid, 1, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoid, 1, 1);

	std::array<GLushort, 4> idx_vtx = { 0, 1, 2, 3 };
	glCreateBuffers(1, &ebo_hdl);
	glNamedBufferStorage(ebo_hdl, sizeof(GLushort) * idx_vtx.size(), idx_vtx.data(), GL_DYNAMIC_STORAGE_BIT);
	glVertexArrayElementBuffer(vaoid, ebo_hdl);

	mdl.vaoid = vaoid;
	mdl.primitive_type = GL_LINE_LOOP;  // Use GL_LINE_LOOP for outline
	mdl.draw_cnt = static_cast<GLsizei>(idx_vtx.size());
	mdl.primitive_cnt = vertices.size();

	return mdl;
}


// Function to create an AABB cube
Model AABB(glm::vec3 color)
{
	struct Vertex {
		glm::vec3 position;  // 3D position
		glm::vec3 color;     // Color
	};


	// Cube vertices with positions and a uniform color
	std::vector<Vertex> vertices{
		// Front face
		{ glm::vec3(1.f,  1.f,  1.f), color },  // Top-right front
		{ glm::vec3(-1.f,  1.f,  1.f), color },  // Top-left front
		{ glm::vec3(-1.f, -1.f,  1.f), color },  // Bottom-left front
		{ glm::vec3(1.f, -1.f,  1.f), color },  // Bottom-right front

		// Back face
		{ glm::vec3(1.f,  1.f, -1.f), color },  // Top-right back
		{ glm::vec3(-1.f,  1.f, -1.f), color },  // Top-left back
		{ glm::vec3(-1.f, -1.f, -1.f), color },  // Bottom-left back
		{ glm::vec3(1.f, -1.f, -1.f), color },  // Bottom-right back
	};

	// Indices for drawing cube edges (outline)
	std::vector<GLushort> indices{
		// Front face
		0, 1, 2, 3, 0,

		// Back face
		4, 5, 6, 7, 4,

		// Connect front and back
		0, 4, 1, 5, 2, 6, 3, 7
	};

	// Create and set up VAO, VBO, EBO
	GLuint vbo_hdl, ebo_hdl, vaoid;
	glCreateBuffers(1, &vbo_hdl);
	glNamedBufferStorage(vbo_hdl, sizeof(Vertex) * vertices.size(), vertices.data(), GL_DYNAMIC_STORAGE_BIT);

	glCreateVertexArrays(1, &vaoid);
	glEnableVertexArrayAttrib(vaoid, 0);
	glVertexArrayVertexBuffer(vaoid, 0, vbo_hdl, offsetof(Vertex, position), sizeof(Vertex));
	glVertexArrayAttribFormat(vaoid, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoid, 0, 0);

	glEnableVertexArrayAttrib(vaoid, 1);
	glVertexArrayVertexBuffer(vaoid, 1, vbo_hdl, offsetof(Vertex, color), sizeof(Vertex));
	glVertexArrayAttribFormat(vaoid, 1, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoid, 1, 1);

	glCreateBuffers(1, &ebo_hdl);
	glNamedBufferStorage(ebo_hdl, sizeof(GLushort) * indices.size(), indices.data(), GL_DYNAMIC_STORAGE_BIT);
	glVertexArrayElementBuffer(vaoid, ebo_hdl);

	// Model structure to return
	Model mdl;
	mdl.vaoid = vaoid;
	mdl.primitive_type = GL_LINE_LOOP;  // Use GL_LINE_LOOP for the cube's outline
	mdl.draw_cnt = static_cast<GLsizei>(indices.size());

	return mdl;
}




/* Draw */


void Model::Draw2D(OpenGLShader& shader)
{

	glBindVertexArray(vaoid);
	//std::cout << vaoid << '\n';
	glDrawElements(primitive_type, draw_cnt, GL_UNSIGNED_SHORT, nullptr);

	glBindVertexArray(0);
	//shader.UnUse();
}


void Model::DrawCollisionBox2D(Model outlineModel)
{
	// Bind the VAO for the outline model



	glBindVertexArray(outlineModel.vaoid);
//	std::cout << outlineModel.vaoid << '\n';
	
	glLineWidth(15.0f);

// Draw the square outline
	glDrawElements(outlineModel.primitive_type, outlineModel.draw_cnt, GL_UNSIGNED_SHORT, 0);

	// Unbind the VAO
	glBindVertexArray(0);



}


void Model::DrawCollisionBox3D(Model outlineModel)
{
	// Bind the VAO for the outline model



	glBindVertexArray(outlineModel.vaoid);
	//	std::cout << outlineModel.vaoid << '\n';

	glLineWidth(15.0f);

	// Draw the square outline
	glDrawElements(outlineModel.primitive_type, outlineModel.draw_cnt, GL_UNSIGNED_SHORT, 0);

	// Unbind the VAO
	glBindVertexArray(0);



}