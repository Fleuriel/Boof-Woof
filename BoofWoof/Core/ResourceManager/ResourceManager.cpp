#include "ResourceManager.h"
#include <iostream>

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

// Function to load textures (simulating DDS texture loading here)
bool ResourceManager::LoadTexturesDDS() {

    for (int i = 0; i < textureDDSFileNames.size(); i++) {

        //add DDS processing here

        texturesDDS[textureDDSFileNames[i]] = 69;
        std::cout << "Texture DDS File Added : " << textureDDSFileNames[i] << std::endl;
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
