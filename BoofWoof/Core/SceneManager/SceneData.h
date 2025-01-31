#pragma once
#include <rapidjson/document.h>

struct SceneData
{
    std::string sceneGUID;

    struct EntityData
    {
        int oldEntityID = -1;
        rapidjson::Document entityJSON;

        // Delete copy constructor & assignment (since rapidjson::Document is non-copyable)
        EntityData(const EntityData&) = delete;
        EntityData& operator=(const EntityData&) = delete;

        // Default constructor
        EntityData()
        {
            entityJSON.SetObject();
        }

        // Move constructor
        EntityData(EntityData&& other) noexcept
        {
            oldEntityID = other.oldEntityID;
            other.oldEntityID = -1;
            entityJSON.SetObject();
            entityJSON.Swap(other.entityJSON);
        }

        // Move assignment
        EntityData& operator=(EntityData&& other) noexcept
        {
            if (this != &other)
            {
                oldEntityID = other.oldEntityID;
                other.oldEntityID = -1;
                entityJSON.SetObject();
                entityJSON.Swap(other.entityJSON);
            }
            return *this;
        }
    };

    std::vector<EntityData> entityList;

    // If you also need to move the entire SceneData in one go:
    SceneData() = default;
    SceneData(const SceneData&) = delete;               // no copy
    SceneData& operator=(const SceneData&) = delete;    // no copy

    SceneData(SceneData&& other) noexcept
    {
        sceneGUID = std::move(other.sceneGUID);
        entityList = std::move(other.entityList);
        // std::vector will move each EntityData (which we made movable)
    }

    SceneData& operator=(SceneData&& other) noexcept
    {
        if (this != &other)
        {
            sceneGUID = std::move(other.sceneGUID);
            entityList = std::move(other.entityList);
        }
        return *this;
    }
};
