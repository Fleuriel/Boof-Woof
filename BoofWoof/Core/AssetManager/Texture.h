#ifndef TEXTURE_H
#define TEXTURE_H


#include <string>
#include <vector>

class Texture {
public:
    unsigned int id;          // OpenGL texture ID (or any other rendering engine)
    int width;
    int height;
    int channels;             // Number of channels (e.g., 3 for RGB, 4 for RGBA)
    std::string filePath;     // File path of the texture
    std::vector<unsigned char> pixelData; // Raw pixel data for the texture

    Texture() : id(0), width(0), height(0), channels(0) {}

    // Constructor to load texture from file
    Texture(const std::string& filePath);

    // Load texture data from file
    bool LoadFromFile(const std::string& filePath);

    // Getters
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    int GetChannels() const { return channels; }
    const unsigned char* GetPixelData() const { return pixelData.data(); }
};

#endif // !TEXTURE_H