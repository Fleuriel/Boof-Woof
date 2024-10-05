#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>
#include "Texture.h"

// Add your preferred image loading library (e.g., stb_image, SOIL, etc.)
// #define STB_IMAGE_IMPLEMENTATION
// #include "stb_image.h"

Texture::Texture(const std::string& filePath) {
    if (!LoadFromFile(filePath)) {
        std::cerr << "Failed to load texture: " << filePath << std::endl;
    }
}

bool Texture::LoadFromFile(const std::string& filePath) {
    // Use an image loading library (e.g., stb_image) to load the file
    // Example with stb_image:
    /*
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
    if (data) {
        pixelData.assign(data, data + (width * height * channels));
        stbi_image_free(data); // Free the loaded data
        return true;
    } else {
        std::cerr << "Failed to load image: " << filePath << std::endl;
        return false;
    }
    */
    return false; // Placeholder, you need to load the actual texture data here
}

