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

#include "GameStateMachine.h"
#include <iostream>


std::chrono::high_resolution_clock::time_point currentTime;
std::chrono::high_resolution_clock::time_point previousTime;
std::chrono::duration<double> deltaTime;

double seconds;


STATE currentState = STATE::START;
STATE nextState = STATE::RUNNING;
STATE previousState = STATE::STARTUPPREVIOUS;

/**************************************************************************
* @brief On Startup Run:
* @param NONE
* @return VOID
*************************************************************************/
void StartUp()
{
	// Set States
	currentState = STATE::START;
	previousState = STATE::STARTUPPREVIOUS;
	nextState = STATE::RUNNING;

	// Change of States

	currentState = nextState;

}


/**************************************************************************
* @brief  Changing States
* @param  CURRENT STATE
* @param  NEXT STATE
* @return void
*************************************************************************/
void ChangeState(STATE Current, STATE Upcoming)
{
	//Swap States.
	currentState = Upcoming;
	previousState = Current;	
}



double FPS()
{
	return 1 / deltaTime.count();
}

double DeltaTime()
{
	return deltaTime.count();
}


bool IsTimeElapsed(unsigned int time) {
	// only run once.
	// Get Time Elapsed.
	seconds += DeltaTime();
#ifdef _DEBUG	
	//std::cout << seconds << GetDT() << '\n';
#endif
	// refresh seconds to 0
	if (seconds >= time) {
		seconds = 0;
		return true;

	}
	else
		return false;
}