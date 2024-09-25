#pragma once
#ifndef TRANSFORM_COMPONENT_HPP
#define TRANSFORM_COMPONENT_HPP

#include "ECS/Coordinator.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../Core/Graphics/Model.h"   // Make sure Model is included
#include "../Core/Graphics/Object.h"  // Full definition of Object is needed here

class TransformComponent 

{
public:
    void SetComponentEntityID(Entity& entity) { m_EntityID = entity; }
    
	// setter
	void SetPosition(glm::vec3 position) { this->position = position; }
	void SetScale(glm::vec3 scale) { this->scale = scale; }
	void SetRotation(glm::vec3 rotation) { this->rotation = rotation; }

	// getter
	glm::vec3& GetPosition() { return position; }
	glm::vec3& GetScale() { return scale; }
	glm::vec3& GetRotation() { return rotation; }
	glm::mat4 GetWorldMatrix(){
		glm::mat4 worldMatrix = glm::mat4(1.0f);


		worldMatrix = glm::rotate(worldMatrix, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		worldMatrix = glm::rotate(worldMatrix, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		worldMatrix = glm::rotate(worldMatrix, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

		glm::mat4 scaleMat = glm::mat4(1.0f);
		scaleMat = glm::scale(scaleMat, scale);

		glm::mat4 translateMat = glm::mat4(1.0f);

		translateMat = glm::translate(translateMat, position);

		worldMatrix = translateMat * worldMatrix * scaleMat;

		/*worldMatrix = glm::mat4{
			worldMatrix[0][0], worldMatrix[1][0], worldMatrix[2][0], 0.0f,
			worldMatrix[0][1], worldMatrix[1][1], worldMatrix[2][1], 0.0f,
			worldMatrix[0][2], worldMatrix[1][2], worldMatrix[2][2], 0.0f,
			position[0], position[1], position[2], 1.0f

	};*/

	return worldMatrix;
	}


    // Constructor and destructor
	TransformComponent() {};
	TransformComponent(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, Entity& entity)
		: position(position), scale(scale), rotation(rotation), m_EntityID(g_Coordinator.GetEntityId(entity)) {/*Empty by design*/
	}

	~TransformComponent() = default;
private:
	Entity m_EntityID{};
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotation;

};

#endif  // GRAPHICS_COMPONENT_H
