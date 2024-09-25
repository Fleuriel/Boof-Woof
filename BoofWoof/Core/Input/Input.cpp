/**************************************************************************
 * @file Input.cpp
 * @author 	TAN Angus Yit Hoe
 * @param DP email: tan.a@digipen.edu [0067684]
 * @param Course: CS 350
 * @param Course: Advanced Computer Graphics II
 * @date  06/14/2024 (14 JUNE 2024)
 * @brief
 *
 * This file allows usage of Input such as Mouse clicks and Keyboard 
 * key inputs.
 *
 *
 *************************************************************************/

#include "Input.h"



InputSystem inputSystem;


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




/**************************************************************************
 * @brief Callback function for handling mouse button input in a GLFW window.
 *
 * This function is a callback used with the GLFW library to handle mouse button input events.
 *
 * @param window The GLFW window that received the input.
 * @param button The mouse button that was pressed or released.
 * @param action The action taken (GLFW_PRESS or GLFW_RELEASE).
 * @param mod Bitfield describing which modifier keys (e.g., Shift, Ctrl, Alt) were held down.
 *
 * This function updates the mouseButtonStates array based on mouse button input events. It specifically
 * records the state of the mouse buttons based on whether they are pressed or released.
 *
 * @note The UNREFERENCED_PARAMETER macro is used to suppress unused parameter warnings.
 * @note This function is typically registered with GLFW using glfwSetMouseButtonCallback().
 *
 * @see mouseButtonStates - The array used to store the state of various input events.
 * @see glfwSetMouseButtonCallback() - Function to register this callback with GLFW.
 *************************************************************************/
void MouseCallBack(GLFWwindow* window4, int button, int action, int mod) {

	(void)window4;

	(void)mod;
	//UNREFERENCED_PARAMETER(window);
	//UNREFERENCED_PARAMETER(mod);



	inputSystem.SetMouseState(button, action);

	inputSystem.buttonPressed = action;
}


/**************************************************************************
 * @brief Callback function for handling mouse scroll wheel input in a GLFW window.
 *
 * This function is a callback used with the GLFW library to handle mouse scroll wheel input events.
 *
 * @param window The GLFW window that received the input.
 * @param xOffset The horizontal scroll offset (not used in this function).
 * @param yOffset The vertical scroll offset, indicating the amount of scrolling.
 *
 * This function updates the `mouse_scroll_total_Y_offset` variable to keep track of the total vertical
 * scrolling that has occurred. It also sets the mouseScrollState variable to indicate whether scrolling
 * is in the upward (INPUT_SCROLLUP) or downward (INPUT_SCROLLDOWN) direction based on the `yOffset` value.
 *
 * @param mouse_scroll_total_Y_offset A global variable that tracks the total vertical scrolling.
 * @param mouseScrollState The array used to store the state of various input events.
 *
 * @note The UNREFERENCED_PARAMETER macro is not used in this function.
 * @note This function is typically registered with GLFW using glfwSetScrollCallback().
 *
 * @see mouse_scroll_total_Y_offset - Global variable to track total vertical scrolling.
 * @see mouseScrollState - The array used to store the state of various input events.
 * @see glfwSetScrollCallback() - Function to register this callback with GLFW.
 *************************************************************************/
void ScrollCallBack(GLFWwindow* window5, double xOffset, double yOffset) {

	(void)xOffset;
	(void)window5;
	//UNREFERENCED_PARAMETER(window);
	//UNREFERENCED_PARAMETER(xOffset);


	//Update variable to track total vertical scrolling
	inputSystem.UpdateScrollTotalYOffset(static_cast<float>(yOffset));

	//Change the scroll states based on y offset value
	inputSystem.SetScrollState((yOffset > 0) ? 1 : (yOffset == 0) ? 0 : -1);
}


void InputSystem::UpdateStatesForNextFrame() {

	// Loop through all keyboard keys (represented by indices)
	for (size_t i = 0; i < GLFW_KEY_LAST + 1; ++i) {

		// Update the state of each key
		// If the key state is 1 (pressed in the current frame), change it to 2 (held down)
		// If the key state is 0 (not pressed), it remains 0
		keyStates[i] = (keyStates[i] == 1) ? 2 : keyStates[i];
	}

	// Reset the mouse scroll state to 0 for the next frame
	mouseScrollState = 0;
}