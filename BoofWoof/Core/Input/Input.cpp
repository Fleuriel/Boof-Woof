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

 // Constructor to initialize default action mappings
InputSystem::InputSystem() {
    // Set up default action mappings
    SetActionMapping(ActionType::MoveForward, GLFW_KEY_W);
    SetActionMapping(ActionType::MoveBackward, GLFW_KEY_S);
    SetActionMapping(ActionType::MoveLeft, GLFW_KEY_A);
    SetActionMapping(ActionType::MoveRight, GLFW_KEY_D);
    SetActionMapping(ActionType::Jump, GLFW_KEY_SPACE);
    SetActionMapping(ActionType::Shoot, GLFW_MOUSE_BUTTON_LEFT);
}

/**************************************************************************
 * @brief Gets the current state of a specific key.
 * @param index The index of the keyboard key.
 * @return Integer representing the state of the key.
 *************************************************************************/
int InputSystem::GetKeyState(int index) {
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
bool InputSystem::GetMouseState(int index) {
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
void InputSystem::UpdateStatesForNextFrame() {
    for (size_t i = 0; i < GLFW_KEY_LAST + 1; ++i) {
        keyStates[i] = (keyStates[i] == 1) ? 2 : keyStates[i];
    }
    mouseScrollState = 0;

    // Get mouse position
    double x, y;
    glfwGetCursorPos(g_Window->GetGLFWWindow(), &x, &y);
    mouse_position = glm::vec2(x, y);
}

/**************************************************************************
 * @brief Sets an action mapping to a specific key.
 * @param action The action to be mapped.
 * @param key The key code to map the action to.
 *************************************************************************/
void InputSystem::SetActionMapping(ActionType action, int key) {
    actionMappings[action] = key;
}

/**************************************************************************
 * @brief Checks if a specific action is currently pressed.
 * @param action The action to check.
 * @return True if the action is pressed, false otherwise.
 *************************************************************************/
bool InputSystem::IsActionPressed(ActionType action) {
    auto it = actionMappings.find(action);
    if (it != actionMappings.end()) {
        int key = it->second;
        return (GetKeyState(key) == 1 || GetKeyState(key) == 2); // 1 = pressed, 2 = held
    }
    return false;
}
