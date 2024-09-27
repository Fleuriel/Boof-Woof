#pragma once
#ifndef TRANSFORM_COMPONENT_HPP
#define TRANSFORM_COMPONENT_HPP

#include "ECS/Coordinator.hpp"
#include "../Core/Graphics/Model.h"   // Make sure Model is included
#include "../Core/Graphics/Object.h"  // Full definition of Object is needed here

class TransformComponent 
{
public:
    
	// Constructor and destructor
	TransformComponent() {};
	TransformComponent(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, Entity& entity)
		: m_Position(position), m_Scale(scale), m_Rotation(rotation), m_EntityID(g_Coordinator.GetEntityId(entity)) {/*Empty by design*/
	}

	~TransformComponent() = default;

	void SetComponentEntityID(Entity& entity) { m_EntityID = entity; }
    
	// setter
	void SetPosition(glm::vec3 position) { this->m_Position = position; }
	void SetScale(glm::vec3 scale) { this->m_Scale = scale; }
	void SetRotation(glm::vec3 rotation) { this->m_Rotation = rotation; }

	// getter
	glm::vec3& GetPosition() { return m_Position; }
	glm::vec3& GetScale() { return m_Scale; }
	glm::vec3& GetRotation() { return m_Rotation; }
	glm::mat4 GetWorldMatrix(){
		glm::mat4 worldMatrix = glm::mat4(1.0f);


		worldMatrix = glm::rotate(worldMatrix, m_Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		worldMatrix = glm::rotate(worldMatrix, m_Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		worldMatrix = glm::rotate(worldMatrix, m_Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

		glm::mat4 scaleMat = glm::mat4(1.0f);
		scaleMat = glm::scale(scaleMat, m_Scale);

		glm::mat4 translateMat = glm::mat4(1.0f);

		translateMat = glm::translate(translateMat, m_Position);

		worldMatrix = translateMat * worldMatrix * scaleMat;

		/*worldMatrix = glm::mat4{
			worldMatrix[0][0], worldMatrix[1][0], worldMatrix[2][0], 0.0f,
			worldMatrix[0][1], worldMatrix[1][1], worldMatrix[2][1], 0.0f,
			worldMatrix[0][2], worldMatrix[1][2], worldMatrix[2][2], 0.0f,
			position[0], position[1], position[2], 1.0f

	};*/

	return worldMatrix;
	}

private:
	Entity m_EntityID{};
	glm::vec3 m_Position{};
	glm::vec3 m_Scale{};
	glm::vec3 m_Rotation{};

};

#endif  // GRAPHICS_COMPONENT_H
