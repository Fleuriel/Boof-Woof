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
    std::cout << "load 1 : " << GetTextureDDS("texture1.dds");
    std::cout << "load 2 : " << GetTextureDDS("texture2.dds");
    std::cout << "load 3 : " << GetTextureDDS("texture3.dds");

    return true;
}

// Function to load textures (simulating DDS texture loading here)
bool ResourceManager::LoadTexturesDDS() {
    // Simulating loading textures by storing some dummy data in the map
    // In real use case, you would load actual DDS texture files here.
    texturesDDS["texture1.dds"] = 1;
    texturesDDS["texture2.dds"] = 2;
    texturesDDS["texture3.dds"] = 3;

    // For now, returning true to indicate successful loading
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
