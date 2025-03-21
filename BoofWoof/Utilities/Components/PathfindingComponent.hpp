/**************************************************************************
 * @file PathfindingComponent.hpp
 * @param DP email:
 * @param Course: CS 3401
 * @param Course: Game Project 3
 * @date 10/01/2025
 * @brief
 *
 * This file contains the definition of the PathfindingComponent class, which is used to
 * store the pathfinding data of an entity.
 *
 *************************************************************************/

#pragma once
#ifndef PATHFINDING_COMPONENT_HPP
#define PATHFINDING_COMPONENT_HPP

#pragma warning(push)
#pragma warning(disable: 6385 6386)

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "ECS/Coordinator.hpp"
#include "../Core/Reflection/ReflectionManager.hpp"

enum class PathfindingStatus {
    IDLE,
    PROCESSING,
    COMPLETE,
    FAILED
};

class PathfindingComponent
{
public:
    PathfindingComponent() = default;

    void SetStartNode(Entity node) { startNodeID = node; }
    Entity GetStartNode() const { return startNodeID; }

    void SetGoalNode(Entity node) { goalNodeID = node; }
    Entity GetGoalNode() const { return goalNodeID; }

    void SetPath(const std::vector<glm::vec3>& newPath) { path = newPath; }
    const std::vector<glm::vec3>& GetPath() const { return path; }

    void SetStatus(PathfindingStatus status) { currentStatus = status; }
    PathfindingStatus GetStatus() const { return currentStatus; }

    void SetBuilt(bool built) { m_built = built; }
    bool GetBuilt() const { return m_built; }

    REFLECT_COMPONENT(PathfindingComponent)
    {
        //REGISTER_PROPERTY(PathfindingComponent, StartNode, Entity, SetStartNode, GetStartNode);
        //REGISTER_PROPERTY(PathfindingComponent, GoalNode, Entity, SetGoalNode, GetGoalNode);
        REGISTER_PROPERTY(PathfindingComponent, StartNodeID, uint32_t, SetStartNode, GetStartNode);
        REGISTER_PROPERTY(PathfindingComponent, GoalNodeID, uint32_t, SetGoalNode, GetGoalNode);
    }

private:
    //Entity startNode;
    //Entity goalNode;
    uint32_t startNodeID;
    uint32_t goalNodeID;
    std::vector<glm::vec3> path;
    PathfindingStatus currentStatus = PathfindingStatus::IDLE;
    bool m_built = false;
};

#endif // PATHFINDING_COMPONENT_HPP

#pragma warning(pop)
