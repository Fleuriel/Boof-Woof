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

	if (currentMode == CameraMode::SHIFTING)
	{
		UpdateShiftingView(camera);
	}

	if (currentMode == CameraMode::SHAKE)
	{
		UpdateShakeView(camera);
	}
}

void CameraController::ToggleCameraMode()
{
    if (currentMode == CameraMode::FIRST_PERSON)
    {
        currentMode = CameraMode::SHIFTING;
		lastMode = CameraMode::FIRST_PERSON;
		cameraMove = getThirdPersonCameraMove(g_Coordinator.GetComponent<CameraComponent>(playerEntity));
    }
    else if (currentMode == CameraMode::THIRD_PERSON)
    {
        currentMode = CameraMode::SHIFTING;
		lastMode = CameraMode::THIRD_PERSON;
		cameraMove = getfirstPersonCameraMove(g_Coordinator.GetComponent<CameraComponent>(playerEntity));
	}
}

void CameraController::ShakeCamera(float time, glm::vec3 range)
{
	shakeTime = 0.0f;
	shakeDuration = time;
	shakeRange = range;
    lastMode = currentMode;
    camera_old_pos = g_Coordinator.GetComponent<CameraComponent>(playerEntity).GetCameraPosition();
    currentMode = CameraMode::SHAKE;
}

void CameraController::ShakePlayer(float time, glm::vec3 range)
{
	shakeTime = 0.0f;
	shakeDuration = time;
	shakeRange = range;
	player_old_pos = g_Coordinator.GetComponent<TransformComponent>(playerEntity).GetPosition();
	camera_old_pos = g_Coordinator.GetComponent<CameraComponent>(playerEntity).GetCameraPosition();
	lastMode = currentMode;
	currentMode = CameraMode::SHAKE;
}

void CameraController::ChangeToFirstPerson(CameraComponent& camera)
{
	
        glm::vec3 playerPos = g_Coordinator.GetComponent<TransformComponent>(playerEntity).GetPosition();

        // Set fixed offset values for the third-person camera
        glm::vec3 offset = thirdPersonOffset; // Desired position relative to the player

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
		
        currentMode = CameraMode::SHIFTING;
        lastMode = CameraMode::THIRD_PERSON;
        cameraMove = getfirstPersonCameraMove(g_Coordinator.GetComponent<CameraComponent>(playerEntity));
	
}


void CameraController::UpdateFirstPersonView(CameraComponent& camera)
{
    // Match the camera's position to the player's position
    if (!g_Coordinator.HaveComponent<TransformComponent>(playerEntity))
    {
		return;
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
    camera.ProcessMouseMovement(mouseOffset.x, -mouseOffset.y);

    glm::vec3 playerPos = g_Coordinator.GetComponent<TransformComponent>(playerEntity).GetPosition();

    // eye offset 
    glm::vec3 eyeOffset = firstPersonOffset;
    // calculate the rotation matrix
    glm::mat4 rotationMatrix = glm::mat4(1.0f);
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(camera.Yaw + 90), glm::vec3(0.0f, -1.0f, 0.0f));
    //rotationMatrix = glm::rotate(rotationMatrix, glm::radians(camera.Pitch), glm::vec3(-1.0f, 0.0f, 0.0f));

    // calculate the new offset
    glm::vec3 newOffset = glm::vec3(rotationMatrix * glm::vec4(eyeOffset, 1.0f));

    camera.Position = playerPos + newOffset;

	// make sure the pitch is within the limits
	if (camera.Pitch > high_limit_pitch_first) camera.Pitch = high_limit_pitch_first;
	if (camera.Pitch < low_limit_pitch_first) camera.Pitch = low_limit_pitch_first;

    // Ensure the camera's direction is updated based on its yaw and pitch
    camera.updateCameraVectors();

    //calculate the translation rotation x, y and z
    float x = g_Coordinator.GetComponent<TransformComponent>(playerEntity).GetRotation().x;
    float y = glm::radians(-camera.Yaw);
    float z = g_Coordinator.GetComponent<TransformComponent>(playerEntity).GetRotation().z;


    glm::vec3 newrotation = glm::vec3(x, y, z);
    g_Coordinator.GetComponent<TransformComponent>(playerEntity).SetRotation(newrotation);

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


    // Apply mouse movement to the camera's yaw and pitch
    camera.ProcessMouseMovement(mouseOffset.x, mouseOffset.y);


    glm::vec3 playerPos = g_Coordinator.GetComponent<TransformComponent>(playerEntity).GetPosition();

    // Set fixed offset values for the third-person camera
    glm::vec3 offset = thirdPersonOffset; // Desired position relative to the player

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

void CameraController::UpdateShiftingView(CameraComponent& camera)
{
	
    moved_dis += cameraSpeed ;
	camera.Position += cameraMove.Move_direct * cameraSpeed;
    if (moved_dis >= cameraMove.distance)
    {
		if (lastMode == CameraMode::FIRST_PERSON)
		{
			currentMode = CameraMode::THIRD_PERSON;
		}
		else if (lastMode == CameraMode::THIRD_PERSON)
		{
			currentMode = CameraMode::FIRST_PERSON;
		}
		moved_dis = 0.0f;
    }
}

void CameraController::UpdateShakeView(CameraComponent& camera)
{
    shakeTime += static_cast<float>(g_Core->m_FixedDT);
	if (shakeTime >= shakeDuration)
	{
		currentMode = lastMode;
		shakeTime = 0.0f;
		shakeDuration = 0.0f;
		camera.Position = camera_old_pos;

	}
	else
	{
		glm::vec3 offset = glm::vec3(
			((rand() % 100) / 100.0f) * shakeRange.x - shakeRange.x / 2.0f,
			((rand() % 100) / 100.0f) * shakeRange.y - shakeRange.y / 2.0f,
			((rand() % 100) / 100.0f) * shakeRange.z - shakeRange.z / 2.0f
		);

		//offset = g_Coordinator.GetComponent<TransformComponent>(playerEntity).GetRotation() * offset;
		camera.Position += offset;
	}
}

void CameraController::UpdateShakePlayer(CameraComponent& camera)
{
	
	shakeTime += static_cast<float>(g_Core->m_FixedDT);
	if (shakeTime >= shakeDuration)
	{
		
		currentMode = lastMode;
		shakeTime = 0.0f;
		shakeDuration = 0.0f;
		g_Coordinator.GetComponent<TransformComponent>(playerEntity).SetPosition(player_old_pos);
		camera.SetCameraPosition(camera_old_pos);
	}
	else
	{
		glm::vec3 offset = glm::vec3(
			((rand() % 100) / 100.0f) * shakeRange.x - shakeRange.x / 2.0f,
			((rand() % 100) / 100.0f) * shakeRange.y - shakeRange.y / 2.0f,
			((rand() % 100) / 100.0f) * shakeRange.z - shakeRange.z / 2.0f
		);

		g_Coordinator.GetComponent<TransformComponent>(playerEntity).SetPosition(player_old_pos + offset);
		glm::vec3 camerapos = camera_old_pos + offset;
		camera.SetCameraPosition(camerapos);
        
	}
}

CameraMove CameraController::getfirstPersonCameraMove(CameraComponent& camera)
{
	CameraMove cm;
    // Match the camera's position to the player's position
    if (!g_Coordinator.HaveComponent<TransformComponent>(playerEntity))
    {
        return cm;
    }

    glm::vec3 playerPos = g_Coordinator.GetComponent<TransformComponent>(playerEntity).GetPosition();

    // calculate the rotation matrix
    glm::mat4 rotationMatrix = glm::mat4(1.0f);
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(camera.Yaw + 90), glm::vec3(0.0f, -1.0f, 0.0f));

	// eye offset
    glm::vec3 eyeOffset = firstPersonOffset;
    eyeOffset = glm::vec3(rotationMatrix * glm::vec4(eyeOffset, 1.0f));
   
	// 3rd person offset
	glm::vec3 cmoffset = thirdPersonOffset; // Desired position relative to the player
	cmoffset = glm::vec3(rotationMatrix * glm::vec4(cmoffset, 1.0f));

	cm.Move_direct = eyeOffset - cmoffset;

	cm.distance = glm::length(cm.Move_direct);
	cm.Move_direct = glm::normalize(cm.Move_direct);

	return cm;

}

CameraMove CameraController::getThirdPersonCameraMove(CameraComponent& camera)
{
    CameraMove cm;
    // Match the camera's position to the player's position
    if (!g_Coordinator.HaveComponent<TransformComponent>(playerEntity))
    {
        return cm;
    }

    glm::vec3 playerPos = g_Coordinator.GetComponent<TransformComponent>(playerEntity).GetPosition();

    // calculate the rotation matrix
    glm::mat4 rotationMatrix = glm::mat4(1.0f);
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(camera.Yaw + 90), glm::vec3(0.0f, -1.0f, 0.0f));

    // eye offset
    glm::vec3 eyeOffset = firstPersonOffset;
    eyeOffset = glm::vec3(rotationMatrix * glm::vec4(eyeOffset, 1.0f));

    // 3rd person offset
    glm::vec3 cmoffset = thirdPersonOffset; // Desired position relative to the player
    cmoffset = glm::vec3(rotationMatrix * glm::vec4(cmoffset, 1.0f));

    cm.Move_direct = cmoffset - eyeOffset;

    cm.distance = glm::length(cm.Move_direct);
    cm.Move_direct = glm::normalize(cm.Move_direct);


    return cm;
}
