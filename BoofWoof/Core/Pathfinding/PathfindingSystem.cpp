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

    //std::cout << "[PathfindingSystem] Building graph...\n";
    //std::cout << "[DEBUG] Node Counter BEFORE processing: " << NodeComponent::GetNodeCounter() << "\n";

    // Step 1: Process all NodeComponents first
    for (const auto& entity : g_Coordinator.GetAliveEntitiesSet()) {
        if (g_Coordinator.HaveComponent<NodeComponent>(entity)) {
            auto& nodeComp = g_Coordinator.GetComponent<NodeComponent>(entity);

            if (nodeComp.GetNodeID() == INVALID_ENTITY) {
                nodeComp.SetNodeID(NodeComponent::GenerateNodeID());
                //std::cout << "[DEBUG] Assigned NodeID " << nodeComp.GetNodeID() << " to Entity " << entity << "\n";
				nodelist.push_back(nodeComp.GetNodeID());
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

            //std::cout << "[DEBUG] Adding Node: ID=" << nodeID << " at Position ("
            //    << nodePosition.x << ", " << nodePosition.y << ", " << nodePosition.z
            //    << ") | Walkable: " << nodeComp.IsWalkable() << "\n";

            //graphNodes[nodeID] = std::make_shared<Node3D>(nodeID, nodePosition, nodeComp.IsWalkable());
            graphNodes[nodeID] = std::make_shared<Node3D>(nodeID, entity, nodePosition, nodeComp.IsWalkable());

        }
    }

    // Step 2: Process all EdgeComponents after nodes are set up
    for (const auto& entity : g_Coordinator.GetAliveEntitiesSet()) {
        if (g_Coordinator.HaveComponent<EdgeComponent>(entity)) {
            auto& edgeComp = g_Coordinator.GetComponent<EdgeComponent>(entity);

            uint32_t startNodeID = edgeComp.GetStartNode();
            uint32_t endNodeID = edgeComp.GetEndNode();

            //std::cout << "[DEBUG] Processing Edge: " << entity
            //    << " | Start Node: " << startNodeID
            //    << " | End Node: " << endNodeID << "\n";

            // Ensure both start and end nodes exist before adding the edge
            if (graphNodes.find(startNodeID) == graphNodes.end()) {
                //std::cout << "[PathfindingSystem] WARNING: Start Node " << startNodeID
                //    << " referenced by Edge " << entity << " does NOT exist in the graph!\n";
                continue;
            }

            if (graphNodes.find(endNodeID) == graphNodes.end()) {
                //std::cout << "[PathfindingSystem] WARNING: End Node " << endNodeID
                //    << " referenced by Edge " << entity << " does NOT exist in the graph!\n";
                continue;
            }

            // **Add Forward Edge**
            if (graphEdges.find(edgeIDCounter) == graphEdges.end()) {
                graphEdges[edgeIDCounter++] = std::make_shared<EdgeComponent>(startNodeID, endNodeID, edgeComp.GetCost());

                //std::cout << "[DEBUG] Added Forward Edge: " << edgeIDCounter - 1
                //    << " | Start Node: " << startNodeID
                //    << " | End Node: " << endNodeID
                //    << " | Cost: " << edgeComp.GetCost() << "\n";
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

                //std::cout << "[DEBUG] Added Reverse Edge: " << edgeIDCounter - 1
                //    << " | Start Node: " << endNodeID
                //    << " | End Node: " << startNodeID
                //    << " | Cost: " << edgeComp.GetCost() << "\n";
            }
        }
    }

    // **DEBUG: Check if all nodes have at least one connection**
    std::cout << "\n[PathfindingSystem] Validating node connections...\n";
    std::vector<uint32_t> disconnectedNodes;

    for (const auto& [nodeID, node] : graphNodes) {
        bool hasConnection = false;

        // Check if this node has any outgoing edges
        for (const auto& [edgeID, edge] : graphEdges) {
            if (edge->GetStartNode() == nodeID || edge->GetEndNode() == nodeID) {
                hasConnection = true;
                break;
            }
        }

        if (!hasConnection) {
            disconnectedNodes.push_back(nodeID);
        }
    }

    if (!disconnectedNodes.empty()) {
        std::cout << "[PathfindingSystem] WARNING: The following nodes have NO connections!\n";
        for (uint32_t nodeID : disconnectedNodes) {
            std::cout << "[DEBUG] Isolated Node: " << nodeID << " at Position ("
                << graphNodes[nodeID]->position.x << ", "
                << graphNodes[nodeID]->position.y << ", "
                << graphNodes[nodeID]->position.z << ")\n";
        }
    }
    else {
        std::cout << "[PathfindingSystem] All nodes have valid connections!\n";
    }

    // Debugging: Check all nodes in graphNodes after build
    std::cout << "[PathfindingSystem] Graph built with " << graphNodes.size()
        << " nodes and " << graphEdges.size() << " edges.\n";
    std::cout << "[PathfindingSystem] Validating bidirectional edges...\n";
    for (const auto& [edgeID, edge] : graphEdges) {
        uint32_t start = edge->GetStartNode();
        uint32_t end = edge->GetEndNode();
        bool hasReverse = false;

        for (const auto& [checkID, checkEdge] : graphEdges) {
            if (checkEdge->GetStartNode() == end && checkEdge->GetEndNode() == start) {
                hasReverse = true;
                break;
            }
        }

        if (!hasReverse) {
            std::cout << "[DEBUG] WARNING: One-way edge found! " << start << " -> " << end << "\n";
        }
    }
    std::cout << "[PathfindingSystem] Edge validation complete!\n";

    //// Debugging: Check all nodes in graphNodes after build
    //std::cout << "[DEBUG] Nodes in Graph: ";
    //for (const auto& [id, node] : graphNodes) {
    //    std::cout << id << " ";
    //}
    //std::cout << std::endl;

    //// **Debug: Print all edges created**
    //std::cout << "[DEBUG] Total Edges Created: " << graphEdges.size() << std::endl;
    //for (const auto& [edgeEntity, edge] : graphEdges) {
    //    std::cout << "[DEBUG] Edge " << edgeEntity
    //        << " | " << edge->GetStartNode()
    //        << " -> " << edge->GetEndNode()
    //        << " | Cost: " << edge->GetCost() << std::endl;
    //}

    //std::cout << "[PathfindingSystem] Graph built with " << graphNodes.size()
    //    << " nodes and " << graphEdges.size() << " edges.\n";
    //std::cout << "[DEBUG] Node Counter AFTER processing: " << NodeComponent::GetNodeCounter() << "\n";
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

                // Print Entity Node path
                std::vector<Entity> entityPath;
                if (FindEntityPath(startNode, goalNode, entityPath)) {
                    std::cout << "[PathfindingSystem] Entity path for entity " << entity << ":\n";
                    for (size_t i = 0; i < entityPath.size(); ++i) {
                        std::cout << "[PathfindingSystem] Step " << i + 1
                            << " | Node Entity: " << entityPath[i] << "\n";
                    }
                }
                else {
                    std::cout << "[PathfindingSystem] [WARNING] Failed to retrieve entity path for entity " << entity << "\n";
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

    outPath.clear();
    open_list = {};
    closed_list.clear();

    // **Reset all node costs**
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

        if (currentNode->nodeID == goalNodeID) {
            ReconstructPath(currentNode, outPath);
            return true;
        }

        closed_list.insert(currentNode->position);

        // **FORCE SEARCH THROUGH ALL NODES**
        for (const auto& [nodeID, node] : graphNodes) {
            if (nodeID == currentNode->nodeID) continue;

            float edgeCost = GetCost(currentNode->nodeID, nodeID);
            if (edgeCost == std::numeric_limits<float>::infinity()) continue;  // Skip disconnected nodes

            float tentativeGCost = currentNode->givenCost + edgeCost;

            if (tentativeGCost < node->givenCost) {
                node->givenCost = tentativeGCost;
                node->finalCost = tentativeGCost + Heuristic(node->position, goal->position);
                node->parent = currentNode;
                open_list.push(node);
            }
        }
    }

    std::cout << "[PathfindingSystem] ERROR: No valid path found. Trying nearest node fallback...\n";

    // **FIND NEAREST REACHABLE NODE INSTEAD**
    Entity closestReachableNode = INVALID_ENTITY;
    float closestDistance = std::numeric_limits<float>::infinity();

    for (const auto& [nodeID, node] : graphNodes) {
        if (nodeID == startNodeID) continue;
        float dist = glm::distance(node->position, graphNodes[goalNodeID]->position);
        if (dist < closestDistance) {
            closestDistance = dist;
            closestReachableNode = nodeID;
        }
    }

    if (closestReachableNode != INVALID_ENTITY) {
        std::cout << "[PathfindingSystem] Fallback: Setting new goal to Node " << closestReachableNode << "\n";
        return FindPath(startNodeID, closestReachableNode, outPath);
    }

    return false;
}

bool PathfindingSystem::FindEntityPath(Entity startNode, Entity goalNode, std::vector<Entity>& outEntityPath) {
    std::vector<glm::vec3> tempPath;
    bool success = FindPath(startNode, goalNode, tempPath); // Use existing position-based FindPath()

    if (!success) return false;

    // Reconstruct Entity path manually
    if (graphNodes.find(goalNode) != graphNodes.end()) {
        ReconstructEntityPath(graphNodes[goalNode], outEntityPath);
        return true;
    }

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

    // Debug: Print neighbors of the given node
    std::cout << "[DEBUG] Neighbors of Node " << nodeID << ": ";
    for (auto neighbor : neighbors) {
        std::cout << neighbor << " ";
    }
    std::cout << std::endl;



    return neighbors;
}

float PathfindingSystem::GetCost(uint32_t startNodeID, uint32_t endNodeID) const {
    float cost = std::numeric_limits<float>::infinity();

    for (const auto& [edgeEntity, edge] : graphEdges) {
        if (edge->GetStartNode() == startNodeID && edge->GetEndNode() == endNodeID) {
            return edge->GetCost();  // Return the actual edge cost
        }
    }

    //std::cout << "[DEBUG] Cost from Node " << startNodeID << " to Node " << endNodeID
    //    << " = " << (cost == std::numeric_limits<float>::infinity() ? "INFINITY" : std::to_string(cost))
    //    << std::endl;

    return std::numeric_limits<float>::infinity();
}

float PathfindingSystem::Heuristic(const glm::vec3& a, const glm::vec3& b) const {
    glm::vec3 diff = a - b;
    float length = glm::length(diff);

    //if (std::isinf(length) || std::isnan(length)) {
    //    std::cout << "[DEBUG] Invalid heuristic: " << length << std::endl;
    //}

    return glm::length(diff);
}

//float PathfindingSystem::Heuristic(const glm::vec3& a, const glm::vec3& b) const {
//    return std::abs(a.x - b.x) + std::abs(a.y - b.y) + std::abs(a.z - b.z);
//}

//void PathfindingSystem::ReconstructPath(const std::shared_ptr<Node3D>& goalNode, std::vector<glm::vec3>& outPath) {
//    auto currentNode = goalNode;
//    while (currentNode) {
//        outPath.push_back(currentNode->position);
//        currentNode = currentNode->parent;
//    }
//    std::reverse(outPath.begin(), outPath.end());
//}

void PathfindingSystem::ReconstructPath(const std::shared_ptr<Node3D>& goalNode, std::vector<glm::vec3>& outPath) {
    auto currentNode = goalNode;

    while (currentNode) {
        if (currentNode->position == glm::vec3(0.0f)) {  // Avoid (0,0,0) positions
            std::cout << "[ERROR] Skipping invalid node during path reconstruction!\n";
            currentNode = currentNode->parent;
            continue;
        }

        outPath.push_back(currentNode->position);
        currentNode = currentNode->parent;
    }

    if (outPath.empty()) {
        std::cout << "[ERROR] No valid path reconstructed! Check parent assignments in FindPath().\n";
    }

    std::reverse(outPath.begin(), outPath.end());
}

void PathfindingSystem::ReconstructEntityPath(const std::shared_ptr<Node3D>& goalNode, std::vector<Entity>& outEntityPath) {
    auto currentNode = goalNode;

    while (currentNode) {
        outEntityPath.push_back(currentNode->entityID);
        currentNode = currentNode->parent;
    }

    std::reverse(outEntityPath.begin(), outEntityPath.end());
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

Entity PathfindingSystem::GetClosestNode(const glm::vec3& position) {
	Entity closestNode = INVALID_ENTITY;
	float closestDistance = std::numeric_limits<float>::infinity();

	for (const auto& [nodeID, node] : graphNodes) {
		float distance = glm::length(node->position - position);
		if (distance < closestDistance) {
			closestDistance = distance;
			closestNode = nodeID;
		}
	}

	return closestNode;
}

std::vector<Entity> PathfindingSystem::GetNodeList() {
	return nodelist;
}

void PathfindingSystem::ForceImmediateUpdate() {
    std::cout << "[PathfindingSystem] Forcing immediate pathfinding update.\n";
    Update(0.0f);  // Pass 0.0f to force recalculation immediately
}



#pragma warning(pop)