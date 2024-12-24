/**************************************************************************
 * @file PathfindingSystem.h
 * @author 	Ang Jun Sheng Aloysius
 * @param DP email: a.junshengaloysius@digipen.edu [2201807]
 * @param Course: CS 3401
 * @param Course: Game Project 3
 * @date  11/12/2024 (12 DEC 2024)
 * @brief
 *
 * This file contains the declarations to update the pathfinding coordinates
 *
 *
 *************************************************************************/

#ifndef PATHFINDINGSYSTEM_H
#define PATHFINDINGSYSTEM_H

#pragma warning(push)
#pragma warning(disable: 4100 4189 4081 4458)

#include "../ECS/System.hpp"
#include <vector>
#include <unordered_map>
#include <queue>
#include <memory>
#include <iostream>

 // Enum for node status in lists
enum class ListStatus3D {
    NONE, OPEN, CLOSED
};

// 3D Grid Position Structure
struct GridPos3D {
    int x, y, z;

    bool operator==(const GridPos3D& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
};

// Hash function for GridPos3D to be used in unordered containers
namespace std {
    template <>
    struct hash<GridPos3D> {
        std::size_t operator()(const GridPos3D& k) const {
            return ((std::hash<int>()(k.x) ^ (std::hash<int>()(k.y) << 1)) >> 1) ^ (std::hash<int>()(k.z) << 1);
        }
    };
}

// Node structure for 3D pathfinding
struct Node3D {
    GridPos3D gridPos;          // The grid coordinates of the node
    float givenCost;            // G cost (cost from start to this node)
    float finalCost;            // F cost (G + H)
    std::shared_ptr<Node3D> parent;  // Pointer to the parent node for path reconstruction
    bool isWalkable;            // Whether this node is walkable
    ListStatus3D onList;        // Current status (NONE, OPEN, CLOSED)

    // Constructor
    Node3D(GridPos3D pos = { 0, 0, 0 }, bool walkable = true)
        : gridPos(pos), givenCost(FLT_MAX), finalCost(FLT_MAX),
        parent(nullptr), isWalkable(walkable), onList(ListStatus3D::NONE) {}
};


// Comparator for priority queue
struct NodeComparator3D {
    bool operator()(const std::shared_ptr<Node3D>& a, const std::shared_ptr<Node3D>& b) const {
        return a->finalCost > b->finalCost;
    }
};

class Grid3D {
public:
    Grid3D(int width, int height, int depth)
        : width(width), height(height), depth(depth) {
        // Initialize the 3D grid with walkable nodes
        for (int x = 0; x < width; ++x) {
            std::vector<std::vector<Node3D>> yzPlane;
            for (int y = 0; y < height; ++y) {
                std::vector<Node3D> zLine;
                for (int z = 0; z < depth; ++z) {
                    zLine.emplace_back(GridPos3D{ x, y, z }, true); // Assume all nodes are walkable initially
                }
                yzPlane.push_back(zLine);
            }
            nodes.push_back(yzPlane);
        }
    }

    Node3D& GetNode(int x, int y, int z) { return nodes[x][y][z]; }

    bool IsValidPosition(const GridPos3D& pos) const {
        return pos.x >= 0 && pos.x < width&&
            pos.y >= 0 && pos.y < height&&
            pos.z >= 0 && pos.z < depth;
    }

private:
    int width, height, depth;
    std::vector<std::vector<std::vector<Node3D>>> nodes;
};

class PathfindingSystem : public System
{
public:
    PathfindingSystem();  // Constructor

    void SetupGrid(int width, int height, int depth);
    bool FindPath(const GridPos3D& start, const GridPos3D& goal, std::vector<GridPos3D>& outPath);

private:
    int gridWidth, gridHeight, gridDepth;

    // Open and Closed Lists
    std::priority_queue<std::shared_ptr<Node3D>, std::vector<std::shared_ptr<Node3D>>, NodeComparator3D> open_list;
    std::unordered_set<GridPos3D> closed_list;

    // All nodes stored for the grid
    std::unordered_map<GridPos3D, std::shared_ptr<Node3D>> all_nodes;

    // Grid-related functions
    bool IsValidPosition(const GridPos3D& pos) const;
    std::vector<Node3D*> GetNeighbors(const Node3D& node);

    // Cost and Heuristic functions
    float GetCost(const GridPos3D& from, const GridPos3D& to) const;
    float Heuristic(const GridPos3D& a, const GridPos3D& b) const;

    // Path reconstruction
    void ReconstructPath(const std::shared_ptr<Node3D>& goalNode, std::vector<GridPos3D>& outPath);
};

//// Class to interface with JoltPhysics
//class PathfindingSystem : public System {
//public:
//
//    PathfindingSystem() : grid(0, 0, 0) {} // Default dimensions
//
//    /**************************************************************************/
//    /*!
//    \brief Updates the Pathfinding coordinates.
//    */
//    /**************************************************************************/
//    void OnUpdate();
//
//    void Cleanup();
//
//    // Core pathfinding function
//    bool FindPath(const GridPos3D& start, const GridPos3D& goal, std::vector<GridPos3D>& outPath);
//
//    void SetupGrid(int width, int height, int depth);
//
//private:
//    Grid3D grid; // Declare the grid as a member variable
//    std::vector<Node3D*> GetNeighbors(const Node3D& node);
//
//    // List of nodes
//    std::priority_queue<std::shared_ptr<Node3D>, std::vector<std::shared_ptr<Node3D>>, NodeComparator3D> openList;
//    std::unordered_set<GridPos3D> closedList;
//
//    float GetCost(const GridPos3D& from, const GridPos3D& to);
//    float Heuristic(const GridPos3D& a, const GridPos3D& b);
//};



#endif // PATHFINDINGSYSTEM_H
