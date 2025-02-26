#pragma once
#ifndef CAMERA_H
#define CAMERA_H


#include "ECS/Coordinator.hpp"
#include "../Core/Reflection/ReflectionManager.hpp"

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum class Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 3.0f;
const float SENSITIVITY = 0.15f;
const float ZOOM = 15.f;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class CameraComponent
{
public:
	bool IsActive = false; // only one camera can be active at a time for entiry entities
    
    // camera Attributes
    glm::vec3 Position{};
    glm::vec3 Front{};
    glm::vec3 Up{};
    glm::vec3 Right{};
    glm::vec3 Down{};
    glm::vec3 WorldUp{};
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
	float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    float width{ 1280 }, height{ 960 };

    // constructor with vectors
    CameraComponent(glm::vec3 position = glm::vec3(0.0f, 0.0f, 8.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH, bool isActive = false) {
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		MovementSpeed = SPEED;
		MouseSensitivity = SENSITIVITY;
        IsActive = isActive;
		updateCameraVectors();
    }

    // constructor with scalar values
    CameraComponent(float posX, float posY, float posZ, float upX, float upY, float upZ, bool isActive, float yaw, float pitch) {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
		MovementSpeed = SPEED;
		MouseSensitivity = SENSITIVITY;
        IsActive = isActive;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix() {
		return glm::lookAt(Position, Position + Front, Up);
	}

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime) {
		float velocity = MovementSpeed * deltaTime;
        if (direction == Camera_Movement::FORWARD) 
            Position += Front * velocity;
		if (direction == Camera_Movement::BACKWARD)
			Position -= Front * velocity;
		if (direction == Camera_Movement::LEFT)
			Position -= Right * velocity;
		if (direction == Camera_Movement::RIGHT)
			Position += Right * velocity;
		if (direction == Camera_Movement::UP)
			Position += Up * velocity;
		if (direction == Camera_Movement::DOWN)
			Position += Down * velocity;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset) {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

    //void RenderMiniMapCam();

    // setter
	void SetCameraDirection(glm::vec3 direction) {
		// update Yaw and Pitch
		Yaw = glm::degrees(atan2(direction.z, direction.x));
		Pitch = glm::degrees(asin(direction.y));
		updateCameraVectors();
	}

	void SetCameraYaw(float yaw) {
		Yaw = yaw;
		updateCameraVectors();
	}

	void SetCameraPitch(float pitch) {
		Pitch = pitch;
		updateCameraVectors();
	}

	void SetCameraPosition(glm::vec3 position) {
		Position = position;
        updateCameraVectors();
	}

	void SetCameraUp(glm::vec3 up) {
		WorldUp = up;
		updateCameraVectors();
	}

	void SetCameraActive(bool isActive) {
        IsActive = isActive;
	}

	// getter
	glm::vec3 GetCameraDirection() {
		return Front;
	}

	float GetCameraYaw() {
		return Yaw;
	}

	float GetCameraPitch() {
		return Pitch;
	}

	glm::vec3 GetCameraPosition() {
		return Position;
	}

	glm::vec3 GetCameraUp() {
		return WorldUp;
	}

	bool GetCameraActive() {
		return IsActive;
	}

	glm::vec3 GEtCameraRight() {
		return Right;
	}

    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors() {
        // calculate the new Front vector

        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = glm::normalize(glm::cross(Right, Front));

        Down = -Up;
    }

	// Reflection integration
    REFLECT_COMPONENT(CameraComponent)
    {
        REGISTER_PROPERTY(CameraComponent, Position, glm::vec3, SetCameraPosition, GetCameraPosition);
        REGISTER_PROPERTY(CameraComponent, WorldUp, glm::vec3, SetCameraUp, GetCameraUp);
        REGISTER_PROPERTY(CameraComponent, Pitch, float, SetCameraPitch, GetCameraPitch);
        REGISTER_PROPERTY(CameraComponent, Yaw, float, SetCameraYaw, GetCameraYaw);
        REGISTER_PROPERTY(CameraComponent, IsActive, bool, SetCameraActive, GetCameraActive);
    }
};


#endif