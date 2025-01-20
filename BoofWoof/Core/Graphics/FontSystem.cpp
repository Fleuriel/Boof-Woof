#include "pch.h"
#include "FontSystem.h"
#include <stb_image.h>
#include "AssetManager/FilePaths.h"

#pragma warning(push)
#pragma warning(disable: 5054)

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include "ResourceManager/ResourceManager.h"
#include <fstream>

FontSystem fontSystem;



void FontSystem::init()
{
	saveBin();
    
	// open bin file
	std::ifstream ifs("font.bin", std::ios::binary);
	if (!ifs.is_open())
	{
		std::cout << "ERROR::FREETYPE: Failed to open file" << std::endl;
		return; 
	}

    

    // disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // load first 128 characters of ASCII set
    for (unsigned char c = 0; c < 128; c++)
    {
        int width{}, height{}, left, top;
		unsigned int advance;
		ifs.read((char*)&width, sizeof(int));
		ifs.read((char*)&height, sizeof(int));
		ifs.read((char*)&left, sizeof(int));
		ifs.read((char*)&top, sizeof(int));
		ifs.read((char*)&advance, sizeof(unsigned int));
        char* buffer = new char[width * height];
		ifs.read(buffer, width * height);

        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
			width,
			height,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture,
			glm::ivec2(width, height),
			glm::ivec2(left, top),
			static_cast<unsigned int>(advance)
        };
        Characters.insert(std::pair<char, Character>(c, character));
        //std::cout << "Loaded Glyph: " << c << ", Size: " << face->glyph->bitmap.width << "x" << face->glyph->bitmap.rows << std::endl;
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    

    // configure VAO/VBO for texture quads
    // -----------------------------------
    glGenVertexArrays(1, &VAO_FONT);
    glGenBuffers(1, &VBO_FONT);
    glBindVertexArray(VAO_FONT);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_FONT);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

	std::cout << "Font System Initialized" << std::endl;    

}

void FontSystem::saveBin()
{
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return;
    }

    // find path to font
    std::string font_name = FILEPATH_RESOURCE_FONTS + "/arial.ttf";
    if (font_name.empty())
    {
        std::cout << "ERROR::FREETYPE: Failed to load font_name" << std::endl;
        return;
    }

	// create bin file
    
	std::ofstream ofs("font.bin", std::ios::binary);
	if (!ofs.is_open())
	{
		std::cout << "ERROR::FREETYPE: Failed to open file" << std::endl;
		return;
	}

    // load font as face
    FT_Face face;
    if (FT_New_Face(ft, font_name.c_str(), 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return;
    }
    else {
        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, 48);

        
        // load first 128 characters of ASCII set
        for (unsigned char c = 0; c < 128; c++)
        {
            // Load character glyph  
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }
            int width = face->glyph->bitmap.width;
            int height = face->glyph->bitmap.rows;
            int left = face->glyph->bitmap_left;
            int top = face->glyph->bitmap_top;
            unsigned int advance = face->glyph->advance.x;
			char* buffer = (char*)face->glyph->bitmap.buffer;

			// write to file
			ofs.write((char*)&width, sizeof(int));
			ofs.write((char*)&height, sizeof(int));
			ofs.write((char*)&left, sizeof(int));
			ofs.write((char*)&top, sizeof(int));
			ofs.write((char*)&advance, sizeof(unsigned int));
			ofs.write(buffer, width * height);
			

        }
        FT_Done_Face(face);
        FT_Done_FreeType(ft);

    }
	std::cout << "Font System Saved" << std::endl;

}

void FontSystem::RenderText(OpenGLShader& shader, std::string text, float x, float y, float scale, glm::vec3 color)
{
	static bool init = true;
    // activate corresponding render state	
    shader.Use();
    shader.SetUniform("textColor", color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO_FONT);

    // iterate through all characters
    std::string::const_iterator c;
    
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO_FONT);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))

		//// print the vetices
		//if (init)
		//{
		//	for (int i = 0; i < 6; i++)
		//	{
  //              // keep 2 dp
		//		std::cout << "Vertex: " << i << " x: " << std::fixed << std::setprecision(2) << vertices[i][0] << " y: " << std::fixed << std::setprecision(2) << vertices[i][1] << " s: " << std::fixed << std::setprecision(2) << vertices[i][2] << " t: " << std::fixed << std::setprecision(2) << vertices[i][3] << std::endl;
		//	}
		//}
    }
    init = false;
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    shader.UnUse();
}







#pragma warning(pop)