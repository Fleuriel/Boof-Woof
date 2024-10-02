#include "Model.h"
#include <array>
//
//
//

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
	mdl.vaoid = vaoid;
	mdl.primitive_type = GL_TRIANGLE_FAN; // Use GL_TRIANGLE_FAN for a square
	mdl.draw_cnt = static_cast<GLsizei>(idx_vtx.size());
	mdl.primitive_cnt = vertices.size();


	// return model
	return mdl;
}





void Model::Draw2D(OpenGLShader& shader)
{
	//shader.Use();
	int tex;
	tex = g_AssetManager.GetTexture("Sadge");


	shader.SetUniform("uTex2d", 6);
	//shader.SetUniform("")

	glBindTextureUnit(6, tex);

	glBindVertexArray(vaoid);

	//std::cout << vaoid << '\n';
	glDrawElements(primitive_type, draw_cnt, GL_UNSIGNED_SHORT, nullptr);

	glBindVertexArray(0);
	//shader.UnUse();
}


void Model::DrawCollisionBox(glm::vec3 position)
{
	glm::vec3 min, max;

	glm::vec3 half_extents = glm::vec3(0.6f);

	min = position - half_extents;
	max = position + half_extents;



	// Compute the 8 vertices of the AABB
	glm::vec3 v1 = glm::vec3(min.x, min.y, min.z);
	glm::vec3 v2 = glm::vec3(min.x, min.y, max.z);
	glm::vec3 v3 = glm::vec3(min.x, max.y, min.z);
	glm::vec3 v4 = glm::vec3(min.x, max.y, max.z);
	glm::vec3 v5 = glm::vec3(max.x, min.y, min.z);
	glm::vec3 v6 = glm::vec3(max.x, min.y, max.z);
	glm::vec3 v7 = glm::vec3(max.x, max.y, min.z);
	glm::vec3 v8 = glm::vec3(max.x, max.y, max.z);

	// Use OpenGL to draw the edges of the AABB
	glBegin(GL_LINES);

	// Bottom square (v1, v5, v6, v2)
	glVertex3f(v1.x, v1.y, v1.z); glVertex3f(v5.x, v5.y, v5.z); // Edge v1 -> v5
	glVertex3f(v5.x, v5.y, v5.z); glVertex3f(v6.x, v6.y, v6.z); // Edge v5 -> v6
	glVertex3f(v6.x, v6.y, v6.z); glVertex3f(v2.x, v2.y, v2.z); // Edge v6 -> v2
	glVertex3f(v2.x, v2.y, v2.z); glVertex3f(v1.x, v1.y, v1.z); // Edge v2 -> v1

	// Top square (v3, v7, v8, v4)
	glVertex3f(v3.x, v3.y, v3.z); glVertex3f(v7.x, v7.y, v7.z); // Edge v3 -> v7
	glVertex3f(v7.x, v7.y, v7.z); glVertex3f(v8.x, v8.y, v8.z); // Edge v7 -> v8
	glVertex3f(v8.x, v8.y, v8.z); glVertex3f(v4.x, v4.y, v4.z); // Edge v8 -> v4
	glVertex3f(v4.x, v4.y, v4.z); glVertex3f(v3.x, v3.y, v3.z); // Edge v4 -> v3

	// Vertical edges (v1 -> v3, v2 -> v4, v5 -> v7, v6 -> v8)
	glVertex3f(v1.x, v1.y, v1.z); glVertex3f(v3.x, v3.y, v3.z); // Edge v1 -> v3
	glVertex3f(v2.x, v2.y, v2.z); glVertex3f(v4.x, v4.y, v4.z); // Edge v2 -> v4
	glVertex3f(v5.x, v5.y, v5.z); glVertex3f(v7.x, v7.y, v7.z); // Edge v5 -> v7
	glVertex3f(v6.x, v6.y, v6.z); glVertex3f(v8.x, v8.y, v8.z); // Edge v6 -> v8

	glEnd();
}