#pragma once
#ifndef HIERARCHY_COMPONENT_HPP
#define HIERARCHY_COMPONENT_HPP

#include "ECS/Coordinator.hpp"
#include "../Core/Reflection/ReflectionManager.hpp"  // Include the reflection system
#include <vector>
#include <algorithm>
#include <sstream>

class HierarchyComponent
{
public:
    // Constructors
    HierarchyComponent() = default;
    explicit HierarchyComponent(Entity parentEntity)
        : parent(parentEntity) {}

    // Parent entity ID
    Entity parent = MAX_ENTITIES; // Assuming MAX_ENTITIES represents an invalid entity

    // Children entity IDs
    std::vector<Entity> children;

    // Methods to manage children
    void AddChild(Entity child)
    {
        // Avoid duplicates
        if (std::find(children.begin(), children.end(), child) == children.end())
        {
            children.push_back(child);
        }
    }

    void RemoveChild(Entity child)
    {
        children.erase(std::remove(children.begin(), children.end(), child), children.end());
    }

    // Reflection integration
    REFLECT_COMPONENT(HierarchyComponent)
    {
        REGISTER_PROPERTY(HierarchyComponent, Parent, std::string, SetParentFromString, GetParentAsString);
        REGISTER_PROPERTY(HierarchyComponent, Children, std::string, SetChildrenFromString, GetChildrenAsString);
    }

    // Setter and Getter for Parent (using string for reflection)
    void SetParentFromString(const std::string& parentStr)
    {
        if (parentStr.empty() || parentStr == "None")
        {
            parent = MAX_ENTITIES;
        }
        else
        {
            parent = static_cast<Entity>(std::stoul(parentStr));
        }
    }

    std::string GetParentAsString() const
    {
        if (parent == MAX_ENTITIES)
        {
            return "None";
        }
        else
        {
            return std::to_string(static_cast<uint32_t>(parent));
        }
    }

    // Setter and Getter for Children (using string for reflection)
    void SetChildrenFromString(const std::string& childrenStr)
    {
        children.clear();
        std::stringstream ss(childrenStr);
        std::string item;
        while (std::getline(ss, item, ','))
        {
            if (!item.empty())
            {
                Entity childEntity = static_cast<Entity>(std::stoul(item));
                children.push_back(childEntity);
            }
        }
    }

    std::string GetChildrenAsString() const
    {
        std::stringstream ss;
        for (size_t i = 0; i < children.size(); ++i)
        {
            ss << static_cast<uint32_t>(children[i]);
            if (i < children.size() - 1)
                ss << ",";
        }
        return ss.str();
    }
};

#endif // HIERARCHY_COMPONENT_HPP
