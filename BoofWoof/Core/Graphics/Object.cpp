#include "pch.h"
#include "../Utilities/Components/GraphicsComponent.hpp"
#include "Windows/WindowManager.h"

glm::mat4 Object::getWorldMatrix() {


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


//void Object2D::UpdateObject(float deltaTime)
//{
//	(void)deltaTime;
//	
//	using glm::radians;
//
//	// Compute matrices
//	glm::mat4 ScaleToWorldToNDC = glm::mat4{
//		2.0f / g_WindowX, 0, 0, 0,
//		0, 2.0f / g_WindowY, 0, 0,
//		0, 0, 2.0f / 1000000000.0f, 0,
//		0, 0, 0, 1
//	};
//
//	// Compute the scale matrix
//	glm::mat4 ScaledVector = glm::mat4(
//		scale.x, 0.0f, 0.0f, 0.0f,
//		0.0f, scale.y, 0.0f, 0.0f,
//		0.0f, 0.0f, scale.z, 0.0f,
//		0.0f, 0.0f, 0.0f, 1.0f
//	);
//
//	glm::mat4 Translate = glm::mat4{
//		1, 0, 0, 0,
//		0, 1, 0, 0,
//		0, 0, 1, 0,
//		position.x,position.y,position.z, 1
//	};
//	glm::mat4 Rotation_x_Axis = glm::mat4{
//		1, 0, 0, 0,
//		0, cosf(radians(rotation.x)), sinf(radians(rotation.x)), 0,
//		0, -sinf(radians(rotation.x)), cosf(radians(rotation.x)), 0,
//		0, 0, 0, 1
//	};
//
//	glm::mat4 Rotation_z_Axis = glm::mat4{
//		cosf(radians(rotation.z)), sinf(radians(rotation.z)), 0, 0,
//		-sinf(radians(rotation.z)), cosf(radians(rotation.z)), 0, 0,
//		0, 0, 1, 0,
//		0, 0, 0, 1
//	};
//
//	glm::mat4 Rotation_y_Axis = glm::mat4{
//		cosf(radians(rotation.y)), 0, -sinf(radians(rotation.y)), 0,
//		0, 1, 0, 0,
//		sinf(radians(rotation.y)), 0, cosf(radians(rotation.y)), 0,
//		0, 0, 0, 1
//	};
//
//	glm::mat4 Rotation = Rotation_z_Axis * Rotation_y_Axis * Rotation_x_Axis;
//
//	// View and Projection matrices
//	glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 5.0f);
//	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
//	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
//	glm::mat4 viewMatrix = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
//
//	float fov = 45.0f; // Field of view in degrees
//	float aspectRatio = static_cast<float>(g_WindowX) / static_cast<float>(g_WindowY);
//	float nearPlane = 1.0f;
//	float farPlane = 100.0f;
//	glm::mat4 PerspectiveProjection = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
//
//
//	// Final transformation matrix
//	//graphicsComp.model_To_NDC_xform = viewMatrix * Translate * Rotation * ScaledVector;
//
//	//*ScaleToWorldToNDC;
//	//graphicsComp.model_To_NDC_xform = PerspectiveProjection * graphicsComp.model_To_NDC_xform;
//}



void Object2D::UpdateObject2D(float deltaTime)
{
	(void)deltaTime;

	using glm::radians;

	scale.x = 100.0f;
	scale.y = 100.0f;

	//uh idk why it slanted if i dont put this but sure i gues?
	rotation.x = 0.0f;

	//this allows movement.
	//position.x += 0.1f;

	// Compute matrices
	// Compute the scale matrix
	glm::mat3 Scale = glm::mat3(
		scale.x, 0.0f, 0.0f,
		0.0f, scale.y, 0.0f,
		0.0f, 0.0f, 1.0f
	);

	// Compute the rotation matrix
	glm::mat3 Rotation = glm::mat3(
		cosf(glm::radians(rotation.x)), sinf(glm::radians(rotation.x)), 0.0f,
		-sinf(glm::radians(rotation.x)), cosf(glm::radians(rotation.x)), 0.0f,
		0.0f, 0.0f, 1.0f
	);

	// Compute the translation matrix
	glm::mat3 Translation = glm::mat3(
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		position.x, position.y, 1.0f
	);


	// Compute the scaling matrix to map from world coordinates to NDC coordinates
	glm::mat3 ScaleToWorldToNDC = glm::mat3(
		2.0f / g_WindowX, 0.0f, 0.0f,
		0.0f, 2.0f / g_WindowY, 0.0f,
		0.0f, 0.0f, 1.0f
	);



	// Compute the model-to-world-to-NDC transformation matrix
	model_to_NDC_xform_2D = ScaleToWorldToNDC  * Translation * Rotation * Scale;


}