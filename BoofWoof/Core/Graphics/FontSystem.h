
#pragma once
#ifndef FONTSYSTEM_H
#define FONTSYSTEM_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include <string>
#include "Shader.h"
#include <vector>
#include <map>
#include "../ECS/System.hpp"
#include FT_FREETYPE_H


/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};




//font system
class FontSystem : public System {
public:
    void init();
    void RenderText(OpenGLShader& shader, std::string text, float x, float y, float scale, glm::vec3 color);

private:
    std::map<GLchar, Character> Characters;
    unsigned int VAO, VBO;

};
extern FontSystem fontSystem;
#endif
