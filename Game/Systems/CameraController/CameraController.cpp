#include "CameraController.h"
#include "../../../BoofWoof/Utilities/Components/MetaData.hpp"


void CameraController::Update(float deltaTime)
{
    // Get active camera component
    auto& camera = g_Coordinator.GetComponent<CameraComponent>(playerEntity);

    // Update based on mode
    if (currentMode == CameraMode::FIRST_PERSON)
    {
        UpdateFirstPersonView(camera);
    }

    if (currentMode == CameraMode::THIRD_PERSON)
    {
        UpdateThirdPersonView(camera);
    }
}

void CameraController::ToggleCameraMode()
{
    if (currentMode == CameraMode::FIRST_PERSON)
    {
        currentMode = CameraMode::THIRD_PERSON;
    }
    else if (currentMode == CameraMode::THIRD_PERSON)
    {
        currentMode = CameraMode::FIRST_PERSON;
    }
}

void CameraController::UpdateFirstPersonView(CameraComponent& camera)
{
    // Match the camera's position to the player's position
    if (g_Coordinator.HaveComponent<TransformComponent>(playerEntity))
    {
        glm::vec3 playerPos = g_Coordinator.GetComponent<TransformComponent>(playerEntity).GetPosition();

        // eye offset 
        glm::vec3 eyeOffset = glm::vec3(0.0f, 0.193f, -1.189f);

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

    ////calculate the translation rotation x, y and z
    //float x = g_Coordinator.GetComponent<TransformComponent>(playerEntity).GetRotation().x;
    //float y = glm::radians(-camera.Yaw);
    //float z = g_Coordinator.GetComponent<TransformComponent>(playerEntity).GetRotation().z;


    //glm::vec3 newrotation = glm::vec3(x, y, z);
    //g_Coordinator.GetComponent<TransformComponent>(playerEntity).SetRotation(newrotation);
}

void CameraController::UpdateThirdPersonView(CameraComponent& camera)
{
    if (!g_Coordinator.HaveComponent<TransformComponent>(playerEntity))
    {
        return;
    }

    // Handle mouse input for rotating the camera
    static glm::vec2 lastMousePos = glm::vec2(g_Input.GetMousePosition().x, g_Input.GetMousePosition().y);
    static bool firstMouse = true;

    glm::vec2 currMousePos = glm::vec2(g_Input.GetMousePosition().x, g_Input.GetMousePosition().y);

    if (firstMouse) {
        lastMousePos = currMousePos;
        firstMouse = false;
    }

    glm::vec2 mouseOffset = currMousePos - lastMousePos;
    lastMousePos = currMousePos;

    // Apply mouse sensitivity
    mouseOffset *= camera.MouseSensitivity;

    // Update the camera's yaw and pitch based on mouse input
    camera.Yaw += mouseOffset.x;       // Horizontal rotation
    camera.Pitch -= mouseOffset.y;     // Vertical rotation (Pitch is inverted to match intuitive controls)

    // Clamp the pitch to avoid flipping the camera
    camera.Pitch = glm::clamp(camera.Pitch, -90.0f, 90.0f);
    glm::vec3 playerPos = g_Coordinator.GetComponent<TransformComponent>(playerEntity).GetPosition();

    // Set fixed offset values for the third-person camera
    glm::vec3 offset = glm::vec3(0.0f, 3.7f, 11.718f); // Desired position relative to the player

    // calculate the rotation matrix
    glm::mat4 rotationMatrix = glm::mat4(1.0f);
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(camera.Yaw + 90), glm::vec3(0.0f, -1.0f, 0.0f));
    //rotationMatrix = glm::rotate(rotationMatrix, glm::radians(camera.Pitch), glm::vec3(-1.0f, 0.0f, 0.0f));

    // calculate the new offset
    glm::vec3 newOffset = glm::vec3(rotationMatrix * glm::vec4(offset, 1.0f));

    // Set the camera's position to the player's position plus the offset
    camera.Position = playerPos + newOffset;
    // Make the camera look at the player - if i set this, it jitters.
    // camera.SetCameraDirection(glm::normalize(playerPos - camera.Position));

    // Update the camera's direction vectors
    camera.updateCameraVectors();

    //calculate the translation rotation x, y and z
    float x = g_Coordinator.GetComponent<TransformComponent>(playerEntity).GetRotation().x;
    float y = glm::radians(-camera.Yaw);
    float z = g_Coordinator.GetComponent<TransformComponent>(playerEntity).GetRotation().z;


    glm::vec3 newrotation = glm::vec3(x, y, z);
    g_Coordinator.GetComponent<TransformComponent>(playerEntity).SetRotation(newrotation);
}