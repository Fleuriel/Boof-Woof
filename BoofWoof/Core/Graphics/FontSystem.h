/**************************************************************************
 * @file Font.h
 * @author 	Guo Chen
 * @param DP email: g.chen@digipen.edu [2200518]
 * @param Course: CS 3401
 * @param Course: Game Project 3
 * @date  10/06/2024 (06 OCTOBER 2024)
 * @brief
 *
 * This file contains the definition of the FontSystem class, which is used to
 * render text on the screen.
 * 
 *************************************************************************/
#pragma once
#ifndef FONTSYSTEM_H
#define FONTSYSTEM_H

#include <ft2build.h>
#include "Shader.h"
#include "../ECS/System.hpp"
#include FT_FREETYPE_H



/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};

struct FontResources {
    std::map<GLchar, Character> Characters;
    GLuint font_textureid{};
    unsigned int VAO_FONT{}, VBO_FONT{};
};



//font system
class FontSystem : public System {
public:
    void init();
    FontResources readFromBin(const std::string& binFilename);
	std::string saveBin(std::string ttf_filename_noExtension);
	void update();
    //void RenderText(OpenGLShader& shader, std::string text, float x, float y, glm::vec2 scale, glm::vec3 color);
	void RenderText(FontResources fontResources, std::string text, float x, float y, glm::vec2 scale, glm::vec3 color);

private:
    /*std::map<GLchar, Character> Characters;
    GLuint font_textureid{};
    unsigned int VAO_FONT{}, VBO_FONT{};*/


};
extern FontSystem fontSystem;
#endif
