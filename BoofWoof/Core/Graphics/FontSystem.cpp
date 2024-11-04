#include "pch.h"
#include "FontSystem.h"
#include <stb_image.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include "ResourceManager/ResourceManager.h"


FontSystem fontSystem;

void FontSystem::init()
{
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return ;
    }

    // find path to font
    std::string font_name = "../BoofWoof/Assets/Font/arial.ttf";
    if (font_name.empty())
    {
        std::cout << "ERROR::FREETYPE: Failed to load font_name" << std::endl;
        return ;
    }

    // load font as face
    FT_Face face;
    if (FT_New_Face(ft, font_name.c_str(), 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return ;
    }
    else {
        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, 48);

        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // load first 128 characters of ASCII set
        for (unsigned char c = 0; c < 128; c++)
        {
            // Load character glyph 
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }
            // generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // now store character for later use
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            Characters.insert(std::pair<char, Character>(c, character));
            //std::cout << "Loaded Glyph: " << c << ", Size: " << face->glyph->bitmap.width << "x" << face->glyph->bitmap.rows << std::endl;
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    // destroy FreeType once we're finished
   FT_Done_Face(face);
    FT_Done_FreeType(ft);


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



void FontSystem::init_font()
{
	// Load font metadata from JSON file
	glyphs = loadFontMetadata("../BoofWoof/Resources/Fonts/arial.json");

    // get texture id
	font_textureid = g_ResourceManager.GetTextureDDS("arial");

	// Configure VAO/VBO for texture quads
	glGenVertexArrays(1, &VAO_FONT);
	glGenBuffers(1, &VBO_FONT);
	glBindVertexArray(VAO_FONT);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_FONT);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

std::unordered_map<GLchar, Glyph> FontSystem::loadFontMetadata(const std::string& jsonPath)
{
    std::unordered_map<GLchar, Glyph> glyphs;

    // Open and parse the JSON file
    std::ifstream ifs(jsonPath);
    if (!ifs.is_open()) {
        std::cerr << "Failed to open JSON file." << std::endl;
        return glyphs;
    }

    rapidjson::IStreamWrapper isw(ifs);
    rapidjson::Document document;
    document.ParseStream(isw);

    if (!document.HasMember("glyphs") || !document["glyphs"].IsArray()) {
        std::cerr << "Invalid JSON format: 'glyphs' array missing." << std::endl;
        return glyphs;
    }

    // Iterate over each glyph in the JSON "glyphs" array
    for (const auto& glyph : document["glyphs"].GetArray()) {
        Glyph g;

        // Load advance
        if (glyph.HasMember("advance") && glyph["advance"].IsNumber()) {
            g.advance = glyph["advance"].GetFloat();
        }

        // Load planeBounds (left, bottom, right, top)
        if (glyph.HasMember("planeBounds")) {
            const auto& pb = glyph["planeBounds"];
            g.planeBounds[0] = pb["left"].GetFloat();
            g.planeBounds[1] = pb["bottom"].GetFloat();
            g.planeBounds[2] = pb["right"].GetFloat();
            g.planeBounds[3] = pb["top"].GetFloat();
        }

        // Load atlasBounds (left, bottom, right, top)
        if (glyph.HasMember("atlasBounds")) {
            const auto& ab = glyph["atlasBounds"];
            g.atlasBounds[0] = ab["left"].GetFloat();
            g.atlasBounds[1] = ab["bottom"].GetFloat();
            g.atlasBounds[2] = ab["right"].GetFloat();
            g.atlasBounds[3] = ab["top"].GetFloat();
        }

        // Load glyph index and store it in the map
        if (glyph.HasMember("index") && glyph["index"].IsInt()) {
			char character = static_cast<char>(glyph["index"].GetInt());
            glyphs[character] = g;
        }
    }

    return glyphs;
}

void FontSystem::render_text(OpenGLShader& shader, std::string text, float x, float y, float scale, glm::vec3 color)
{
    shader.Use();
    shader.SetUniform("textColor", color.x, color.y, color.z);
    shader.SetUniform("text", 0);  // Ensure sampler is bound to texture unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font_textureid);  // Bind the font texture
    glBindVertexArray(VAO_FONT);

    // Iterate through all characters in the string
    for (const char& character : text)
    {
        if (glyphs.find(character) == glyphs.end()) continue; // Skip if glyph not found

        const Glyph& glyph = glyphs[character];

        // Calculate position and size of character quad
        float xpos = x + glyph.planeBounds[0] * scale;
        float ypos = y - (glyph.planeBounds[3] - glyph.planeBounds[1]) * scale;
        float w = (glyph.planeBounds[2] - glyph.planeBounds[0]) * scale;
        float h = (glyph.planeBounds[3] - glyph.planeBounds[1]) * scale;

        // Texture coordinates in the atlas
        float tx = glyph.atlasBounds[0] / 1436.0f;  // Adjust based on your atlas width
        float ty = glyph.atlasBounds[1] / 1436.0f;  // Adjust based on your atlas height
        float tw = (glyph.atlasBounds[2] - glyph.atlasBounds[0]) / 1436.0f;
        float th = (glyph.atlasBounds[3] - glyph.atlasBounds[1]) / 1436.0f;

        // Update VBO with the character quad data
        float vertices[6][4] = {
            { xpos,     ypos + h,   tx,        ty        },
            { xpos,     ypos,       tx,        ty + th   },
            { xpos + w, ypos,       tx + tw,   ty + th   },

            { xpos,     ypos + h,   tx,        ty        },
            { xpos + w, ypos,       tx + tw,   ty + th   },
            { xpos + w, ypos + h,   tx + tw,   ty        }
        };

        // Bind the VBO and update data
        glBindBuffer(GL_ARRAY_BUFFER, VBO_FONT);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        // Render the character quad
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Advance the cursor to the start of the next character
        x += glyph.advance * scale;
    }

    // Clean up state
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    shader.UnUse();
}



