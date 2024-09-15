/**************************************************************************
 * @file GameStateMachine.cpp
 * @author 	TAN Angus Yit Hoe
 * @param DP email: tan.a@digipen.edu [0067684]
 * @param Course: CS 350
 * @param Course: Advanced Computer Graphics II
 * @date  06/14/2024 (14 JUNE 2024)
 * @brief
 *
 * This file allows changing of states, while having the added functionality
 * of getting FPS and DeltaTime in the game.
 *
 *
 *************************************************************************/

#pragma once
#ifndef GAME_STATE_MACHINE_H
#define GAME_STATE_MACHINE_H

#include <chrono>
#include <iostream>

/**************************************************************************
* @brief STATE Enumeration Class
*************************************************************************/
enum class STATE
{
	START = 0,
	RUNNING,
	END,
	

	STARTUPPREVIOUS = 2147483647
};

// FPS
extern std::chrono::high_resolution_clock::time_point currentTime;
extern std::chrono::high_resolution_clock::time_point previousTime;
extern std::chrono::duration<double> deltaTime;

// States
extern STATE currentState, previousState, nextState;


/**************************************************************************
* @brief Initialize Startup Variable
*************************************************************************/
void StartUp();

/**************************************************************************
* @brief Changing States
*************************************************************************/
void ChangeState(STATE Current, STATE Upcoming);



// FPS
double FPS();

// DeltaTime
double DeltaTime();

// Is Time Elapsed in Seconds
bool IsTimeElapsed(unsigned int time);




#endif



