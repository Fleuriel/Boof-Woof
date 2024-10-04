#pragma once
#ifndef OBJECT_H
#define OBJECT_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "GraphicsSystem.h"
//#include "../Utilities/Components/GraphicsComponent.h"
//#include "Windows/WindowSystem.h"

class Model;
class Model2D;

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
	
	glm::vec3 position{};
	glm::vec3 scale{};
	glm::vec3 rotation{};
	glm::vec3 color{};

	Model* model{};
	

	glm::mat4 worldMatrix{};
	glm::mat4 getWorldMatrix();


	//void UpdateObject(Entity entity, GraphicsComponent& graphicsComp, float deltaTime);

//	void UpdateObject(Entity entity);

	// bounding volume


	Object() : position(glm::vec3(0.0f)), scale(glm::vec3(0.0f)), rotation(glm::vec3(0.0f)), color(glm::vec3(0.0f)), worldMatrix(glm::mat4(0.0f)), model(nullptr) {}
};


class Object2D
{
public:

	glm::vec3 position{};
	glm::vec3 scale{};
	glm::vec3 rotation{};
	glm::vec3 color{};

	Model2D* model{};

	glm::mat3 model_to_NDC_xform_2D{};


	void UpdateObject2D(float deltaTime);


	Object2D(): position(glm::vec3(0.0f)), scale(glm::vec3(0.0f)), rotation(glm::vec3(0.0f)), color(glm::vec3(0.0f)), model_to_NDC_xform_2D(glm::mat3(0.0f)), model(nullptr) {}

};

#endif
