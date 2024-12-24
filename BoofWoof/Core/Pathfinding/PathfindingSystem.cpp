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


PathfindingSystem::PathfindingSystem() : gridWidth(0), gridHeight(0), gridDepth(0) {
    // Default initialization for grid dimensions.
}

void PathfindingSystem::SetupGrid(int width, int height, int depth) {
    gridWidth = width;
    gridHeight = height;
    gridDepth = depth;

    // Clear existing data
    all_nodes.clear();

    // Initialize all nodes in the 3D grid
    for (int x = 0; x < gridWidth; ++x) {
        for (int y = 0; y < gridHeight; ++y) {
            for (int z = 0; z < gridDepth; ++z) {
                GridPos3D pos = { x, y, z };
                all_nodes[pos] = std::make_shared<Node3D>(pos);
            }
        }
    }

    std::cout << "3D grid setup complete: " << width << "x" << height << "x" << depth << std::endl;
}


bool PathfindingSystem::FindPath(const GridPos3D& start, const GridPos3D& goal, std::vector<GridPos3D>& outPath) {
    // Clear lists for a new pathfinding request
    open_list = {};
    closed_list.clear();

    if (!IsValidPosition(start) || !IsValidPosition(goal)) {
        std::cout << "Invalid start or goal position." << std::endl;
        return false;
    }

    auto startNode = all_nodes[start];
    auto goalNode = all_nodes[goal];

    startNode->givenCost = 0.0f;
    startNode->finalCost = Heuristic(start, goal);
    open_list.push(startNode);

    while (!open_list.empty()) {
        auto currentNode = open_list.top();
        open_list.pop();

        // If we reach the goal, reconstruct the path
        if (currentNode->gridPos == goal) {
            ReconstructPath(currentNode, outPath);
            return true;
        }

        closed_list.insert(currentNode->gridPos);

        for (Node3D* neighbor : GetNeighbors(*currentNode)) {
            if (closed_list.find(neighbor->gridPos) != closed_list.end()) continue;

            float tentativeGCost = currentNode->givenCost + GetCost(currentNode->gridPos, neighbor->gridPos);

            if (tentativeGCost < neighbor->givenCost) {
                neighbor->givenCost = tentativeGCost;
                neighbor->finalCost = tentativeGCost + Heuristic(neighbor->gridPos, goal);
                neighbor->parent = currentNode;

                if (neighbor->onList != ListStatus3D::OPEN) {
                    neighbor->onList = ListStatus3D::OPEN;
                    open_list.push(all_nodes[neighbor->gridPos]);
                }
            }
        }
    }

    return false;  // No path found
}


std::vector<Node3D*> PathfindingSystem::GetNeighbors(const Node3D& node) {
    std::vector<Node3D*> neighbors;

    // Define orthogonal and diagonal moves
    std::vector<GridPos3D> moves = {
        {1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}, // Orthogonal
        {1, 1, 0}, {1, -1, 0}, {-1, 1, 0}, {-1, -1, 0},                    // Diagonal in XY
        {1, 0, 1}, {-1, 0, 1}, {1, 0, -1}, {-1, 0, -1},                    // Diagonal in XZ
        {0, 1, 1}, {0, -1, 1}, {0, 1, -1}, {0, -1, -1},                    // Diagonal in YZ
        {1, 1, 1}, {1, 1, -1}, {1, -1, 1}, {1, -1, -1},                    // Diagonal in 3D
        {-1, 1, 1}, {-1, 1, -1}, {-1, -1, 1}, {-1, -1, -1}                 // Diagonal in 3D
    };

    for (const auto& move : moves) {
        GridPos3D neighborPos = {
            node.gridPos.x + move.x,
            node.gridPos.y + move.y,
            node.gridPos.z + move.z
        };

        if (IsValidPosition(neighborPos) && all_nodes[neighborPos]->isWalkable) {
            neighbors.push_back(all_nodes[neighborPos].get());
        }
    }

    return neighbors;
}

void PathfindingSystem::ReconstructPath(const std::shared_ptr<Node3D>& goalNode, std::vector<GridPos3D>& outPath) {
    auto currentNode = goalNode;
    while (currentNode) {
        outPath.push_back(currentNode->gridPos);
        currentNode = currentNode->parent;
    }
    std::reverse(outPath.begin(), outPath.end());
}

float PathfindingSystem::GetCost(const GridPos3D& from, const GridPos3D& to) const {
    int dx = std::abs(from.x - to.x);
    int dy = std::abs(from.y - to.y);
    int dz = std::abs(from.z - to.z);

    // Straight = 1, Diagonal = sqrt(2), 3D Diagonal = sqrt(3)
    if (dx + dy + dz == 1) return 1.0f;       // Straight
    if (dx + dy + dz == 2) return 1.414f;     // Diagonal
    return 1.732f;                            // 3D Diagonal
}


float PathfindingSystem::Heuristic(const GridPos3D& a, const GridPos3D& b) const {
    float dx = static_cast<float>(std::abs(a.x - b.x));
    float dy = static_cast<float>(std::abs(a.y - b.y));
    float dz = static_cast<float>(std::abs(a.z - b.z));
    return std::sqrt(dx * dx + dy * dy + dz * dz);  // Euclidean distance
}

bool PathfindingSystem::IsValidPosition(const GridPos3D& pos) const {
    return pos.x >= 0 && pos.x < gridWidth&&
        pos.y >= 0 && pos.y < gridHeight&&
        pos.z >= 0 && pos.z < gridDepth;
}





#pragma warning(pop)