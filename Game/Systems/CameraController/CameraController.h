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
    glm::vec3 positionMove = glm::vec3(0.0f);
    float pitchMove = 0.0f;
    float yawMove = 0.0f;
};

class CameraController {
public:
    
    CameraController(Entity playerEntity) : playerEntity(playerEntity), cameraMove{} { }

    void Update(float deltaTime);

    void ToggleCameraMode();

	//void update_cameramove(float time);

private:
    void UpdateFirstPersonView(CameraComponent& camera);
    void UpdateThirdPersonView(CameraComponent& camera);
	void UpdateShiftingView(CameraComponent& camera);

    CameraMove getfirstPersonCameraMove();
    CameraMove getThirdPersonCameraMove();


private:
    Entity playerEntity{};
    CameraMode currentMode = CameraMode::FIRST_PERSON;
    glm::vec3 thirdPersonOffset = glm::vec3(0.0f, 2.0f, -5.0f); // Example offset for third-person view
	CameraMode lastMode = CameraMode::FIRST_PERSON;
    CameraMove cameraMove;
};
