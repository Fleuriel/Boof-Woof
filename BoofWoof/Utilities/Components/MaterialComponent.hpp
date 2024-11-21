#ifndef MATERIAL_COMPONENT_HPP
#define MATERIAL_COMPONENT_HPP

#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include "../Core/AssetManager/Descriptor.h"



class MaterialComponent {
public:
    // Constructors
//    MaterialComponent() = default;
    MaterialComponent(){}


    // Assignment operator
  //  MaterialComponent& operator=(const MaterialComponent& other) {
  //      if (this != &other) {
  //          shaderName = other.shaderName;
  //          color = other.color;
  //          textureID = other.textureID;
  //          shininess = other.shininess;
  //      }
  //      return *this;
  //  }


    // Setters and Getters
//    void SetShaderName(const std::string& shader) { shaderName = shader; }
    
    void SetColor(const glm::vec4& baseColor) 
    { 
        materialDesc.albedoColorRed = baseColor.r;
        materialDesc.albedoColorGreen = baseColor.g;
        materialDesc.albedoColorBlue = baseColor.b;
        materialDesc.albedoColorAlpha = baseColor.a;   
    }
    
    void SetRed(float r) { materialDesc.albedoColorRed = r; }
    void SetGreen(float g) { materialDesc.albedoColorGreen = g; }
    void SetBlue(float b) { materialDesc.albedoColorBlue = b; }
    void SetAlpha(float a) { materialDesc.albedoColorAlpha = a; }

    void SetTexture(int tex) { materialDesc.textureID = tex; }
    
    MaterialDescriptor GetMaterial() { return materialDesc; }


    bool loadedMaterial = false;


    //void SetTextureID(int texID) {
    //    textureID = (texID >= 0) ? texID : -1; // Ensure textureID is valid
    //}
    //
    //void SetShininess(float shiny) {
    //    shininess = (shiny < 0.0f) ? 0.0f : shiny; // Ensure shininess is non-negative
    //}

   // const std::string& GetShaderName() const { return shaderName; }
    const glm::vec4& GetColor() const { return glm::vec4(materialDesc.albedoColorRed, materialDesc.albedoColorGreen, materialDesc.albedoColorBlue, materialDesc.albedoColorAlpha); }
   // int GetTextureID() const { return textureID; }
   // float GetShininess() const { return shininess; }

    MaterialDescriptor materialDesc;
private:
};
#endif;