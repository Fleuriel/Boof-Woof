/**************************************************************************
 * @file Input.h
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
#pragma once

#ifndef INPUT_H
#define INPUT_H

#include "../Graphics/GraphicsSystem.h"
#include <vector>
#include <array>
#include <iostream>

/**************************************************************************
 * @brief InputSystem Class
 *************************************************************************/
class InputSystem
{
public:
	int GetKeyState(int index);										// Get KeyStates

	void SetKeyState(int index, int value);							// Setting KeyStates
	
	bool GetMouseState(int index);									// Get MouseStates

	void SetMouseState(int index, int value);						// Setting MouseStates
	
	/**************************************************************************
	 * @brief Retrieves the state of the mouse scroll wheel.
	 * @return An integer representing the scroll state: 1 for scrolling up, 0 for no scrolling, -1 for scrolling down.
	 *************************************************************************/
	int GetScrollState();

	/**************************************************************************
	 * @brief Sets the state of the mouse scroll wheel.
	 * @param value An integer representing the scroll state: 1 for scrolling up, 0 for no scrolling, -1 for scrolling down.
	 *************************************************************************/
	void SetScrollState(int);


	/**************************************************************************
	 * @brief Updates the total Y offset of the mouse scroll wheel.
	 * @param val The value to add to the total Y offset.
	 *************************************************************************/
	void UpdateScrollTotalYOffset(float);

	/**************************************************************************
	 * @brief Retrieves the total Y offset of the mouse scroll wheel.
	 * @return The total Y offset value.
	 *************************************************************************/
	float GetScrollTotalYOffset();

	/**************************************************************************
	 * @brief Updates the states of keyboard keys and mouse scroll for the next frame.
	 *
	 * This function is typically called once per frame to update the state of keyboard
	 * keys and reset the mouse scroll state for the next frame.
	 *
	 * @note It assumes that the `keyStates` array has been previously initialized to store
	 *       the state of each keyboard key, and `mouseScrollState` has been initialized
	 *       to store the state of the mouse scroll wheel.
	 *************************************************************************/
	void UpdateStatesForNextFrame();

	bool typePW{ false };
	std::string hiddenconsole{};
	bool buttonPressed{ false };

private:
	// Define an array to keep track of key states
	std::array<int, GLFW_KEY_LAST + 1> keyStates{};

	// Define an array to keep track of mouse button states
	std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> mouseButtonStates{};

	// 1 for scrolling up, 0 for not scrolling, -1 for scrolling down
	int mouseScrollState{ 0 };

	// keeps track of total vertical scrolling
	float mouse_scroll_total_Y_offset{ 0 };

};


extern InputSystem inputSystem;

/**************************************************************************
 * @brief Callback function for handling keyboard input in a GLFW window.
 *
 * This function is a callback used with the GLFW library to handle keyboard input events.
 *
 * @param window The GLFW window that received the input.
 * @param key The keyboard key code that was pressed or released.
 * @param scancode The system-specific scancode of the key.
 * @param action The action taken (GLFW_PRESS, GLFW_RELEASE, GLFW_REPEAT).
 * @param mod Bitfield describing which modifier keys (e.g., Shift, Ctrl, Alt) were held down.
 *
 * This function updates various input states based on the keyboard input events.
 * - When a key is pressed (action == GLFW_PRESS), it updates the keyStates array
 *   to record the state of alphabets, numbers, special keys, and keyboard commands.
 * - It also responds to specific key combinations like Ctrl+C, Ctrl+V, Ctrl+X, and Ctrl+Z
 *   for copy, paste, cut, and undo commands, respectively.
 * - Additionally, it can set or clear certain input states based on the key pressed,
 *   such as Caps Lock, Tab, and Escape.
 *
 * @note The UNREFERENCED_PARAMETER macro is used to suppress unused parameter warnings.
 * @note This function is typically registered with GLFW using glfwSetKeyCallback().
 *
 * @see keyStates - The array used to store the state of various keyboard keys.
 * @see glfwSetKeyCallback() - Function to register this callback with GLFW.
 *************************************************************************/
void KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mod);

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
void MouseCallBack(GLFWwindow* window, int button, int action, int mod);

#endif