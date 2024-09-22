#pragma once
#include "../Core/ECS/pch.h"
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <string>
#include "../Core/Graphics/Model.h"
#include "../Core/Graphics/Shader.h"


class GraphicsComponent {
public:
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotation;
	glm::vec3 color;


	Model* model;
	OpenGLShader* shaderpgm;


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
    // Constructor and destructor
    GraphicsComponent();
//    GraphicsComponent(const OpenGLModel& model, int id = 0);
    ~GraphicsComponent() = default;
};

extern std::list<GraphicsComponent> objects;