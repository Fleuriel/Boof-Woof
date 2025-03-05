#ifndef DEBUG_RENDERER_H
#define DEBUG_RENDERER_H

#include <vector>
#include <glm/glm.hpp>

class DebugRenderer
{
public:
    DebugRenderer();
    ~DebugRenderer();

    void Initialize();
    void Begin();
    void SubmitAABB(const glm::vec3& center, const glm::vec3& halfExtents, const glm::vec3& color);
    void End();
    void Render();

private:
    GLuint VAO{}, VBO{}, IBO{};
    std::vector<float> vertexBuffer;
    std::vector<GLuint> indexBuffer;
    OpenGLShader* debugShader = &g_AssetManager.GetShader("Debug");
};

#endif // DEBUG_RENDERER_H
