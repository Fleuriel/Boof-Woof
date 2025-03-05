#include "pch.h"
#include "../Graphics/DebugRenderer.h"
#include "../AssetManager/AssetManager.h"

constexpr int MAX_VERTICES = 1024 * 6;  // Large enough buffer size
constexpr int MAX_INDICES = 1024 * 8;

DebugRenderer::DebugRenderer() {}

DebugRenderer::~DebugRenderer()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &IBO);
}

void DebugRenderer::Initialize()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &IBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_INDICES * sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);

    // Position Attribute (vec3)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);
    glEnableVertexAttribArray(0);

    // Color Attribute (vec3)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

}

void DebugRenderer::Begin()
{
    vertexBuffer.clear();
    indexBuffer.clear();
}

void DebugRenderer::SubmitAABB(const glm::vec3& center, const glm::vec3& halfExtents, const glm::vec3& color)
{
    static const std::vector<glm::vec3> unitCube = {
        {-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1}, // Back Face
        {-1, -1,  1}, {1, -1,  1}, {1, 1,  1}, {-1, 1,  1}  // Front Face
    };

    static const std::vector<GLuint> cubeIndices = {
        0, 1, 1, 2, 2, 3, 3, 0,  // Back
        4, 5, 5, 6, 6, 7, 7, 4,  // Front
        0, 4, 1, 5, 2, 6, 3, 7   // Connecting Lines
    };

    GLuint startIndex = vertexBuffer.size() / 6;

    for (const auto& point : unitCube)
    {
        glm::vec3 transformedPoint = center + halfExtents * point;
        vertexBuffer.push_back(transformedPoint.x);
        vertexBuffer.push_back(transformedPoint.y);
        vertexBuffer.push_back(transformedPoint.z);
        vertexBuffer.push_back(color.r);
        vertexBuffer.push_back(color.g);
        vertexBuffer.push_back(color.b);
    }

    for (GLuint index : cubeIndices)
    {
        indexBuffer.push_back(startIndex + index);
    }
}

void DebugRenderer::End()
{
    if (vertexBuffer.empty() || indexBuffer.empty()) return;

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertexBuffer.size() * sizeof(float), vertexBuffer.data());

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indexBuffer.size() * sizeof(GLuint), indexBuffer.data());
}

void DebugRenderer::Render()
{
    if (vertexBuffer.empty() || indexBuffer.empty()) return;

    debugShader->Use();
    glBindVertexArray(VAO);
    glDrawElements(GL_LINES, indexBuffer.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
