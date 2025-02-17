///**************************************************************************
// * @file PathfindingSystem.h
// * @author 	Ang Jun Sheng Aloysius
// * @param DP email: a.junshengaloysius@digipen.edu [2201807]
// * @param Course: CS 3401
// * @param Course: Game Project 3
// * @date  11/12/2024 (12 DEC 2024)
// * @brief
// *
// * This file contains the declarations to update the pathfinding coordinates
// *
// *
// *************************************************************************/
//
//#ifndef PATHFINDINGSYSTEM_H
//#define PATHFINDINGSYSTEM_H
//
//#pragma warning(push)
//#pragma warning(disable: 4100 4189 4081 4458)
//
//#include "../ECS/System.hpp"
//#include <vector>
//#include <unordered_map>
//#include <queue>
//#include <memory>
//#include <iostream>
//
// // Enum for node status in lists
//enum class ListStatus3D {
//    NONE, OPEN, CLOSED
//};
//
//// 3D Grid Position Structure
//struct GridPos3D {
//    int x, y, z;
//
//    bool operator==(const GridPos3D& other) const {
//        return x == other.x && y == other.y && z == other.z;
//    }
//};
//
//// Hash function for GridPos3D to be used in unordered containers
//namespace std {
//    template <>
//    struct hash<GridPos3D> {
//        std::size_t operator()(const GridPos3D& k) const {
//            return ((std::hash<int>()(k.x) ^ (std::hash<int>()(k.y) << 1)) >> 1) ^ (std::hash<int>()(k.z) << 1);
//        }
//    };
//}
//
//// Node structure for 3D pathfinding
//struct Node3D {
//    GridPos3D gridPos;          // The grid coordinates of the node
//    float givenCost;            // G cost (cost from start to this node)
//    float finalCost;            // F cost (G + H)
//    std::shared_ptr<Node3D> parent;  // Pointer to the parent node for path reconstruction
//    bool isWalkable;            // Whether this node is walkable
//    ListStatus3D onList;        // Current status (NONE, OPEN, CLOSED)
//
//    // Constructor
//    Node3D(GridPos3D pos = { 0, 0, 0 }, bool walkable = true)
//        : gridPos(pos), givenCost(FLT_MAX), finalCost(FLT_MAX),
//        parent(nullptr), isWalkable(walkable), onList(ListStatus3D::NONE) {}
//};
//
//
//// Comparator for priority queue
//struct NodeComparator3D {
//    bool operator()(const std::shared_ptr<Node3D>& a, const std::shared_ptr<Node3D>& b) const {
//        return a->finalCost > b->finalCost;
//    }
//};
//
//class Grid3D {
//public:
//    Grid3D(int width, int height, int depth)
//        : width(width), height(height), depth(depth) {
//        // Initialize the 3D grid with walkable nodes
//        for (int x = 0; x < width; ++x) {
//            std::vector<std::vector<Node3D>> yzPlane;
//            for (int y = 0; y < height; ++y) {
//                std::vector<Node3D> zLine;
//                for (int z = 0; z < depth; ++z) {
//                    zLine.emplace_back(GridPos3D{ x, y, z }, true); // Assume all nodes are walkable initially
//                }
//                yzPlane.push_back(zLine);
//            }
//            nodes.push_back(yzPlane);
//        }
//    }
//
//    Node3D& GetNode(int x, int y, int z) { return nodes[x][y][z]; }
//
//    bool IsValidPosition(const GridPos3D& pos) const {
//        return pos.x >= 0 && pos.x < width&&
//            pos.y >= 0 && pos.y < height&&
//            pos.z >= 0 && pos.z < depth;
//    }
//
//private:
//    int width, height, depth;
//    std::vector<std::vector<std::vector<Node3D>>> nodes;
//};
//
//class PathfindingSystem : public System
//{
//public:
//    PathfindingSystem();  // Constructor
//
//    void SetupGrid(int width, int height, int depth);
//    bool FindPath(const GridPos3D& start, const GridPos3D& goal, std::vector<GridPos3D>& outPath);
//
//private:
//    int gridWidth, gridHeight, gridDepth;
//
//    // Open and Closed Lists
//    std::priority_queue<std::shared_ptr<Node3D>, std::vector<std::shared_ptr<Node3D>>, NodeComparator3D> open_list;
//    std::unordered_set<GridPos3D> closed_list;
//
//    // All nodes stored for the grid
//    std::unordered_map<GridPos3D, std::shared_ptr<Node3D>> all_nodes;
//
//    // Grid-related functions
//    bool IsValidPosition(const GridPos3D& pos) const;
//    std::vector<Node3D*> GetNeighbors(const Node3D& node);
//
//    // Cost and Heuristic functions
//    float GetCost(const GridPos3D& from, const GridPos3D& to) const;
//    float Heuristic(const GridPos3D& a, const GridPos3D& b) const;
//
//    // Path reconstruction
//    void ReconstructPath(const std::shared_ptr<Node3D>& goalNode, std::vector<GridPos3D>& outPath);
//};
//
//
//
//#endif // PATHFINDINGSYSTEM_H

#ifndef PATHFINDINGSYSTEM_H
#define PATHFINDINGSYSTEM_H

#pragma warning(push)
#pragma warning(disable: 4100 4189 4081 4458)

#include "../ECS/System.hpp"
#include "../Utilities/Components/EdgeComponent.hpp"
#include "../Utilities/Components/NodeComponent.hpp"
#include "../Utilities/Components/PathfindingComponent.hpp"
#include <../Utilities/Components/TransformComponent.hpp>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <memory>
#include <iostream>

enum class ListStatus3D {
    NONE, OPEN, CLOSED
};

namespace std {
    template <>
    struct hash<glm::vec3> {
        std::size_t operator()(const glm::vec3& v) const {
            return std::hash<float>()(v.x) ^ (std::hash<float>()(v.y) << 1) ^ (std::hash<float>()(v.z) << 2);
        }
    };
}

// Node structure for 3D pathfinding
//struct Node3D {
//    glm::vec3 position;         // The world position of the node
//    float givenCost;            // G cost (cost from start to this node)
//    float finalCost;            // F cost (G + H)
//    std::shared_ptr<Node3D> parent;  // Pointer to the parent node for path reconstruction
//    bool isWalkable;            // Whether this node is walkable
//    ListStatus3D onList;        // Current status (NONE, OPEN, CLOSED)
//
//    // Constructor
//    Node3D(glm::vec3 pos = { 0.0f, 0.0f, 0.0f }, bool walkable = true)
//        : position(pos), givenCost(FLT_MAX), finalCost(FLT_MAX),
//        parent(nullptr), isWalkable(walkable), onList(ListStatus3D::NONE) {}
//};

// Node structure for 3D pathfinding
struct Node3D {
    Entity entity;               // Store the entity reference
    glm::vec3 position;
    float givenCost;
    float finalCost;
    std::shared_ptr<Node3D> parent;
    bool isWalkable;

    Node3D(Entity ent, glm::vec3 pos = glm::vec3(0.0f), bool walkable = true)
        : entity(ent), position(pos), givenCost(FLT_MAX), finalCost(FLT_MAX),
        parent(nullptr), isWalkable(walkable) {}
};


// Comparator for priority queue
struct NodeComparator3D {
    bool operator()(const std::shared_ptr<Node3D>& a, const std::shared_ptr<Node3D>& b) const {
        return a->finalCost > b->finalCost;
    }
};

class PathfindingSystem : public System
{
public:
    PathfindingSystem();

    void BuildGraph();
    bool FindPath(Entity startNode, Entity goalNode, std::vector<glm::vec3>& outPath);

    void Update(float deltaTime);

    void ResetPathfinding();

private:
    std::unordered_map<Entity, std::shared_ptr<Node3D>> graphNodes;  // Nodes in the graph
    std::unordered_map<Entity, std::shared_ptr<EdgeComponent>> graphEdges; // Edges in the graph

    std::priority_queue<std::shared_ptr<Node3D>, std::vector<std::shared_ptr<Node3D>>, NodeComparator3D> open_list;
    //std::unordered_set<Entity> closed_list;
    std::unordered_set<glm::vec3> closed_list;

    //float GetCost(const glm::vec3& from, const glm::vec3& to) const;
    float GetCost(Entity start, Entity end) const;
    float Heuristic(const glm::vec3& a, const glm::vec3& b) const;
    void ReconstructPath(const std::shared_ptr<Node3D>& goalNode, std::vector<glm::vec3>& outPath);
    std::vector<Entity> GetNeighbors(Entity node);
};

#endif // PATHFINDINGSYSTEM_H

