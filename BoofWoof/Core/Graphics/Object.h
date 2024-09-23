#pragma once
#ifndef OBJECT_H
#define OBJECT_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "GraphicsSystem.h"
#include "../Core/Graphics/Model.h"
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


class GraphicsComponent;
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
	glm::mat4 getWorldMatrix();


	void UpdateObject(Entity entity, GraphicsComponent& graphicsComp, float deltaTime);

//	void UpdateObject(Entity entity);

	// bounding volume
};
#endif
