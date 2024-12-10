/**************************************************************************
 * @file PathfindingSystem.h
 * @author 	Ang Jun Sheng Aloysius
 * @param DP email: a.junshengaloysius@digipen.edu [2201807]
 * @param Course: CS 3401
 * @param Course: Game Project 3
 * @date  11/12/2024 (12 DEC 2024)
 * @brief
 *
 * This file contains the declarations to update the pathfinding coordinates
 *
 *
 *************************************************************************/

#ifndef PATHFINDINGSYSTEM_H
#define PATHFINDINGSYSTEM_H

#pragma warning(push)
#pragma warning(disable: 4100 4189 4081 4458)

#include "../ECS/System.hpp"

// Class to interface with JoltPhysics
class PathfindingSystem : public System {
public:

    /**************************************************************************/
    /*!
    \brief Updates the Pathfinding coordinates.
    */
    /**************************************************************************/
    void OnUpdate();

    void Cleanup();


private:

};



#endif // PATHFINDINGSYSTEM_H
