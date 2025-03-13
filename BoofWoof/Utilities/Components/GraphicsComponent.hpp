/**************************************************************************
 * @file GraphicsComponent.hpp
 * @author  Guo Chen
 * @param DP email: g.chen@digipen.edu [2200518]
 * @param Course: CS 3401
 * @param Course: Game Project 3
 * @date  10/06/2024 (06 OCTOBER 2024)
 * @brief
 *
 * This file contains the definition of the GraphicsComponent class, which is used to
 * store the model name and model ID of an entity.
 *
 *************************************************************************/

#pragma once
#ifndef GRAPHICS_COMPONENT_H
#define GRAPHICS_COMPONENT_H
#ifdef APIENTRY
#undef APIENTRY
#endif



#include "ECS/Coordinator.hpp"
#include "../Core/Graphics/Model.h"   // Make sure Model is included
#include "../Core/Graphics/Object.h"  // Full definition of Object is needed here
#include "../Core/Reflection/ReflectionManager.hpp" // Include the reflection manager
#include "../Utilities/Components/MaterialComponent.hpp"

class MaterialComponent;


class GraphicsComponent
{
public:
    void SetComponentEntityID(Entity& entity) { m_EntityID = entity; }

    // Setters
    void setModelName(std::string modelName) { m_ModelName = modelName; }
    void SetModelID(int modelID) { m_ModelID = modelID; }



    void AddTexture(int textureid) { textures.push_back(textureid); }

    void SetDiffuse(int textureID) 
    {
        if (textures.size() < 1) textures.resize(1); // Ensure at least one element
        textures[0] = textureID;
    }
    void SetNormal(int textureID) 
    {
        if (textures.size() < 2) textures.resize(2); // Ensure at least two elements
        textures[1] = textureID;
    }
    void SetHeight(int textureID) 
    {
        if (textures.size() < 3) textures.resize(3); // Ensure at least three elements
        textures[2] = textureID;
    }

	void SetTextures(std::vector<int> textureids) { textures = textureids; }
	void clearTextures() { textures.clear(); }
	bool RemoveTexture(int textureid) {
		for (int i = 0; i < textures.size(); i++) {
			if (textures[i] == textureid) {
				textures.erase(textures.begin() + i);
				return true;
			}
		}
		return false;
	}


    bool RemoveDiffuse() {
        if (textures.size() > 0 && textures[0] != -1) { // Check if textures[0] exists and is not already removed
            textures[0] = -1; // Mark as removed
            return true;
        }
        return false; // textures[0] doesn't exist or is already removed
    }

    bool RemoveNormal() {
        if (textures.size() > 1 && textures[0] != -1 && textures[1] != -1) { // Check both conditions
            textures[1] = -1; // Mark as removed
            return true;
        }
        return false; // Cannot remove if conditions aren't met
    }

    bool RemoveHeight() {
        if (textures.size() > 2 && textures[0] != -1 && textures[1] != -1 && textures[2] != -1) { // Check all conditions
            textures[2] = -1; // Mark as removed
            return true;
        }
        return false; // Cannot remove if conditions aren't met
    }


    void SetModel(Model* model) { m_Model = model; }

    //getter
    Model* getModel() const { return m_Model; }
    inline int getModelID() const { return m_ModelID; };



	void SetFollowCamera(bool follow) { followCamera = follow; }

    // Getters
    std::string getModelName() const { return m_ModelName; }\
//	std::vector<int> getTextures() const { return textures; }
    
    std::string getTextureName() const { return textureName; }

    //bool incrementTextureNumber() { Texture texture; textures.push_back(texture.id); return true; }
    int getTextureNumber() const { return static_cast<int>(textures.size()); }
    int getTexture(int i) const { return textures[i]; }
	bool getFollowCamera() const { return followCamera; }

    // Set texture name
    void setTexture(std::string texture) { textureName = texture; }

    glm::vec3 boundingBox;

   std::string GetShaderName() { return material.GetShaderName(); }
   std::string GetMaterialName() { return material.GetMaterialName(); }
   int GetShaderIdx() { return material.GetShaderIndex(); }
   std::string GetDiffuseName() { return material.GetDiffuseName(); }
   std::string GetNormalName() { return  material.GetNormalName(); }
   std::string GetHeightName() { return  material.GetHeightName(); }

    //GraphicsComponent() { std::cout << "eUHEEE\n";}



    GraphicsComponent() : hasMaterial(false), boundingBox(glm::vec3(0.0f)) {}

    GraphicsComponent(std::string modelName, Entity& entity, bool followCam = true)
        : m_ModelName(modelName),
        m_EntityID(g_Coordinator.GetEntityId(entity)),
 //       textureName(texName),
        followCamera(followCam),
        hasMaterial(false),
        boundingBox(glm::vec3(0.0f))
    {
        std::cout << "Model Name of Graphics Component: " << m_ModelName << '\n';
    }



    GraphicsComponent(Model* model, Entity entity)
    :   m_Model(model), m_EntityID(entity){}

    ~GraphicsComponent() = default;

    // Reflection integration
    REFLECT_COMPONENT(GraphicsComponent)
    {
        REGISTER_PROPERTY(GraphicsComponent, ModelName, std::string, setModelName, getModelName);
        REGISTER_PROPERTY(GraphicsComponent, ModelID, int, SetModelID, getModelID);
	//	REGISTER_PROPERTY(GraphicsComponent, Textures, std::vector<int>, SetTextures, getTextures);
        REGISTER_PROPERTY(GraphicsComponent, FollowCamera, bool, SetFollowCamera, getFollowCamera);
    }


    MaterialComponent material;
    bool hasMaterial;       // Indicates if a material is assigned

//    Animation animation;


    void AddMaterial(const MaterialComponent& newMaterial) {
        material = newMaterial;
        hasMaterial = true;
    }

    void RemoveMaterial() {
        hasMaterial = false;
        // Reset material to a default state, if applicable
    }
   
   
    bool LoadMaterialDesc(std::string filepath)
    {
        return material.LoadMaterialDescriptor(filepath);
   
    }


    bool pauseAnimation = false;
    
    void SetAnimationTime(float setAnimation)
    {
        AnimTime = setAnimation;
    }


    float GetAnimationTime()
    {
        return AnimTime;
    }

  //  void AddAnimation(GraphicsComponent& graphics, const Animation& newAnimation) {
  //      graphics.animation = newAnimation;
  //      graphics.hasAnimation = true;
  //  }
  //
  //  void RemoveAnimation(GraphicsComponent& graphics) {
  //      graphics.hasAnimation = false;
  //  }

private:

    float AnimTime;
    Entity m_EntityID{};
    Model* m_Model{};
    std::string m_ModelName{};
    int m_ModelID{};
    std::string textureName;
    std::vector<int> textures{};
    bool followCamera = true;

};

#endif  // GRAPHICS_COMPONENT_H
