#pragma once

#include <EngineCore.h>
#include "ECS/Coordinator.hpp"

enum class CameraMode
{
    FIRST_PERSON,
    THIRD_PERSON,
    SHIFTING,
	SHAKE
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
	void ShakeCamera(float time, glm::vec3 range);
	void ShakePlayer(float time, glm::vec3 range);
	void ChangeToFirstPerson(CameraComponent& camera);

private:
    void UpdateFirstPersonView(CameraComponent& camera);
    void UpdateThirdPersonView(CameraComponent& camera);
	void UpdateShiftingView(CameraComponent& camera);
	void UpdateShakeView(CameraComponent& camera);
	void UpdateShakePlayer(CameraComponent& camera);

    CameraMove getfirstPersonCameraMove(CameraComponent& camera);
    CameraMove getThirdPersonCameraMove(CameraComponent& camera);


private:
    Entity playerEntity{};
	CameraMode currentMode = CameraMode::FIRST_PERSON;
    glm::vec3 thirdPersonOffset = glm::vec3(0.0f, 1.7f, 3.718f); // Example offset for third-person view
	glm::vec3 firstPersonOffset = glm::vec3(0.0f, 0.21f, -1.189f); // Example offset for first-person view
	CameraMode lastMode = CameraMode::FIRST_PERSON;
    CameraMove cameraMove;
	float cameraSpeed = 0.05f;
    float moved_dis = 0.0f;


	float shakeTime = 0.0f;
	float shakeDuration = 0.0f;
	glm::vec3 shakeRange = glm::vec3(0.0f);
	glm::vec3 player_old_pos = glm::vec3(0.0f);
	glm::vec3 camera_old_pos = glm::vec3(0.0f);

	float low_limit_pitch_first = -10.0f;
	float high_limit_pitch_first = 45.0f;
};
