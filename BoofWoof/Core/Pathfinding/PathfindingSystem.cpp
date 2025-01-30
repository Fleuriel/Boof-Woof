/**************************************************************************
 * @file PhysicsSystem.cpp
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
        if (g_Coordinator.HaveComponent<NodeComponent>(entity)) {
            auto& nodeComp = g_Coordinator.GetComponent<NodeComponent>(entity);
            glm::vec3 nodePosition = nodeComp.GetPosition(); // Default to NodeComponent's position

            // Check if the entity has a TransformComponent and use its position
            if (g_Coordinator.HaveComponent<TransformComponent>(entity)) {
                auto& transformComp = g_Coordinator.GetComponent<TransformComponent>(entity);
                nodePosition = transformComp.GetPosition(); // Override with TransformComponent's position
            }

            // Store the node with its entity reference
            graphNodes[entity] = std::make_shared<Node3D>(entity, nodePosition, nodeComp.IsWalkable());

            //std::cout << "[PathfindingSystem] Added Node: " << entity
            //    << " | Position: (" << nodePosition.x << ", "
            //    << nodePosition.y << ", " << nodePosition.z << ")"
            //    << " | Walkable: " << nodeComp.IsWalkable() << "\n";
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

    // Ensure the graph is built before processing entities
    BuildGraph();

    for (const auto& entity : g_Coordinator.GetAliveEntitiesSet()) {
        if (!g_Coordinator.HaveComponent<PathfindingComponent>(entity))
            continue;

        auto& pathfindingComp = g_Coordinator.GetComponent<PathfindingComponent>(entity);

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

            float tentativeGCost = currentNode->givenCost + GetCost(currentNode->position, neighbor->position);

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

//bool PathfindingSystem::FindPath(Entity startNode, Entity goalNode, std::vector<glm::vec3>& outPath) {
//    if (graphNodes.find(startNode) == graphNodes.end() || graphNodes.find(goalNode) == graphNodes.end()) {
//        std::cout << "Invalid start or goal node.\n";
//        return false;
//    }
//
//    open_list = {};
//    closed_list.clear();
//
//    auto start = graphNodes[startNode];
//    auto goal = graphNodes[goalNode];
//
//    start->givenCost = 0.0f;
//    start->finalCost = Heuristic(start->position, goal->position);
//    open_list.push(start);
//
//    while (!open_list.empty()) {
//        auto currentNode = open_list.top();
//        open_list.pop();
//
//        if (currentNode->position == goal->position) {
//            ReconstructPath(currentNode, outPath);
//            return true;
//        }
//
//        closed_list.insert(currentNode->position);
//
//        for (Entity neighborEntity : GetNeighbors(startNode)) {
//            auto neighbor = graphNodes[neighborEntity];
//            if (closed_list.find(neighbor->position) != closed_list.end() || !neighbor->isWalkable) {
//                continue;
//            }
//
//            float tentativeGCost = currentNode->givenCost + GetCost(currentNode->position, neighbor->position);
//
//            if (tentativeGCost < neighbor->givenCost) {
//                neighbor->givenCost = tentativeGCost;
//                neighbor->finalCost = tentativeGCost + Heuristic(neighbor->position, goal->position);
//                neighbor->parent = currentNode;
//
//                open_list.push(neighbor);
//            }
//        }
//    }
//
//    return false;
//}

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


float PathfindingSystem::GetCost(const glm::vec3& start, const glm::vec3& end) const {
    glm::vec3 diff = end - start;
    return glm::length(diff); // Use Euclidean distance for the cost
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
    std::cout << "[PathfindingSystem] Resetting pathfinding system..." << std::endl;

    graphNodes.clear();
    graphEdges.clear();

    std::cout << "[PathfindingSystem] Graph cleared. Rebuilding..." << std::endl;
    BuildGraph(); // Rebuild the graph with fresh entities

    std::cout << "[PathfindingSystem] Pathfinding system reset successfully!" << std::endl;
}


//PathfindingSystem::PathfindingSystem() : gridWidth(0), gridHeight(0), gridDepth(0) {
//    // Default initialization for grid dimensions.
//}
//
//void PathfindingSystem::SetupGrid(int width, int height, int depth) {
//    gridWidth = width;
//    gridHeight = height;
//    gridDepth = depth;
//
//    // Clear existing data
//    all_nodes.clear();
//
//    // Initialize all nodes in the 3D grid
//    for (int x = 0; x < gridWidth; ++x) {
//        for (int y = 0; y < gridHeight; ++y) {
//            for (int z = 0; z < gridDepth; ++z) {
//                GridPos3D pos = { x, y, z };
//                all_nodes[pos] = std::make_shared<Node3D>(pos);
//            }
//        }
//    }
//
//    std::cout << "3D grid setup complete: " << width << "x" << height << "x" << depth << std::endl;
//}
//
//
//bool PathfindingSystem::FindPath(const GridPos3D& start, const GridPos3D& goal, std::vector<GridPos3D>& outPath) {
//    // Clear lists for a new pathfinding request
//    open_list = {};
//    closed_list.clear();
//
//    if (!IsValidPosition(start) || !IsValidPosition(goal)) {
//        std::cout << "Invalid start or goal position." << std::endl;
//        return false;
//    }
//
//    auto startNode = all_nodes[start];
//    auto goalNode = all_nodes[goal];
//
//    startNode->givenCost = 0.0f;
//    startNode->finalCost = Heuristic(start, goal);
//    open_list.push(startNode);
//
//    while (!open_list.empty()) {
//        auto currentNode = open_list.top();
//        open_list.pop();
//
//        // If we reach the goal, reconstruct the path
//        if (currentNode->gridPos == goal) {
//            ReconstructPath(currentNode, outPath);
//            return true;
//        }
//
//        closed_list.insert(currentNode->gridPos);
//
//        for (Node3D* neighbor : GetNeighbors(*currentNode)) {
//            if (closed_list.find(neighbor->gridPos) != closed_list.end()) continue;
//
//            float tentativeGCost = currentNode->givenCost + GetCost(currentNode->gridPos, neighbor->gridPos);
//
//            if (tentativeGCost < neighbor->givenCost) {
//                neighbor->givenCost = tentativeGCost;
//                neighbor->finalCost = tentativeGCost + Heuristic(neighbor->gridPos, goal);
//                neighbor->parent = currentNode;
//
//                if (neighbor->onList != ListStatus3D::OPEN) {
//                    neighbor->onList = ListStatus3D::OPEN;
//                    open_list.push(all_nodes[neighbor->gridPos]);
//                }
//            }
//        }
//    }
//
//    return false;  // No path found
//}
//
//
//std::vector<Node3D*> PathfindingSystem::GetNeighbors(const Node3D& node) {
//    std::vector<Node3D*> neighbors;
//
//    // Define orthogonal and diagonal moves
//    std::vector<GridPos3D> moves = {
//        {1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}, // Orthogonal
//        {1, 1, 0}, {1, -1, 0}, {-1, 1, 0}, {-1, -1, 0},                    // Diagonal in XY
//        {1, 0, 1}, {-1, 0, 1}, {1, 0, -1}, {-1, 0, -1},                    // Diagonal in XZ
//        {0, 1, 1}, {0, -1, 1}, {0, 1, -1}, {0, -1, -1},                    // Diagonal in YZ
//        {1, 1, 1}, {1, 1, -1}, {1, -1, 1}, {1, -1, -1},                    // Diagonal in 3D
//        {-1, 1, 1}, {-1, 1, -1}, {-1, -1, 1}, {-1, -1, -1}                 // Diagonal in 3D
//    };
//
//    for (const auto& move : moves) {
//        GridPos3D neighborPos = {
//            node.gridPos.x + move.x,
//            node.gridPos.y + move.y,
//            node.gridPos.z + move.z
//        };
//
//        if (IsValidPosition(neighborPos) && all_nodes[neighborPos]->isWalkable) {
//            neighbors.push_back(all_nodes[neighborPos].get());
//        }
//    }
//
//    return neighbors;
//}
//
//void PathfindingSystem::ReconstructPath(const std::shared_ptr<Node3D>& goalNode, std::vector<GridPos3D>& outPath) {
//    auto currentNode = goalNode;
//    while (currentNode) {
//        outPath.push_back(currentNode->gridPos);
//        currentNode = currentNode->parent;
//    }
//    std::reverse(outPath.begin(), outPath.end());
//}
//
//float PathfindingSystem::GetCost(const GridPos3D& from, const GridPos3D& to) const {
//    int dx = std::abs(from.x - to.x);
//    int dy = std::abs(from.y - to.y);
//    int dz = std::abs(from.z - to.z);
//
//    // Straight = 1, Diagonal = sqrt(2), 3D Diagonal = sqrt(3)
//    if (dx + dy + dz == 1) return 1.0f;       // Straight
//    if (dx + dy + dz == 2) return 1.414f;     // Diagonal
//    return 1.732f;                            // 3D Diagonal
//}
//
//
//float PathfindingSystem::Heuristic(const GridPos3D& a, const GridPos3D& b) const {
//    float dx = static_cast<float>(std::abs(a.x - b.x));
//    float dy = static_cast<float>(std::abs(a.y - b.y));
//    float dz = static_cast<float>(std::abs(a.z - b.z));
//    return std::sqrt(dx * dx + dy * dy + dz * dz);  // Euclidean distance
//}
//
//bool PathfindingSystem::IsValidPosition(const GridPos3D& pos) const {
//    return pos.x >= 0 && pos.x < gridWidth&&
//        pos.y >= 0 && pos.y < gridHeight&&
//        pos.z >= 0 && pos.z < gridDepth;
//}





#pragma warning(pop)