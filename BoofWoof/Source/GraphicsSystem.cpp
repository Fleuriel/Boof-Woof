#include "GraphicsSystem.h"
#include "WindowComponent.h"


#include "Shader.h"
#include <utility>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <unordered_map>


// Static members
GLFWwindow* GraphicsSystem::newWindow = nullptr;
GLuint GraphicsSystem::mdl_ref = 0;
GLuint GraphicsSystem::shd_ref = 0;
unsigned int GraphicsSystem::VBO = 0, GraphicsSystem::VAO = 0, GraphicsSystem::EBO = 0;

void GraphicsSystem::initWindow() {
    // Implement window creation
}

void GraphicsSystem::initGraphicsPipeline() {
    // Implement graphics pipeline initialization
}

void GraphicsSystem::OpenGLWindowResizeCallback(GLFWwindow* window, int width, int height) {
    // Implement window resize handling
}

void GraphicsSystem::UpdateLoop(std::vector<GraphicsComponent>& components) {
    // Loop through components and update them
}

void GraphicsSystem::Update(glm::vec3 newPosition, glm::vec3 Scale, glm::vec3 angle)
{

	for (auto& entity : mEntities) {
	
		GraphicsComponent& graphicObject = gCoordinator.GetComponent<GraphicsComponent>(entity);
		WindowComponent& winComp = gCoordinator.GetComponent<WindowComponent>(entity);

		using glm::radians;

		//Scale the model based on float variable.
		graphicObject.ScaleModel = glm::vec3(Scale.x, Scale.y, Scale.z);
		//scaleModel = glm::vec2(100, 100);

		// Increase the position based on the xSpeed of the user.
		// i.e if the user acceleration is 0, then speed increase
		// of xAccel is 0, and position would not change.
		graphicObject.Position = glm::vec3(newPosition.x, newPosition.y, newPosition.z);
		//position = glm::vec2(0, 0);

		// Boolean from the user to set if rotation is yes or no.


		// Compute the scale matrix
		glm::mat4 ScaledVector = glm::mat4(
			graphicObject.ScaleModel.x, 0.0f, 0.0f, 0.0f,
			0.0f, graphicObject.ScaleModel.y, 0.0f, 0.0f,
			0.0f, 0.0f, graphicObject.ScaleModel.z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);



		// Matrix for translation
		glm::mat4 Translate = glm::mat4{
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			graphicObject.Position.x, graphicObject.Position.y, graphicObject.Position.z, 1
		};


		glm::mat4 Rotation_x_Axis = glm::mat4{
			1, 0, 0, 0,
			0, cosf(radians(angle.x)), sinf(radians(angle.x)), 0,
			0, -sinf(radians(angle.x)), cosf(radians(angle.x)), 0,
			0, 0, 0, 1
		};

		glm::mat4 Rotation_z_Axis = glm::mat4{
			cosf(radians(angle.z)), sinf(radians(angle.z)), 0, 0,
			-sinf(radians(angle.z)), cosf(radians(angle.z)), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};

		glm::mat4 Rotation_y_Axis = glm::mat4{
		   cosf(radians(angle.y)), 0, -sinf(radians(angle.y)), 0,
		   0, 1, 0, 0,
		   sinf(radians(angle.y)), 0, cosf(radians(angle.y)), 0,
		   0, 0, 0, 1
		};

		// Matrix for rotation
		glm::mat4 Rotation = Rotation_z_Axis * Rotation_y_Axis * Rotation_x_Axis;



		// in the case of orthographic perspective
		// Not in use but put here to be able to recreate next time.
		float scaleX = 2.0f / winComp.windowSize.first;
		float scaleY = 2.0f / winComp.windowSize.second;
		float scaleZ = 2.0f / 1000000000;


		//Orthographic Projection
		// Scale to world NDC-coordinates matrix
		glm::mat4 ScaleToWorldToNDC = glm::mat4{
			scaleX, 0, 0, 0,
			0, scaleY, 0, 0,
			0, 0, scaleZ, 0,
			0, 0, 0, 1
		};

		// Define camera parameters
		glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 5.0f);
		glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

		// Compute the view matrix
		glm::mat4 viewMatrix = glm::lookAt(cameraPosition, cameraTarget, cameraUp);


		float fov = 45.0f; // Field of view in degrees
		float aspectRatio = static_cast<float>(winComp.windowSize.first) / static_cast<float>(winComp.windowSize.second);
		float nearPlane = 1.0f;
		float farPlane = 100.0f;

		// Gives the zoom in/out feel
		glm::mat4 PerspectiveProjection = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);


		// SRT
		graphicObject.model_To_NDC_xform = viewMatrix * Translate * Rotation * ScaledVector;
		// Compute the model-to-world-to-NDC transformation matrix
		graphicObject.model_To_NDC_xform = PerspectiveProjection * graphicObject.model_To_NDC_xform;
	}
	//std::cout << "Object Update\n";
// Compute the angular displacement in radians

	

}

void GraphicsSystem::Draw(std::vector<GraphicsComponent>& components) {
    // Loop through components and draw them
    for (auto& component : components) {
        DrawObject(component);
    }
}

void GraphicsSystem::DrawObject(GraphicsComponent& component) {
    // Draw logic using component data
}

void GraphicsSystem::CreateObject(OpenGLModel model, int Tag) {
    // Implement object creation
}