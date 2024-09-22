#pragma once
#ifndef OBJECT_H
#define OBJECT_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "Model.h"
#include <memory>

//enum class ObjectType
//{
//	SPHERE,
//	AABB_,
//	RAY,
//	PLANE,
//	POINT3D,
//	TRIANGLE,
//	OBJECT
//};


class Object
{
	public:
	//ObjectType type;
	
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotation;
	glm::vec3 color;
	

	Model* model;
	

	glm::mat4 worldMatrix;
	glm::mat4 getWorldMatrix() {

		
		worldMatrix = glm::mat4(1.0f);
		
		
		
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

	// bounding volume
};
#endif
