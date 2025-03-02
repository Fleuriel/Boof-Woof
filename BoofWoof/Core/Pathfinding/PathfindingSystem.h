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

#ifndef PATHFINDINGSYSTEM_H
#define PATHFINDINGSYSTEM_H

#pragma warning(push)
#pragma warning(disable: 4100 4189 4081 4458)

#include "../ECS/System.hpp"
#include "../Utilities/Components/EdgeComponent.hpp"
#include "../Utilities/Components/NodeComponent.hpp"
#include "../Utilities/Components/PathfindingComponent.hpp"
#include <../Utilities/Components/TransformComponent.hpp>
#include "../Utilities/Components/HierarchyComponent.hpp"
#include "../Core/Graphics/TransformSystem.hpp"
#include <glm/gtx/matrix_decompose.hpp>
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
class Node3D {
public:
    uint32_t nodeID;  // Store Node ID
    glm::vec3 position;
    bool isWalkable;
    float givenCost;
    float finalCost;
    std::shared_ptr<Node3D> parent;

    Node3D(uint32_t id, glm::vec3 pos, bool walkable)
        : nodeID(id), position(pos), isWalkable(walkable), givenCost(FLT_MAX), finalCost(FLT_MAX), parent(nullptr) {}
};


//// Node structure for 3D pathfinding
//struct Node3D {
//    Entity entity;               // Store the entity reference
//    glm::vec3 position;
//    float givenCost;
//    float finalCost;
//    std::shared_ptr<Node3D> parent;
//    bool isWalkable;
//
//    Node3D(Entity ent, glm::vec3 pos = glm::vec3(0.0f), bool walkable = true)
//        : entity(ent), position(pos), givenCost(FLT_MAX), finalCost(FLT_MAX),
//        parent(nullptr), isWalkable(walkable) {}
//};


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

    bool DecomposeTransform(const glm::mat4& transform, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale);

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
    //std::vector<int> GetNeighbors(int nodeID);

    static uint32_t edgeIDCounter; // Unique ID counter for edges
};

#endif // PATHFINDINGSYSTEM_H

