#ifndef MATERIAL_COMPONENT_HPP
#define MATERIAL_COMPONENT_HPP

#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>


class MaterialComponent {
public:
    // Constructors
    MaterialComponent() = default;
    MaterialComponent(const std::string& shader, const glm::vec4& baseColor, int texID, float shiny)
        : shaderName(shader), color(baseColor), textureID(texID), shininess(shiny) {
    }

    // Assignment operator
    MaterialComponent& operator=(const MaterialComponent& other) {
        if (this != &other) {
            shaderName = other.shaderName;
            color = other.color;
            textureID = other.textureID;
            shininess = other.shininess;
        }
        return *this;
    }

    // Setters and Getters
    void SetShaderName(const std::string& shader) { shaderName = shader; }
    void SetColor(const glm::vec4& baseColor) { color = baseColor; }
    void SetTextureID(int texID) { textureID = texID; }
    void SetShininess(float shiny) { shininess = shiny; }

    const std::string& GetShaderName() const { return shaderName; }
    const glm::vec4& GetColor() const { return color; }
    int GetTextureID() const { return textureID; }
    float GetShininess() const { return shininess; }

private:
    std::string shaderName;
    glm::vec4 color;
    int textureID;
    float shininess;
};
#endif;