/**************************************************************************
 * @file Input.cpp
 * @author 	Aaron Chan Jun Xiang
 * @param DP email: aaronjunxiang.chan@digipen.edu [2200880]
 * @param Course: CS 3401
 * @param Course: Game Project 3
 * @date  10/06/2024 (06 OCTOBER 2024)
 * @brief This file defines the input system, including action mappings.
 *************************************************************************/

#include "pch.h"
#include "Input.h"
#include "Windows/WindowManager.h"

extern bool g_IsCamPanning;

 // Constructor to initialize default action mappings
InputSystem::InputSystem() {
    // Set up default action mappings
    SetActionMapping("MoveForward", GLFW_KEY_W);
    SetActionMapping("MoveBackward", GLFW_KEY_S);
    SetActionMapping("MoveLeft", GLFW_KEY_A);
    SetActionMapping("MoveRight", GLFW_KEY_D);
    SetActionMapping("Jump", GLFW_KEY_SPACE);
    SetActionMapping("Bark", GLFW_KEY_F);
	SetActionMapping("Sniff", GLFW_KEY_E);
	SetActionMapping("Sprint", GLFW_KEY_LEFT_SHIFT);
    SetActionMapping("Biting", GLFW_MOUSE_BUTTON_LEFT);
    SetActionMapping("Escape", GLFW_KEY_ESCAPE);
}

/**************************************************************************
 * @brief Gets the current state of a specific key.
 * @param index The index of the keyboard key.
 * @return Integer representing the state of the key.
 *************************************************************************/
int InputSystem::GetKeyState(int index) {
    if (g_IsCamPanning)
        return 0;
    if (inputLocked)
        return false;
    return keyStates[index];
}

/**************************************************************************
 * @brief Sets the state of a specific keyboard key.
 * @param index The index of the keyboard key.
 * @param value State to set: 0 for released, 1 for pressed, 2 for held.
 *************************************************************************/
void InputSystem::SetKeyState(int index, int value) {
    keyStates[index] = value;
}

/**************************************************************************
 * @brief Gets the state of a specific mouse button.
 * @param index The index of the mouse button.
 * @return Boolean representing the mouse button's state.
 *************************************************************************/
int InputSystem::GetMouseState(int index) {
    if (g_IsCamPanning)
        return 0;
    if (inputLocked)
        return false;

    return mouseButtonStates[index];
}

/**************************************************************************
 * @brief Sets the state of a specific mouse button.
 * @param index The index of the mouse button to set.
 * @param value The new state to assign: 1 for pressed, 0 for released.
 *************************************************************************/
void InputSystem::SetMouseState(int index, int value) {
    mouseButtonStates[index] = value;
}

/**************************************************************************
 * @brief Retrieves the scroll state of the mouse wheel.
 * @return Integer representing the scroll state.
 *************************************************************************/
int InputSystem::GetScrollState() {
    if (g_IsCamPanning)
        return 0;
    if (inputLocked)
        return false;

    return mouseScrollState;
}

/**************************************************************************
 * @brief Sets the scroll state of the mouse wheel.
 * @param value Scroll state: 1 for scrolling up, 0 for no scrolling, -1 for scrolling down.
 *************************************************************************/
void InputSystem::SetScrollState(int value) {
    mouseScrollState = value;
}

/**************************************************************************
 * @brief Updates the total Y offset of the mouse scroll wheel.
 * @param val Value to add to the total Y offset.
 *************************************************************************/
void InputSystem::UpdateScrollTotalYOffset(float val) {
    mouse_scroll_total_Y_offset += val;
}

/**************************************************************************
 * @brief Retrieves the total Y offset of the mouse scroll wheel.
 * @return The total Y offset value.
 *************************************************************************/
float InputSystem::GetScrollTotalYOffset() {
    return mouse_scroll_total_Y_offset;
}

/**************************************************************************
 * @brief Updates the states of keyboard keys and mouse scroll for the next frame.
 *************************************************************************/
glm::vec2 last_mouse_pos = {};
void InputSystem::UpdateStatesForNextFrame() {

    for (int i = 0; i < GLFW_KEY_LAST + 1; ++i) {
        //keyStates[i] = ((keyStates[i] == 1) ? 2 : keyStates[i]);
        if (GetKeyState(i) == 1) {
            //std::cout << "Press\n";
            SetKeyState(i,2);
            continue;
        }
        if (GetKeyState(i) == 2) {
            //std::cout << "Hold\n";
            continue;
        }
    }

    for (int i = 0; i < GLFW_MOUSE_BUTTON_LAST + 1; ++i) {
        if (GetMouseState(i) == 1) {
            //std::cout << "Click\n";
            SetMouseState(i, 2);
            continue;
        }
        if (GetMouseState(i) == 2) {
            //std::cout << "Hold\n";
            continue;
        }
    }

    mouseScrollState = 0;

    // Get mouse position
    double x, y;
    glfwGetCursorPos(g_Window->GetGLFWWindow(), &x, &y);
    mouse_position = glm::vec2(x, y);
	static bool first = true;
	if (first) {
        mouse_pos_UI = mouse_position;
		first = false;
	}
    if (mouse_pos_UI == last_mouse_pos) {
        mouse_pos_UI = mouse_position;
    }
    last_mouse_pos = mouse_position;
	
    //std::cout << "Mouse scroll val: " << mouse_scroll_total_Y_offset << std::endl;
}

/**************************************************************************
 * @brief Sets an action mapping to a specific key.
 * @param action The action to be mapped.
 * @param key The key code to map the action to.
 *************************************************************************/
void InputSystem::SetActionMapping(const char * action, int key) {
    actionMappings[action] = key;
}

/**************************************************************************
 * @brief Checks if a specific action is currently pressed.
 * @param action The action to check.
 * @return True if the action is pressed, false otherwise.
 *************************************************************************/
bool InputSystem::IsActionPressed(const char * action) {

    if (g_IsCamPanning)
        return false;

    if (inputLocked)
        return false;


    auto it = actionMappings.find(action);
    if (it != actionMappings.end()) {
        int key = it->second;
        return (GetKeyState(key) == 1 || GetKeyState(key) == 2); // 1 = pressed, 2 = held
    }
    return false;
}
