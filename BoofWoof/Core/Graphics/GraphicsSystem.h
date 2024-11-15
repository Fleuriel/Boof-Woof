#pragma once

#ifndef GRAPHICS_SYSTEM_H
#define GRAPHICS_SYSTEM_H

#include "../Utilities/Components/GraphicsComponent.hpp"
#include "../Utilities/Components/TransformComponent.hpp"
#include "Camera.h"
#include "Animation/AnimationManager.h"

class Model;
class Model2D;
class GraphicsComponent;

#include "../ECS/System.hpp"


struct ShaderParams {

    glm::mat4 WorldMatrix;
    glm::mat4 View;
    glm::mat4 Projection;
    glm::vec3 Color;
    

    ShaderParams() : WorldMatrix(glm::mat4(0.0f)), View(glm::mat4(0.0f)), Projection(glm::mat4(0.0f)), Color(glm::vec3(0.1f, 0.2f, 0.3f)) {}

};


class GraphicsSystem : public System
{
public:
    void initGraphicsPipeline();
    void UpdateLoop();
    void Draw(std::vector<GraphicsComponent>& components);
    void DrawObject(GraphicsComponent& component);
    
    static bool glewInitialized;
    static bool debug;
    static bool D2; // 0 is 2D, 1 is 3D
    static bool D3; // 0 is 2D, 1 is 3D
    
    
    int set_Texture_T;
    static int set_Texture_;

    std::string textureNameInput;

    void SetShaderUniforms(OpenGLShader& shader, const ShaderParams& shdrParam);

    GLuint GetFramebufferTexture() const { return textureColorbuffer; }  // Getter function

    //void AddModel_3D(std::string const& path);
    void AddObject_3D(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, glm::vec3 color, Model* model);

    void AddModel_2D();

    void UpdateViewportSize(int width, int height);  // Method to handle viewport resizing
    inline void SetEditorMode(bool EditorMode) { editorMode = EditorMode; };
	Camera& GetCamera() { return camera; };

	glm::vec3 GetLightPos() { return lightPos; };
	void SetLightPos(glm::vec3 pos) { lightPos = pos; };

    bool DrawMaterialSphere();

    void generateNewFrameBuffer(unsigned int& fbo, unsigned int& textureColorbuffer, unsigned int& rbo, int width, int height);



    void clearAllEntityTextures();



    // GraphicsSystem() : fbo(0), textureColorbuffer(0), rbo(0){}


private:
    unsigned int fbo;
    unsigned int textureColorbuffer;  // Store the framebuffer texture ID here
    unsigned int rbo;
    bool editorMode = false;
    static GLFWwindow* newWindow;  // OpenGL window
	static Camera camera;
    static glm::vec3 lightPos;

};

#endif