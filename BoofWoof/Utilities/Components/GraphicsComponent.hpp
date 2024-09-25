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
	//void SetModel2D(Model2D model) { m_Model2D = model; }

	//getter
	Model* getModel() const { return m_Model; }
	//Model2D getModel2D() const { return m_Model2D; }

    // Constructor and destructor
	GraphicsComponent() {};

	GraphicsComponent(Model* model, Entity entity)
		:m_Model(model), m_EntityID(entity)  {/*Empty by design*/
	}

	//GraphicsComponent(Model2D model2D, Entity entity)
	//	:m_Model2D(model2D), m_EntityID(entity) {
	//}

    ~GraphicsComponent() = default;

private:
	Entity m_EntityID{};
	Model* m_Model{};
	//Model2D m_Model2D{};
};

#endif  // GRAPHICS_COMPONENT_H
