/**************************************************************************
 * @file EdgeComponent.hpp
 * @param DP email:
 * @param Course: CS 3401
 * @param Course: Game Project 3
 * @date 10/01/2025
 * @brief
 *
 * This file contains the definition of the EdgeComponent class, which is used to
 * store the edge data of an entity.
 *
 *************************************************************************/

#pragma once
#ifndef EDGE_COMPONENT_HPP
#define EDGE_COMPONENT_HPP

#pragma warning(push)
#pragma warning(disable: 6385 6386)

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "ECS/Coordinator.hpp"
#include "../Core/Reflection/ReflectionManager.hpp"

class EdgeComponent {
public:

    EdgeComponent(Entity start = {}, Entity end = {}, float edgeCost = 1.0f)
        : startNodeID(start), endNodeID(end), cost(edgeCost) {}

    void SetStartNode(Entity start) { startNodeID = start; }
    Entity GetStartNode() const { return startNodeID; }
    void SetEndNode(Entity end) { endNodeID = end; }
    Entity GetEndNode() const { return endNodeID; }
    void SetCost(float edgeCost) { cost = edgeCost; }
    float GetCost() const { return cost; }

    REFLECT_COMPONENT(EdgeComponent)
    {
        REGISTER_PROPERTY(EdgeComponent, StartNodeID, Entity, SetStartNode, GetStartNode);
        REGISTER_PROPERTY(EdgeComponent, EndNodeID, Entity, SetEndNode, GetEndNode);
        REGISTER_PROPERTY(EdgeComponent, Cost, float, SetCost, GetCost);
    }

private:
    Entity startNodeID;  // Starting node of the edge
    Entity endNodeID;    // Ending node of the edge
    float cost;        // Traversal cost
};

#endif

#pragma warning(pop)
