/**************************************************************************
 * @file Input.h
 * @author 	Aaron Chan Jun Xiang
 * @param DP email: aaronjunxiang.chan@digipen.edu [2200880]
 * @param Course: CS 3401
 * @param Course: Game Project 3
 * @date  10/06/2024 (06 OCTOBER 2024)
 * @brief
 *
 * This file declares the input system
 *
 *************************************************************************/

#pragma once

#ifndef INPUT_H
#define INPUT_H

#include "../Graphics/GraphicsSystem.h"
#include <vector>
#include <array>
#include <iostream>

#define g_Input InputSystem::GetInstance()

/**************************************************************************
 * @brief InputSystem Class
 *************************************************************************/
class InputSystem
{
public:
	static InputSystem& GetInstance() {
		static InputSystem instance;
		return instance;
	}

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

	// true for on, false for off
	bool capsLockReleased{ true };
	bool capsLockOn{ false };

	bool typePW{ false };
	std::string hiddenconsole{};
	bool buttonPressed{ false };

private:
	// Define an array to keep track of key states
	std::array<int, GLFW_KEY_LAST + 1> keyStates{};

	// Define an array to keep track of mouse button states
	std::array<int, GLFW_MOUSE_BUTTON_LAST + 1> mouseButtonStates{};

	// 1 for scrolling up, 0 for not scrolling, -1 for scrolling down
	int mouseScrollState{ 0 };

	// keeps track of total vertical scrolling
	float mouse_scroll_total_Y_offset{ 0 };

};

#endif