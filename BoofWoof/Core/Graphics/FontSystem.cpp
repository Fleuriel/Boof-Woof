//#include "Fontsystem.h"
//
//
//Font* FontSystem = NULL;
//Font::Font() {
//}
//
////FT_Library ft;
//namespace
//{
//
//    //for opengl api
//    GLuint VAO, VBO;
//
//    //to keep track of the current font being used
//    outline* fontTracker = nullptr;
//
//    FT_Error error;
//}
//
////helper function to compile and link shaders
////void init_shaders();
//
////helper function to normailise coordinates
//void normalise_coord(float& x, float& y);
//
////helper function to draw text
//void RenderText(std::string text, float x, float y, float scale, glm::ivec3 color);
//
////fonts:
////Aldrich-Regular
//void SetFont(std::string font)
//{
//    fontTracker = AssetManager::getoutline(font);
//}
//
///******************************************************************************
//    loads font assets, initilise shaders,
//    configure opengl context to draw texture for the shader program
//*******************************************************************************/
//void Font::Initialize()
//{
//
//}
//
///******************************************************************************
//    to draw the font texture, specified by the parameters
//*******************************************************************************/
//bool DrawText(std::string const& text, float posX, float posY, float scale, float red, float green, float blue)
//{
//    for (size_t pos1{}, pos2{ 1 }; pos2 != std::string::npos; posY -= FontSystem->pixel_height)
//    {
//        pos2 = text.find_first_of('\n', pos1);
//        std::string line{ text.substr(pos1 , pos2 - pos1) };
//        RenderText(line, posX, posY, scale, glm::vec3(red, green, blue));
//        pos1 = pos2 + 1;
//    }
//    return true;
//}
//
///******************************************************************************
//    to calculate the total width of the text string to be drawn, in pixels,
//    depending on what the text string is, in the default font size
//*******************************************************************************/
//int find_width(std::string const& str, std::string font, float scale)
//{
//    int width{};
//    for (size_t i = 0; i < str.size(); ++i)
//    {
//
//        FT_Load_Char(AssetManager::getoutline(font)->face, str.at(i), FT_LOAD_RENDER);
//        width += AssetManager::getoutline(font)->face->glyph->advance.x;
//    }
//    return static_cast<int>((width >> 6) * scale);
//
//}
//
//
//int find_height(std::string const&, std::string font, float scale)
//{
//    return static_cast<int>(scale * FontSystem->pixel_height);
//
//}
//
////normoalise coordinates where centre is 0 , 0
//void normalise_coord(float& x, float& y)
//{
//    x += window->width_init / 2.0f;
//    y += window->height_init / 2.0f;
//}
//
////draw the text with opengl api
//void RenderText(std::string text, float x, float y, float scale, glm::ivec3 color)
//{
//    normalise_coord(x, y);
//    AssetManager::shaderval("font").Use();
//    // activate corresponding render 
//    glUniform3f(glGetUniformLocation(AssetManager::shaderval("font").GetHandle(), "textColor"), (float)color.x, (float)color.y, (float)color.z);
//    glActiveTexture(GL_TEXTURE0);
//    glBindVertexArray(VAO);
//
//    // iterate through all characters
//    for (char c : text)
//    {
//        Character ch = fontTracker->Characters[c];
//
//        float xpos = x + ch.Bearing.x * scale;
//        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
//
//        float w = ch.Size.x * scale;
//        float h = ch.Size.y * scale;
//        // update VBO for each character
//        float vertices[6][4] = {
//            { xpos,     ypos + h,   0.0f, 0.0f },
//            { xpos,     ypos,       0.0f, 1.0f },
//            { xpos + w, ypos,       1.0f, 1.0f },
//
//            { xpos,     ypos + h,   0.0f, 0.0f },
//            { xpos + w, ypos,       1.0f, 1.0f },
//            { xpos + w, ypos + h,   1.0f, 0.0f }
//        };
//        // render glyph texture over quad
//        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
//        // update content of VBO memory
//        glBindBuffer(GL_ARRAY_BUFFER, VBO);
//        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
//        glBindBuffer(GL_ARRAY_BUFFER, 0);
//        // render quad
//        glDrawArrays(GL_TRIANGLES, 0, 6);
//        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
//        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
//    }
//    glBindVertexArray(0);
//    glBindTexture(GL_TEXTURE_2D, 0);
//    AssetManager::shaderval("font").UnUse();
//}
//
//void Font::Update()
//{
//}
//
//Font::~Font()
//{
//    /*
//    for (int i = 0; i < total; ++i)
//    {
//       // FT_Done_Face(fontOutlines[i].face);
//    }
//    */
//    // FT_Done_FreeType(ft);
//}
//
////load the ascii characters into the container of characters
//void outline::load_ascii_chars()
//{
//    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//    // load first 128 characters of ASCII set
//    for (unsigned char c = 0; c < 128; c++)
//    {
//        // Load character glyph 
//        error = (FT_Load_Char(face, c, FT_LOAD_RENDER));
//        if (error)
//        {
//            std::cout << "ERROR::FREETYTPE: Failed to load Glyph!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
//        }
//        // generate texture 
//        unsigned int texture;
//        glGenTextures(1, &texture);
//        glBindTexture(GL_TEXTURE_2D, texture);
//        glTexImage2D(
//            GL_TEXTURE_2D,
//            0,
//            GL_RED,
//            face->glyph->bitmap.width,
//            face->glyph->bitmap.rows,
//            0,
//            GL_RED,
//            GL_UNSIGNED_BYTE,
//            face->glyph->bitmap.buffer
//        );
//        // set texture options
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        // now store character for later use
//        Character charac = {
//            texture,
//            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
//            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
//            static_cast<unsigned int>(face->glyph->advance.x)
//        };
//        Characters.insert(std::pair<char, Character>(c, charac));
//    }
//    glBindTexture(GL_TEXTURE_2D, 0);
//}
//
////set normalise pixel size
//void outline::set_pixel_size(int size)
//{
//    FT_Set_Pixel_Sizes(face, 0, size);
//}
//
//void setup_font_vao(GLuint vaoid, GLuint vboid)
//{
//    ::VAO = vaoid;
//    ::VBO = vboid;
//}
