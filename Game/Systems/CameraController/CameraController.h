#pragma once

#include <EngineCore.h>
#include "ECS/Coordinator.hpp"

enum class CameraMode
{
    FIRST_PERSON,
    THIRD_PERSON,
    SHIFTING
};

struct CameraMove {
    glm::vec3 Move_direct = glm::vec3(0.0f);
	float distance = 0.0f;
	float Yaw_rate = 0.0f;
	float Pitch_rate = 0.0f;
};

class CameraController {
public:
    
    CameraController(Entity playerEntity) : playerEntity(playerEntity), cameraMove{} { }

    void Update(float deltaTime);

    void ToggleCameraMode();

private:
    void UpdateFirstPersonView(CameraComponent& camera);
    void UpdateThirdPersonView(CameraComponent& camera);
	void UpdateShiftingView(CameraComponent& camera);

    CameraMove getfirstPersonCameraMove(CameraComponent& camera);
    CameraMove getThirdPersonCameraMove(CameraComponent& camera);


private:
    Entity playerEntity{};
    CameraMode currentMode = CameraMode::FIRST_PERSON;
    glm::vec3 thirdPersonOffset = glm::vec3(0.0f, 2.0f, -5.0f); // Example offset for third-person view
	CameraMode lastMode = CameraMode::FIRST_PERSON;
    CameraMove cameraMove;
	float cameraSpeed = 0.05f;
    float moved_dis = 0.0f;
};
