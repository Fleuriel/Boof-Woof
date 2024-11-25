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
    

    std::string& GetMaterialName() { return materialDesc.materialName; }
    std::string& GetMaterialNameMat() { return materialDesc.materialNameMat; }
    void SetMaterialName(std::string name) { materialDesc.materialName = name; std::cout << materialDesc.materialName << '\n'; }
    void SetMaterialNameMat(std::string name) { materialDesc.materialNameMat = name + ".mat"; std::cout << materialDesc.materialNameMat << '\n'; }

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

    int GetAlphaMaterial() { return materialDesc.materialAlpha; }
    
    void SetAlphaMaterial(int a) 
    {
        materialDesc.materialAlpha = a;
    }

    void SetFinalAlpha(float val) { materialDesc.finalAlpha = val; }
    float& GetFinalAlphaRef() { return materialDesc.finalAlpha; }
    float GetFinalAlpha() { return materialDesc.finalAlpha; }


    void SetDiffuseID(int tex) { materialDesc.DiffuseID = tex; }
    void SetNormalID(int norm) { materialDesc.NormalID = norm; }
    void SetHeightID(int height) { materialDesc.HeightID = height; }

    std::string GetDiffuseName() { return materialDesc.textureDiffuse; }
    std::string GetNormalName() { return materialDesc.textureNormal; }
    std::string GetHeightName() { return materialDesc.textureHeight; }

    void SetDiffuseName(std::string Name) { materialDesc.textureDiffuse = Name; }
    void SetNormalName(std::string Name) { materialDesc.textureNormal = Name; }
    void SetHeightName(std::string Name) { materialDesc.textureHeight = Name; }

    void RemoveDiffuse() { materialDesc.textureDiffuse = " "; }
    void RemoveNormal() { materialDesc.textureNormal = " "; }
    void RemoveHeight() { materialDesc.textureHeight = " "; }

    float GetMetallic() { return materialDesc.metallic; }
    float &GetMetallicRef() { return materialDesc.metallic; }
    void SetMetallic(float val) { materialDesc.metallic = val; }

    float GetSmoothness() { return materialDesc.smoothness; }
    float &GetSmoothnessRef() { return materialDesc.smoothness; }
    void SetSmoothness(float val) { materialDesc.smoothness = val; }

    float GetMaterialRed() { return materialDesc.albedoColorRed; }
    float GetMaterialGreen() { return materialDesc.albedoColorGreen; }
    float GetMaterialBlue() { return materialDesc.albedoColorBlue; }
    float GetMaterialAlpha() { return materialDesc.albedoColorAlpha; }
    glm::vec3 GetMaterialRGB() { return glm::vec3(materialDesc.albedoColorRed, materialDesc.albedoColorGreen, materialDesc.albedoColorBlue); }



    void SetShaderIndex(int index) { materialDesc.shaderIndex = index; }
    void SetShaderName(std::string name) { materialDesc.shaderChosen = name; }

    int& GetShaderIndexRef() { return materialDesc.shaderIndex; }
    int GetShaderIndex() { return materialDesc.shaderIndex; }
    
    int& GetShaderIDRef() { return materialDesc.shaderIndex; }
    int GetShaderID() { return materialDesc.shaderIndex; }

    std::string GetShaderName() { return materialDesc.shaderChosen; }
    std::string GetShaderString() { return materialDesc.shaderChosen; }


    std::string& GetShaderNameRef() { return materialDesc.shaderChosen; }
    std::string& GetShaderStringRef() { return materialDesc.shaderChosen; }

    bool LoadMaterialDescriptor(std::string filepath) { return materialDesc.LoadMaterialDescriptor(filepath); }
    bool SaveMaterialDescriptor(std::string filepath) { return materialDesc.SaveMaterialDescriptor(filepath); }

    int GetDiffuseID() { return materialDesc.DiffuseID; }
    int GetHeightID() { return materialDesc.HeightID; }
    int GetNormalID() { return materialDesc.NormalID; }



    MaterialDescriptor GetMaterial() { return materialDesc; }





    bool loadedMaterial = false;


   // const std::string& GetShaderName() const { return shaderName; }
    const glm::vec4& GetColor() const { return glm::vec4(materialDesc.albedoColorRed, materialDesc.albedoColorGreen, materialDesc.albedoColorBlue, materialDesc.albedoColorAlpha); }
   // int GetTextureID() const { return textureID; }
   // float GetShininess() const { return shininess; }

private:
    MaterialDescriptor materialDesc;
};
#endif;