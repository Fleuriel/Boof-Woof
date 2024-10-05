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

	//getter
	std::string getModelName() { return m_ModelName; }
	int getModelID() { return m_ModelID; }

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
};

#endif  // GRAPHICS_COMPONENT_H
