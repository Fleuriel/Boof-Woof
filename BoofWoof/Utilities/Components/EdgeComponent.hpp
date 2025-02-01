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
        : startNode(start), endNode(end), cost(edgeCost) {}

    void SetStartNode(Entity start) { startNode = start; }
    Entity GetStartNode() const { return startNode; }
    void SetEndNode(Entity end) { endNode = end; }
    Entity GetEndNode() const { return endNode; }
    void SetCost(float edgeCost) { cost = edgeCost; }
    float GetCost() const { return cost; }

    REFLECT_COMPONENT(EdgeComponent)
    {
        REGISTER_PROPERTY(EdgeComponent, StartNode, Entity, SetStartNode, GetStartNode);
        REGISTER_PROPERTY(EdgeComponent, EndNode, Entity, SetEndNode, GetEndNode);
        REGISTER_PROPERTY(EdgeComponent, Cost, float, SetCost, GetCost);
    }

private:
    Entity startNode;  // Starting node of the edge
    Entity endNode;    // Ending node of the edge
    float cost;        // Traversal cost
};

#endif

#pragma warning(pop)
