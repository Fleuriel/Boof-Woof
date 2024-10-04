#include "Model.h"
#include <array>
#include <stb_image.h>

//
//
//

unsigned int TextureFromFile(const char* path, const std::string& directory)
{
	std::string filename = std::string(path);
    filename = directory + '/' + filename;

      unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
		GLenum format{};
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
       std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

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