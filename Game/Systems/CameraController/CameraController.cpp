#include "CameraController.h"
#include "../../../BoofWoof/Utilities/Components/MetaData.hpp"


void CameraController::Update(float deltaTime)
{
	// Get active camera component
	auto& camera = g_Coordinator.GetComponent<CameraComponent>(playerEntity);

	// Update to 1st POV
	UpdateFirstPersonView(camera, deltaTime);
}

void CameraController::UpdateFirstPersonView(CameraComponent& camera, float deltaTime)
{
    // Match the camera's position to the player's position
    if (g_Coordinator.HaveComponent<TransformComponent>(playerEntity)) 
    {
       glm::vec3 playerPos = g_Coordinator.GetComponent<TransformComponent>(playerEntity).GetPosition();

       // eye offset 
       glm::vec3 eyeOffset = glm::vec3(1.088f, 1.1f, 0.0f); 

       // Set the camera's position to the player's eye position
       camera.Position = playerPos + eyeOffset;
    }

    // Handle mouse input for camera rotation
    static glm::vec2 lastMousePos = glm::vec2(g_Input.GetMousePosition().x, g_Input.GetMousePosition().y);
    static bool firstMouse = true;

    // Get current mouse position
    glm::vec2 currMousePos = glm::vec2(g_Input.GetMousePosition().x, g_Input.GetMousePosition().y);

    if (firstMouse) {
        lastMousePos = currMousePos;
        firstMouse = false;
    }

    // Calculate mouse offset
    glm::vec2 mouseOffset = currMousePos - lastMousePos;

    // Update last mouse position for the next frame
    lastMousePos = currMousePos;

    // Apply mouse sensitivity
    mouseOffset *= camera.MouseSensitivity;

    // Apply mouse movement to the camera's yaw and pitch
    camera.ProcessMouseMovement(mouseOffset.x, mouseOffset.y);

    // Ensure the camera's direction is updated based on its yaw and pitch
    camera.updateCameraVectors();
}
