/**************************************************************************
 * @file PathfindingSystem.cpp
 * @author 	Ang Jun Sheng Aloysius
 * @param DP email: a.junshengaloysius@digipen.edu [2201807]
 * @param Course: CS 3401
 * @param Course: Game Project 3
 * @date  11/12/2024 (11 DEC 2024)
 * @brief
 *
 * This file contains the definitions to update the pathfinding coordinates
 *
 *
 *************************************************************************/
#include "pch.h"
#include "PathfindingSystem.h"
#include <cmath>
#include <queue>
#include <unordered_set>
#include <functional>

#pragma warning(push)
#pragma warning(disable: 4505 4458)

uint32_t PathfindingSystem::edgeIDCounter = 1; // Start from 1 to avoid zero conflicts

PathfindingSystem::PathfindingSystem() {}

void PathfindingSystem::BuildGraph() {
    graphNodes.clear();
    graphEdges.clear();

    NodeComponent::ResetNodeCounter();

    std::cout << "[PathfindingSystem] Building graph...\n";
    std::cout << "[DEBUG] Node Counter BEFORE processing: " << NodeComponent::GetNodeCounter() << "\n";

    // Step 1: Process all NodeComponents first
    for (const auto& entity : g_Coordinator.GetAliveEntitiesSet()) {
        if (g_Coordinator.HaveComponent<NodeComponent>(entity)) {
            auto& nodeComp = g_Coordinator.GetComponent<NodeComponent>(entity);

            if (nodeComp.GetNodeID() == INVALID_ENTITY) {
                nodeComp.SetNodeID(NodeComponent::GenerateNodeID());
                std::cout << "[DEBUG] Assigned NodeID " << nodeComp.GetNodeID() << " to Entity " << entity << "\n";
            }

            glm::vec3 nodePosition = nodeComp.GetPosition();

            if (g_Coordinator.HaveComponent<TransformComponent>(entity) &&
                !g_Coordinator.HaveComponent<HierarchyComponent>(entity)) {
                auto& transformComp = g_Coordinator.GetComponent<TransformComponent>(entity);
                nodePosition = transformComp.GetPosition();
            }

            if (g_Coordinator.HaveComponent<HierarchyComponent>(entity)) {
                Entity parent = g_Coordinator.GetComponent<HierarchyComponent>(entity).parent;
                if (parent != MAX_ENTITIES) {
                    auto transformSystem = g_Coordinator.GetSystem<TransformSystem>();
                    glm::mat4 parentWorldMatrix = transformSystem->GetWorldMatrix(parent);

                    glm::vec3 worldPosition, worldRotation, worldScale;
                    if (DecomposeTransform(parentWorldMatrix, worldPosition, worldRotation, worldScale)) {
                        nodePosition = worldPosition;
                    }
                }
            }

            uint32_t nodeID = nodeComp.GetNodeID();

            std::cout << "[DEBUG] Adding Node: ID=" << nodeID << " at Position ("
                << nodePosition.x << ", " << nodePosition.y << ", " << nodePosition.z
                << ") | Walkable: " << nodeComp.IsWalkable() << "\n";

            graphNodes[nodeID] = std::make_shared<Node3D>(nodeID, nodePosition, nodeComp.IsWalkable());
        }
    }

    // Step 2: Process all EdgeComponents after nodes are set up
    for (const auto& entity : g_Coordinator.GetAliveEntitiesSet()) {
        if (g_Coordinator.HaveComponent<EdgeComponent>(entity)) {
            auto& edgeComp = g_Coordinator.GetComponent<EdgeComponent>(entity);

            uint32_t startNodeID = edgeComp.GetStartNode();
            uint32_t endNodeID = edgeComp.GetEndNode();

            std::cout << "[DEBUG] Processing Edge: " << entity
                << " | Start Node: " << startNodeID
                << " | End Node: " << endNodeID << "\n";

            // Ensure both start and end nodes exist before adding the edge
            if (graphNodes.find(startNodeID) == graphNodes.end()) {
                std::cout << "[PathfindingSystem] WARNING: Start Node " << startNodeID
                    << " referenced by Edge " << entity << " does NOT exist in the graph!\n";
                continue;
            }

            if (graphNodes.find(endNodeID) == graphNodes.end()) {
                std::cout << "[PathfindingSystem] WARNING: End Node " << endNodeID
                    << " referenced by Edge " << entity << " does NOT exist in the graph!\n";
                continue;
            }

            // **Add Forward Edge**
            if (graphEdges.find(edgeIDCounter) == graphEdges.end()) {
                graphEdges[edgeIDCounter++] = std::make_shared<EdgeComponent>(startNodeID, endNodeID, edgeComp.GetCost());

                std::cout << "[DEBUG] Added Forward Edge: " << edgeIDCounter - 1
                    << " | Start Node: " << startNodeID
                    << " | End Node: " << endNodeID
                    << " | Cost: " << edgeComp.GetCost() << "\n";
            }

            // **Add Reverse Edge ONLY IF it doesn't already exist**
            bool reverseEdgeExists = false;
            for (const auto& [edgeEntity, edge] : graphEdges) {
                if (edge->GetStartNode() == endNodeID && edge->GetEndNode() == startNodeID) {
                    reverseEdgeExists = true;
                    break;
                }
            }

            if (!reverseEdgeExists) {
                graphEdges[edgeIDCounter++] = std::make_shared<EdgeComponent>(endNodeID, startNodeID, edgeComp.GetCost());

                std::cout << "[DEBUG] Added Reverse Edge: " << edgeIDCounter - 1
                    << " | Start Node: " << endNodeID
                    << " | End Node: " << startNodeID
                    << " | Cost: " << edgeComp.GetCost() << "\n";
            }
        }
    }



    std::cout << "[PathfindingSystem] Graph built with " << graphNodes.size()
        << " nodes and " << graphEdges.size() << " edges.\n";
    std::cout << "[DEBUG] Node Counter AFTER processing: " << NodeComponent::GetNodeCounter() << "\n";
}

void PathfindingSystem::Update(float deltaTime) {

    for (const auto& entity : g_Coordinator.GetAliveEntitiesSet()) {
        if (!g_Coordinator.HaveComponent<PathfindingComponent>(entity))
            continue;

        auto& pathfindingComp = g_Coordinator.GetComponent<PathfindingComponent>(entity);

        // Ensure the graph is built before processing entities
        if (!pathfindingComp.GetBuilt()) {
            BuildGraph();
            pathfindingComp.SetBuilt(true);

            std::vector<glm::vec3> path;

            // Clear previous path before attempting a new one
            pathfindingComp.SetPath(std::vector<glm::vec3>()); // Reset path

            Entity startNode = pathfindingComp.GetStartNode();
            Entity goalNode = pathfindingComp.GetGoalNode();

            std::cout << "[PathfindingSystem] Processing entity " << entity
                << " | Start Node: " << startNode
                << " | Goal Node: " << goalNode << "\n";

            // Check if start and goal nodes exist in the graph
            if (graphNodes.find(startNode) == graphNodes.end()) {
                std::cout << "[PathfindingSystem] ERROR: Start node " << startNode << " not found in graph!\n";
                pathfindingComp.SetStatus(PathfindingStatus::FAILED);
                continue;
            }

            if (graphNodes.find(goalNode) == graphNodes.end()) {
                std::cout << "[PathfindingSystem] ERROR: Goal node " << goalNode << " not found in graph!\n";
                pathfindingComp.SetStatus(PathfindingStatus::FAILED);
                continue;
            }

            bool success = FindPath(startNode, goalNode, path);

            if (success) {
                pathfindingComp.SetPath(path);
                pathfindingComp.SetStatus(PathfindingStatus::COMPLETE);
                std::cout << "[PathfindingSystem] Path found for entity " << entity << " | Path Length: " << path.size() << "\n";

                // Print Full Path
                for (size_t i = 0; i < path.size(); ++i) {
                    std::cout << "[PathfindingSystem] Step " << i + 1
                        << " | Position: (" << path[i].x << ", "
                        << path[i].y << ", " << path[i].z << ")\n";
                }
            }
            if (!success) {
                pathfindingComp.SetStatus(PathfindingStatus::FAILED);
                std::cout << "[PathfindingSystem] ERROR: No valid path found for entity " << entity << "!\n";
            }
        }
    }
}

bool PathfindingSystem::FindPath(uint32_t startNodeID, uint32_t goalNodeID, std::vector<glm::vec3>& outPath) {
    if (graphNodes.find(startNodeID) == graphNodes.end() || graphNodes.find(goalNodeID) == graphNodes.end()) {
        std::cout << "[PathfindingSystem] ERROR: Start or Goal Node not in graph!\n";
        return false;
    }

    // Reset path to ensure no stale data
    outPath.clear();

    open_list = {};
    closed_list.clear();

    // **Reset node costs**
    for (auto& [id, node] : graphNodes) {
        node->givenCost = std::numeric_limits<float>::infinity();
        node->finalCost = std::numeric_limits<float>::infinity();
        node->parent = nullptr;
    }

    auto start = graphNodes[startNodeID];
    auto goal = graphNodes[goalNodeID];

    start->givenCost = 0.0f;
    start->finalCost = Heuristic(start->position, goal->position);
    open_list.push(start);

    while (!open_list.empty()) {
        auto currentNode = open_list.top();
        open_list.pop();

        if (currentNode->position == goal->position) {
            ReconstructPath(currentNode, outPath);
            return true;
        }

        closed_list.insert(currentNode->position);

        for (uint32_t neighborNodeID : GetNeighbors(currentNode->nodeID)) {
            auto neighbor = graphNodes[neighborNodeID];
            if (closed_list.find(neighbor->position) != closed_list.end() || !neighbor->isWalkable) {
                continue;
            }

            float edgeCost = GetCost(currentNode->nodeID, neighbor->nodeID);
            float tentativeGCost = currentNode->givenCost + edgeCost;

            if (tentativeGCost < neighbor->givenCost) {
                neighbor->givenCost = tentativeGCost;
                neighbor->finalCost = tentativeGCost + Heuristic(neighbor->position, goal->position);
                neighbor->parent = currentNode;

                open_list.push(neighbor);
            }
        }
    }

    std::cout << "[PathfindingSystem] ERROR: No path found from Node " << startNodeID << " to " << goalNodeID << "!\n";
    return false;
}


std::vector<uint32_t> PathfindingSystem::GetNeighbors(uint32_t nodeID) {
    std::vector<uint32_t> neighbors;

    for (const auto& [edgeEntity, edge] : graphEdges) {
        if (edge->GetStartNode() == nodeID) {
            neighbors.push_back(edge->GetEndNode());
        }
        else if (edge->GetEndNode() == nodeID) {
            // Add this to ensure the graph works in BOTH directions
            neighbors.push_back(edge->GetStartNode());
        }
    }

    return neighbors;
}

float PathfindingSystem::GetCost(uint32_t startNodeID, uint32_t endNodeID) const {
    for (const auto& [edgeEntity, edge] : graphEdges) {
        if (edge->GetStartNode() == startNodeID && edge->GetEndNode() == endNodeID) {
            return edge->GetCost();  // Return the actual edge cost
        }
    }
    return std::numeric_limits<float>::infinity();
}

float PathfindingSystem::Heuristic(const glm::vec3& a, const glm::vec3& b) const {
    glm::vec3 diff = a - b;
    return glm::length(diff);
}

void PathfindingSystem::ReconstructPath(const std::shared_ptr<Node3D>& goalNode, std::vector<glm::vec3>& outPath) {
    auto currentNode = goalNode;
    while (currentNode) {
        outPath.push_back(currentNode->position);
        currentNode = currentNode->parent;
    }
    std::reverse(outPath.begin(), outPath.end());
}

void PathfindingSystem::ResetPathfinding() {

    for (const auto& entity : g_Coordinator.GetAliveEntitiesSet()) {
        if (!g_Coordinator.HaveComponent<PathfindingComponent>(entity))
            continue;
        auto& pathfindingComp = g_Coordinator.GetComponent<PathfindingComponent>(entity);
        pathfindingComp.SetBuilt(false);
    }

}

bool PathfindingSystem::DecomposeTransform(const glm::mat4& transform, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale)
{
    using namespace glm;

    vec3 skew;
    vec4 perspective;
    quat orientation;

    if (!glm::decompose(transform, scale, orientation, position, skew, perspective))
    {
        return false;
    }

    // Correct the signs of the scale components if necessary
    if (determinant(transform) < 0)
    {
        scale = -scale;
    }

    // Convert quaternion to Euler angles (in radians)
    rotation = eulerAngles(orientation);

    return true;
}





#pragma warning(pop)