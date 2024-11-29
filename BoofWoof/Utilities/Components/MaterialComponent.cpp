#include "pch.h"
#include "../Utilities/Components/MaterialComponent.hpp"



#include <fstream>
#include <sstream>
#include <string>




MaterialComponent LoadMaterialFromFile(const std::string& filePath) {
    MaterialComponent material;
    std::ifstream file(filePath);

    std::cout << "it loadin\n";

    if (!file.is_open()) {
        std::cerr << "Failed to open material file: " << filePath << std::endl;
        return material; // Return a default instance if the file cannot be read
    }

    // std::string line;
    // while (std::getline(file, line)) {
    //     std::istringstream iss(line);
    //     std::string key;
    //     iss >> key;
    //
    //     if (key == "ShaderName") {
    //         std::string shaderName;
    //         iss >> shaderName;
    //         material.SetShaderName(shaderName);
    //     }
    //     else if (key == "Color") {
    //         float r, g, b, a;
    //         iss >> r >> g >> b >> a;
    //         material.SetColor(glm::vec4(r, g, b, a));
    //     }
    //     else if (key == "TextureID") {
    //         int textureID;
    //         iss >> textureID;
    //         material.SetTextureID(textureID);
    //     }
    //     else if (key == "Shininess") {
    //         float shininess;
    //         iss >> shininess;
    //         material.SetShininess(shininess);
    //     }
    // }
    //
    //
    // std::cout << material.GetColor().r <<'\t' << material.GetColor().g << '\t' << material.GetColor().b << '\t' << '\n';
    // std::cout << material.GetShaderName() << '\n';
    // std::cout << material.GetShininess() << '\n';
    // std::cout << material.GetTextureID() << '\n';


    file.close();
    return material;
}

void MaterialComponent::RegisterProperties()
{
    static bool registered = false;
    if (!registered)
    {
        RegisterAllProperties();  // Call the generated RegisterAllProperties function
        registered = true;
    }
}