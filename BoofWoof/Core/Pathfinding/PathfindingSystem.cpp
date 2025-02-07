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

PathfindingSystem::PathfindingSystem() {}

void PathfindingSystem::BuildGraph() {
    graphNodes.clear();
    graphEdges.clear();

    //std::cout << "[PathfindingSystem] Building graph...\n";

    // Step 1: Process all NodeComponents first
    for (const auto& entity : g_Coordinator.GetAliveEntitiesSet()) {
        //if (g_Coordinator.HaveComponent<NodeComponent>(entity)) {
        //    auto& nodeComp = g_Coordinator.GetComponent<NodeComponent>(entity);
        //    glm::vec3 nodePosition = nodeComp.GetPosition(); // Default to NodeComponent's position

        //    // Check if the entity has a TransformComponent and use its position
        //    if (g_Coordinator.HaveComponent<TransformComponent>(entity)) {
        //        auto& transformComp = g_Coordinator.GetComponent<TransformComponent>(entity);
        //        nodePosition = transformComp.GetPosition(); // Override with TransformComponent's position
        //    }

        //    // Store the node with its entity reference
        //    graphNodes[entity] = std::make_shared<Node3D>(entity, nodePosition, nodeComp.IsWalkable());

        //    //std::cout << "[PathfindingSystem] Added Node: " << entity
        //    //    << " | Position: (" << nodePosition.x << ", "
        //    //    << nodePosition.y << ", " << nodePosition.z << ")"
        //    //    << " | Walkable: " << nodeComp.IsWalkable() << "\n";
        //}
        if (g_Coordinator.HaveComponent<NodeComponent>(entity)) {
            auto& nodeComp = g_Coordinator.GetComponent<NodeComponent>(entity);
            glm::vec3 nodePosition = nodeComp.GetPosition(); // Default to NodeComponent's position

            // If it has a TransformComponent, use its position
            if (g_Coordinator.HaveComponent<TransformComponent>(entity) && !g_Coordinator.HaveComponent<HierarchyComponent>(entity)) {
                auto& transformComp = g_Coordinator.GetComponent<TransformComponent>(entity);
                nodePosition = transformComp.GetPosition();
            }

            // **Check if entity has a parent (HierarchyComponent)**
            if (g_Coordinator.HaveComponent<HierarchyComponent>(entity)) {
                Entity parent = g_Coordinator.GetComponent<HierarchyComponent>(entity).parent;
                if (parent != MAX_ENTITIES) {
                    // Get world transform from TransformSystem
                    auto transformSystem = g_Coordinator.GetSystem<TransformSystem>();
                    glm::mat4 parentWorldMatrix = transformSystem->GetWorldMatrix(parent);

                    // Apply parent's world position
                    glm::vec3 worldPosition, worldRotation, worldScale;
                    if (DecomposeTransform(parentWorldMatrix, worldPosition, worldRotation, worldScale)) {
                        nodePosition = worldPosition;
                    }
                }
                std::cout << "Node Position (World) x: (" << nodePosition.x << ", " << nodePosition.y << ", " << nodePosition.z << ")" << std::endl;
            }

            // Store the node with the updated position
            graphNodes[entity] = std::make_shared<Node3D>(entity, nodePosition, nodeComp.IsWalkable());
        }
    }

    // Step 2: Process all EdgeComponents after nodes are set up
    for (const auto& entity : g_Coordinator.GetAliveEntitiesSet()) {
        if (g_Coordinator.HaveComponent<EdgeComponent>(entity)) {
            auto& edgeComp = g_Coordinator.GetComponent<EdgeComponent>(entity);

            // Ensure both start and end nodes exist before adding the edge
            if (graphNodes.find(edgeComp.GetStartNode()) == graphNodes.end()) {
                //std::cout << "[PathfindingSystem] WARNING: Start Node " << edgeComp.GetStartNode()
                //    << " referenced by Edge " << entity << " does NOT exist in the graph!\n";
                continue;
            }

            if (graphNodes.find(edgeComp.GetEndNode()) == graphNodes.end()) {
                //std::cout << "[PathfindingSystem] WARNING: End Node " << edgeComp.GetEndNode()
                //    << " referenced by Edge " << entity << " does NOT exist in the graph!\n";
                continue;
            }

            // Store edges
            graphEdges[entity] = std::make_shared<EdgeComponent>(
                edgeComp.GetStartNode(), edgeComp.GetEndNode(), edgeComp.GetCost()
                );

            //std::cout << "[PathfindingSystem] Added Edge: " << entity
            //    << " | Start Node: " << edgeComp.GetStartNode()
            //    << " | End Node: " << edgeComp.GetEndNode()
            //    << " | Cost: " << edgeComp.GetCost() << "\n";
        }
    }

    //std::cout << "[PathfindingSystem] Graph built with " << graphNodes.size()
    //    << " nodes and " << graphEdges.size() << " edges.\n";
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


            Entity startNode = pathfindingComp.GetStartNode();
            Entity goalNode = pathfindingComp.GetGoalNode();

            //std::cout << "[PathfindingSystem] Processing entity " << entity
            //    << " | Start Node: " << startNode
            //    << " | Goal Node: " << goalNode << "\n";

            // Check if start and goal nodes exist in the graph
            if (graphNodes.find(startNode) == graphNodes.end()) {
                //std::cout << "[PathfindingSystem] ERROR: Start node " << startNode << " not found in graph!\n";
                pathfindingComp.SetStatus(PathfindingStatus::FAILED);
                continue;
            }

            if (graphNodes.find(goalNode) == graphNodes.end()) {
                //std::cout << "[PathfindingSystem] ERROR: Goal node " << goalNode << " not found in graph!\n";
                pathfindingComp.SetStatus(PathfindingStatus::FAILED);
                continue;
            }

            std::vector<glm::vec3> path;
            bool success = FindPath(startNode, goalNode, path);

            if (success) {
                pathfindingComp.SetPath(path);
                pathfindingComp.SetStatus(PathfindingStatus::COMPLETE);
                //std::cout << "[PathfindingSystem] Path found for entity " << entity << " | Path Length: " << path.size() << "\n";

                //// Print Full Path
                //for (size_t i = 0; i < path.size(); ++i) {
                //    std::cout << "[PathfindingSystem] Step " << i + 1
                //        << " | Position: (" << path[i].x << ", "
                //        << path[i].y << ", " << path[i].z << ")\n";
                //}
            }
            else {
                pathfindingComp.SetStatus(PathfindingStatus::FAILED);
                //std::cout << "[PathfindingSystem] ERROR: No valid path found for entity " << entity << "!\n";
            }
        }
    }
}

bool PathfindingSystem::FindPath(Entity startNode, Entity goalNode, std::vector<glm::vec3>& outPath) {
    if (graphNodes.find(startNode) == graphNodes.end() || graphNodes.find(goalNode) == graphNodes.end()) {
        //std::cout << "[PathfindingSystem] ERROR: Start or Goal Node not in graph!\n";
        return false;
    }

    open_list = {};
    closed_list.clear();

    auto start = graphNodes[startNode];
    auto goal = graphNodes[goalNode];

    start->givenCost = 0.0f;
    start->finalCost = Heuristic(start->position, goal->position);
    open_list.push(start);

    while (!open_list.empty()) {
        auto currentNode = open_list.top();
        open_list.pop();

        //std::cout << "[PathfindingSystem] Expanding Node: " << currentNode->position.x << ", " << currentNode->position.y << ", " << currentNode->position.z << "\n";

        if (currentNode->position == goal->position) {
            ReconstructPath(currentNode, outPath);
            return true;
        }

        closed_list.insert(currentNode->position);

        for (Entity neighborEntity : GetNeighbors(currentNode->entity)) {
            auto neighbor = graphNodes[neighborEntity];
            if (closed_list.find(neighbor->position) != closed_list.end() || !neighbor->isWalkable) {
                continue;
            }

            // float tentativeGCost = currentNode->givenCost + GetCost(currentNode->position, neighbor->position);
            float edgeCost = GetCost(currentNode->entity, neighbor->entity); // Use correct entity lookup
            float tentativeGCost = currentNode->givenCost + edgeCost;


            if (tentativeGCost < neighbor->givenCost) {
                neighbor->givenCost = tentativeGCost;
                neighbor->finalCost = tentativeGCost + Heuristic(neighbor->position, goal->position);
                neighbor->parent = currentNode;

                open_list.push(neighbor);
            }
        }
    }

    //std::cout << "[PathfindingSystem] ERROR: No path found from Node " << startNode << " to " << goalNode << "!\n";
    return false;
}

std::vector<Entity> PathfindingSystem::GetNeighbors(Entity node) {
    std::vector<Entity> neighbors;

    //std::cout << "[PathfindingSystem] Checking neighbors for Node " << node << ":\n";

    for (const auto& [edgeEntity, edge] : graphEdges) {
        if (edge->GetStartNode() == node) {
            neighbors.push_back(edge->GetEndNode());
            //std::cout << "[PathfindingSystem]   Found neighbor: " << edge->GetEndNode() << " via Edge " << edgeEntity << "\n";
        }
    }

    return neighbors;
}


//float PathfindingSystem::GetCost(const glm::vec3& start, const glm::vec3& end) const {
//    glm::vec3 diff = end - start;
//    return glm::length(diff); // Use Euclidean distance for the cost
//}

float PathfindingSystem::GetCost(Entity start, Entity end) const {
    for (const auto& [edgeEntity, edge] : graphEdges) {
        if (edge->GetStartNode() == start && edge->GetEndNode() == end) {
            return edge->GetCost();  // Return the actual edge cost
        }
    }
    return std::numeric_limits<float>::infinity(); // If no edge exists, return a large value
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