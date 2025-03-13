/**************************************************************************
 * @file NodeComponent.hpp
 * @param DP email:
 * @param Course: CS 3401
 * @param Course: Game Project 3
 * @date 10/01/2025
 * @brief
 *
 * This file contains the definition of the NodeComponent class, which is used to
 * store the node data of an entity.
 *
 *************************************************************************/

#pragma once
#ifndef NODE_COMPONENT_HPP
#define NODE_COMPONENT_HPP

#pragma warning(push)
#pragma warning(disable: 6385 6386)

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "ECS/Coordinator.hpp"
#include "../Core/Reflection/ReflectionManager.hpp"

class NodeComponent {
public:
    NodeComponent(); // Constructor does NOT auto-increment counter

    // Setters
    void SetPosition(const glm::vec3& pos) { position = pos; }
    void SetWalkable(bool walkable) { isWalkable = walkable; }
    void SetNodeID(uint32_t id) { nodeID = id; } // Allow manual assignment

    // Getters
    glm::vec3 GetPosition() const { return position; }
    bool IsWalkable() const { return isWalkable; }
    uint32_t GetNodeID() const { return nodeID; } // Get unique node ID

    // Generate a unique Node ID (only called when a node is assigned in BuildGraph)
    static uint32_t GenerateNodeID() {
        //std::cout << "[DEBUG] Generating new NodeID: " << nodeCounter << "\n";
        return nodeCounter++;
    }

    // Debugging utilities
    static uint32_t GetNodeCounter() { return nodeCounter; }
    static void ResetNodeCounter() { nodeCounter = 0; }

    REFLECT_COMPONENT(NodeComponent)
    {
        REGISTER_PROPERTY(NodeComponent, Position, glm::vec3, SetPosition, GetPosition);
        REGISTER_PROPERTY(NodeComponent, Walkable, bool, SetWalkable, IsWalkable);
        REGISTER_PROPERTY(NodeComponent, NodeID, uint32_t, SetNodeID, GetNodeID);
    }

private:
    glm::vec3 position = glm::vec3(0.0f); // Default to (0,0,0)
    bool isWalkable = true;  // Whether this node is walkable
    uint32_t nodeID = INVALID_ENTITY; // Default to an invalid entity

    static uint32_t nodeCounter; // Static counter for node IDs
};

#endif
#pragma warning(pop)
