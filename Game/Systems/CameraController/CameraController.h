#pragma once

#include <EngineCore.h>
#include "ECS/Coordinator.hpp"

enum class CameraMode 
{
    FIRST_PERSON,
    THIRD_PERSON,
    SHIFTING
};

struct cameraInfo {
	glm::vec3 Position;
	float Yaw;
	float Pitch;
};

class CameraController {
public:
    CameraController(Entity playerEntity) : playerEntity(playerEntity) { }

    void Update(float deltaTime);

    void ToggleCameraMode();

private:
    void UpdateFirstPersonView(CameraComponent& camera);
    void UpdateThirdPersonView(CameraComponent& camera);
	void UpdateShiftingView(CameraComponent& camera, glm::vec3 targetPos, float targetYaw, float targetPitch, float deltaTime);
	cameraInfo GetFirstPersonView(CameraComponent& camera);
	cameraInfo GetThirdPersonView(CameraComponent& camera);

private:
    Entity playerEntity{};
    CameraMode currentMode = CameraMode::FIRST_PERSON;
	CameraMode previousMode = CameraMode::FIRST_PERSON;
    glm::vec3 thirdPersonOffset = glm::vec3(0.0f, 2.0f, -5.0f); // Example offset for third-person view
};
