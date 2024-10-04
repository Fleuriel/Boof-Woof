#include "ResourceManager.h"
#include <iostream>
#include <GL/glew.h>
#include "AssetManager/FilePaths.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STBI_ONLY_DDS // Enable support for DDS only, if you don't want other formats

ResourceManager g_ResourceManager;

// Constructor
ResourceManager::ResourceManager() {
    // Initialization if needed
}

// Destructor
ResourceManager::~ResourceManager() {
    // Cleanup resources if necessary
}

bool ResourceManager::LoadAll() {
    LoadTexturesDDS();
    //std::cout << "load 1 : " << GetTextureDDS("texture1.dds");
    //std::cout << "load 2 : " << GetTextureDDS("texture2.dds");
    //std::cout << "load 3 : " << GetTextureDDS("texture3.dds");

    return true;
}

int SetUpTexture(const char* filename)
{
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);

    if (data)
    {
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Set texture wrapping/filtering options (on the currently bound texture)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Load the texture into OpenGL
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB; // Detect format
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Free the image memory after uploading it to GPU
        stbi_image_free(data);

        return textureID;
    }
    else
    {
        std::cerr << "Failed to load texture: " << filename << std::endl;
        return -1;
    }
}

// Function to load textures (simulating DDS texture loading here)
bool ResourceManager::LoadTexturesDDS() {

    for (int i = 0; i < textureDDSFileNames.size(); i++) {

        //add DDS processing here

        int result = SetUpTexture((FILEPATH_DDS + textureDDSFileNames[i] + ".dds").c_str());

        if (result != -1) {
            texturesDDS[textureDDSFileNames[i]] = result;
            std::cout << "Texture DDS File Added : " << textureDDSFileNames[i] << std::endl;
        }
    }
    // For now, returning true to indicate successful loading
    return true;
}

bool ResourceManager::AddTextureDDS(std::string textureName) {
    textureDDSFileNames.push_back(textureName);
    return true;
}

// Function to access textures
int ResourceManager::GetTextureDDS(std::string textureName) {
    // Check if the texture exists in the map
    if (texturesDDS.find(textureName) != texturesDDS.end()) {
        // Return the texture ID if found
        return texturesDDS[textureName];
    }
    else {
        // Return -1 or some error value if texture not found
        std::cerr << "Texture not found: " << textureName << std::endl;
        return -1;
    }
}
