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
	void SetModel(Model* model) { m_Model = model; }

	//getter
	Model* getModel() const { return m_Model; }

    // Constructor and destructor
	GraphicsComponent() {};
	GraphicsComponent(Model* model, Entity entity)
		:m_Model(model), m_EntityID(entity)  {/*Empty by design*/
	}
    ~GraphicsComponent() = default;

private:
	Entity m_EntityID{};
	Model* m_Model{};
};

#endif  // GRAPHICS_COMPONENT_H