/**************************************************************************
 * @file GraphicsComponent.hpp
 * @author 	Guo Chen
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

#include "ECS/Coordinator.hpp"
#include "../Core/Graphics/Model.h"   // Make sure Model is included
#include "../Core/Graphics/Object.h"  // Full definition of Object is needed here


class GraphicsComponent
{
public:
	void SetComponentEntityID(Entity& entity) { m_EntityID = entity; }

	//setter
	
	void setModelName(std::string modelName) { 
		m_ModelName = modelName; }
	void SetModelID(int modelID) { m_ModelID = modelID; }
	void AddTexture(int textureid) { 
		Texture texture;
		texture.id = textureid;
		textures.push_back(texture);
	}


	//getter
	std::string getModelName() { return m_ModelName; }
	int getModelID() { return m_ModelID; }


	void setTexture(std::string texture) { textureName = texture; }
	std::string getTexture() { return textureName; }
	int getTextureNumber() { return textures.size(); }
	Texture getTexture(int i) { return textures[i]; }


    // Constructor and destructor
	GraphicsComponent() {};
	
	GraphicsComponent(std::string modelName, Entity& entity) : m_ModelName(modelName), m_EntityID(g_Coordinator.GetEntityId(entity))
	{/*Empty by design*/
	}

    ~GraphicsComponent() = default;

private:
	Entity m_EntityID{};
	std::string m_ModelName{};
	int m_ModelID{};
	std::string textureName;
	std::vector<Texture> textures{};
};

#endif  // GRAPHICS_COMPONENT_H
