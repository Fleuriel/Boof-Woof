#pragma once

#include <EngineCore.h>
#include "ECS/Coordinator.hpp"

enum class CameraMode 
{
    FIRST_PERSON,
    THIRD_PERSON
};

class CameraController {
public:
    CameraController(Entity playerEntity) : playerEntity(playerEntity) { }

    void Update(float deltaTime);

    void ToggleCameraMode();

private:
    void UpdateFirstPersonView(CameraComponent& camera);
    void UpdateThirdPersonView(CameraComponent& camera);

private:
    Entity playerEntity{};
    CameraMode currentMode = CameraMode::FIRST_PERSON;
    glm::vec3 thirdPersonOffset = glm::vec3(0.0f, 2.0f, -5.0f); // Example offset for third-person view
};
