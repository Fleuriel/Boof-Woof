/**************************************************************************
 * @file Input.cpp
 * @author 	Aaron Chan Jun Xiang
 * @param DP email: aaronjunxiang.chan@digipen.edu [2200880]
 * @param Course: CS 3401
 * @param Course: Game Project 3
 * @date  10/06/2024 (06 OCTOBER 2024)
 * @brief
 *
 * This file defines the input system
 *
 *************************************************************************/
#include "pch.h"
#include "Input.h"
#include "Windows/WindowManager.h"
int InputSystem::GetKeyState(int index) {
	return keyStates[index];
}

/**************************************************************************
 * @brief Sets the state of a specific keyboard key.
 * @param index The index of the keyboard key to set the state for.
 * @param value An integer representing the state to set: 0 for released, 1 for pressed, 2 for held.
 * @return None
 *************************************************************************/
void InputSystem::SetKeyState(int index, int value) {
	keyStates[index] = value;
}


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

int InputSystem::GetScrollState() {
	return mouseScrollState;
}

/**************************************************************************
 * @brief Sets the state of the mouse scroll wheel.
 * @param value An integer representing the scroll state: 1 for scrolling up, 0 for no scrolling, -1 for scrolling down.
 *************************************************************************/
void InputSystem::SetScrollState(int value) {
	mouseScrollState = value;
}

/**************************************************************************
 * @brief Updates the total Y offset of the mouse scroll wheel.
 * @param val The value to add to the total Y offset.
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

void InputSystem::UpdateStatesForNextFrame() {

	// Loop through all keyboard keys (represented by indices)
	for (size_t i = 0; i < GLFW_KEY_LAST + 1; ++i) {

		// Update the state of each key
		// If the key state is 1 (pressed in the current frame), change it to 2 (held down)
		// If the key state is 0 (not pressed), it remains 0
		keyStates[i] = ((keyStates[i] == 1) ? 2 : keyStates[i]);
	}

	// Reset the mouse scroll state to 0 for the next frame
	mouseScrollState = 0;

	//get mouse position
	double x, y;
	glfwGetCursorPos(g_Window->GetGLFWWindow(), &x, &y);
	mouse_position = glm::vec2(x, y);
}