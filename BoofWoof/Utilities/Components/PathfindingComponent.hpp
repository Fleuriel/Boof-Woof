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

    void SetStartNode(Entity node) { startNode = node; }
    Entity GetStartNode() const { return startNode; }

    void SetGoalNode(Entity node) { goalNode = node; }
    Entity GetGoalNode() const { return goalNode; }

    void SetPath(const std::vector<glm::vec3>& newPath) { path = newPath; }
    const std::vector<glm::vec3>& GetPath() const { return path; }

    void SetStatus(PathfindingStatus status) { currentStatus = status; }
    PathfindingStatus GetStatus() const { return currentStatus; }

    REFLECT_COMPONENT(PathfindingComponent)
    {
        REGISTER_PROPERTY(PathfindingComponent, StartNode, Entity, SetStartNode, GetStartNode);
        REGISTER_PROPERTY(PathfindingComponent, GoalNode, Entity, SetGoalNode, GetGoalNode);
    }

private:
    Entity startNode;
    Entity goalNode;
    std::vector<glm::vec3> path;
    PathfindingStatus currentStatus = PathfindingStatus::IDLE;
};

#endif // PATHFINDING_COMPONENT_HPP


// // Enum for pathfinding status
//enum class PathfindingStatus {
//    IDLE,
//    PROCESSING,
//    COMPLETE,
//    FAILED
//};
//
//// Heuristic options
//enum class HeuristicType {
//    EUCLIDEAN,
//    MANHATTAN,
//    OCTILE
//};
//
//class PathfindingComponent
//{
//public:
//    // Constructors
//    PathfindingComponent() = default;
//    PathfindingComponent(const glm::vec3& start, const glm::vec3& goal)
//        : startPosition(start), goalPosition(goal), isActive(false), currentStatus(PathfindingStatus::IDLE) {}
//
//    ~PathfindingComponent() = default;
//
//    // Setters
//    void SetStartPosition(const glm::vec3& start) { startPosition = start; }
//    void SetGoalPosition(const glm::vec3& goal) { goalPosition = goal; }
//    void SetActive(bool active) { isActive = active; }
//    void SetHeuristic(HeuristicType heuristic) { heuristicType = heuristic; }
//    void SetPath(const std::vector<glm::vec3>& newPath) { path = newPath; }
//    void SetStatus(PathfindingStatus status) { currentStatus = status; }
//    void EnableDebug(bool debug) { debugEnabled = debug; }
//
//    // Getters
//    glm::vec3 GetStartPosition() const { return startPosition; }
//    glm::vec3 GetGoalPosition() const { return goalPosition; }
//    bool IsActive() const { return isActive; }
//    HeuristicType GetHeuristic() const { return heuristicType; }
//    const std::vector<glm::vec3>& GetPath() const { return path; }
//    PathfindingStatus GetStatus() const { return currentStatus; }
//    bool IsDebugEnabled() const { return debugEnabled; }
//
//    // Reflection integration
//    REFLECT_COMPONENT(PathfindingComponent)
//    {
//        REGISTER_PROPERTY(PathfindingComponent, StartPosition, glm::vec3, SetStartPosition, GetStartPosition);
//        REGISTER_PROPERTY(PathfindingComponent, GoalPosition, glm::vec3, SetGoalPosition, GetGoalPosition);
//        REGISTER_PROPERTY(PathfindingComponent, Active, bool, SetActive, IsActive);
//        REGISTER_PROPERTY(PathfindingComponent, Heuristic, HeuristicType, SetHeuristic, GetHeuristic);
//        REGISTER_PROPERTY(PathfindingComponent, DebugEnabled, bool, EnableDebug, IsDebugEnabled);
//    }
//
//private:
//    glm::vec3 startPosition;                    // Start position for pathfinding
//    glm::vec3 goalPosition;                     // Goal position for pathfinding
//    bool isActive = false;                      // Whether this component is active for pathfinding
//    HeuristicType heuristicType = HeuristicType::EUCLIDEAN;  // Heuristic to use
//    std::vector<glm::vec3> path;                // The calculated path (waypoints)
//    PathfindingStatus currentStatus = PathfindingStatus::IDLE;  // Current status of pathfinding
//    bool debugEnabled = false;                  // Debug flag for visual feedback
//};
//
//#endif  // PATHFINDING_COMPONENT_HPP

#pragma warning(pop)
