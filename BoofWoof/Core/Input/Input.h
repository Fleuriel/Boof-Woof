/**************************************************************************
 * @file Input.h
 * @author 	Aaron Chan Jun Xiang
 * @param DP email: aaronjunxiang.chan@digipen.edu [2200880]
 * @param Course: CS 3401
 * @param Course: Game Project 3
 * @date  10/06/2024 (06 OCTOBER 2024)
 * @brief This file declares the input system with action mapping functionality.
 *************************************************************************/

#pragma once

#ifndef INPUT_H
#define INPUT_H

#include "../Graphics/GraphicsSystem.h"

#define g_Input InputSystem::GetInstance()

 /**************************************************************************
  * @brief Enum for different action types
  *************************************************************************/
enum class ActionType {
    MoveForward,
    MoveBackward,
    MoveLeft,
    MoveRight,
    Jump,
	Bark,
	Sniff,
    Biting
    // Add more actions as needed
};

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

    InputSystem();

    // Action Mapping Methods
    void SetActionMapping(const char *, int key);               // Set key for action
    bool IsActionPressed(const char * action);                         // Check if action is pressed

    // Key and Mouse State Methods
    int GetKeyState(int index);                                      // Get KeyStates
    void SetKeyState(int index, int value);                          // Set KeyStates
    int GetMouseState(int index);                                   // Get MouseStates
    void SetMouseState(int index, int value);                        // Set MouseStates
    glm::vec2 GetMousePosition() { return mouse_position; };         // Get Mouse Position

	// UI Mouse Position
	void SetMousePositionUI(glm::vec2 pos) { mouse_pos_UI = pos; };  // Set Mouse Position for UI
	glm::vec2 GetMousePositionUI() { return mouse_pos_UI; };         // Get Mouse Position for UI


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
     *************************************************************************/
    void UpdateStatesForNextFrame();

    // Other Variables
    bool capsLockReleased{ true };
    bool capsLockOn{ false };
    bool typePW{ false };
    std::string hiddenconsole{};
    bool buttonPressed{ false };

private:
    std::unordered_map<const char*, int> actionMappings;              // Maps actions to keys
    std::array<int, GLFW_KEY_LAST + 1> keyStates{};                  // Array to keep track of key states
    std::array<int, GLFW_MOUSE_BUTTON_LAST + 1> mouseButtonStates{}; // Array to keep track of mouse button states

    // Mouse and Scroll State Variables
    int mouseScrollState{ 0 };                                       // 1 for scrolling up, 0 for not scrolling, -1 for scrolling down
    float mouse_scroll_total_Y_offset{ 0 };                          // Keeps track of total vertical scrolling
    glm::vec2 mouse_position{ 0, 0 };                                // Stores mouse position
    
	glm::vec2 mouse_pos_UI{ 0, 0 };                                  // Stores mouse position for UI
};

#endif
