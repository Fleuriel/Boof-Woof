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
//
//#pragma once
//#ifndef NODE_COMPONENT_HPP
//#define NODE_COMPONENT_HPP
//
//#pragma warning(push)
//#pragma warning(disable: 6385 6386)
//
//#include <glm/glm.hpp>
//#include <vector>
//#include <string>
//#include "ECS/Coordinator.hpp"
//#include "../Core/Reflection/ReflectionManager.hpp"
//
//class NodeComponent {
//public:
//    // Default Constructor - Assigns a unique node ID
//    NodeComponent();
//
//    // Setters
//    void SetPosition(const glm::vec3& pos) { position = pos; }
//    void SetWalkable(bool walkable) { isWalkable = walkable; }
//
//    // Getters
//    glm::vec3 GetPosition() const { return position; }
//    bool IsWalkable() const { return isWalkable; }
//
//    // Node ID Management
//    unsigned int GetNodeID() const { return nodeID; }
//
//    // Reflection System
//    REFLECT_COMPONENT(NodeComponent)
//    {
//        REGISTER_PROPERTY(NodeComponent, Position, glm::vec3, SetPosition, GetPosition);
//        REGISTER_PROPERTY(NodeComponent, Walkable, bool, SetWalkable, IsWalkable);
//    }
//
//private:
//    unsigned int nodeID;   // Unique node ID
//    glm::vec3 position;    // Node's position in the world
//    bool isWalkable = true; // Whether this node is walkable
//
//    // Static global node ID counter (Declared here but defined in .cpp)
//    static unsigned int globalNodeID;
//};
//
//#endif  // NODE_COMPONENT_HPP
//
//#pragma warning(pop)



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

    NodeComponent() : position(0.0f), isWalkable(true) {}

    void AddEdge(Entity edge) {
        connectedEdges.push_back(edge);
    }

    // Setters
    void SetPosition(const glm::vec3& pos) { position = pos; }
    void SetWalkable(bool walkable) { isWalkable = walkable; }

    // Getters
    glm::vec3 GetPosition() const { return position; }
    bool IsWalkable() const { return isWalkable; }

    REFLECT_COMPONENT(NodeComponent)
    {
        REGISTER_PROPERTY(NodeComponent, Position, glm::vec3, SetPosition, GetPosition);
        REGISTER_PROPERTY(NodeComponent, Walkable, bool, SetWalkable, IsWalkable);
    }

private:
    glm::vec3 position;  // Node's position in the world
    bool isWalkable = true;  // Whether this node is walkable
    std::vector<Entity> connectedEdges; // List of connected edges

};

#endif

#pragma warning(pop)
