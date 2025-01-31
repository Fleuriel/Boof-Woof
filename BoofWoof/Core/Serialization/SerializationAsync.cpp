#include "pch.h"               
#include "SerializationAsync.h"
#include "Serialization.h"      // Contains your existing SaveScene/LoadScene logic
#include "EngineCore.h"         // For g_Coordinator, if needed
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <filesystem>
#include <vector>
#include <cstdio>

void SerializationAsync::BeginLoad(const std::string& filepath)
{
    // If a load is already in progress, handle accordingly
    m_Loading = true;
    m_Done = false;

    // Launch background thread
    m_Thread = std::thread([this, filepath]() {
        ThreadLoadFunction(filepath);
        });

    // Optionally detach or keep joinable
    m_Thread.detach();
}

bool SerializationAsync::IsDone() const
{
    return m_Done;
}

SceneData SerializationAsync::GetLoadedData()
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    return std::move(m_SceneData);
}


void SerializationAsync::ThreadLoadFunction(const std::string& filepath)
{
    SceneData tempData;

    // --- Below is a simplified version of your "LoadScene" function,
    //     but we only parse JSON, we do NOT create ECS entities. ---

    FILE* fp = fopen(filepath.c_str(), "rb");
    if (!fp)
    {
        std::cerr << "Failed to open file for loading: " << filepath << std::endl;
        m_Done = true;
        return;
    }

    std::vector<char> readBuffer(65536);
    rapidjson::FileReadStream is(fp, readBuffer.data(), readBuffer.size());

    rapidjson::Document doc;
    doc.ParseStream(is);
    fclose(fp);

    if (!doc.IsObject())
    {
        std::cerr << "Invalid JSON format in: " << filepath << std::endl;
        m_Done = true;
        return;
    }

    // Get the SceneGUID
    if (doc.HasMember("SceneGUID"))
    {
        tempData.sceneGUID = doc["SceneGUID"].GetString();
    }

    // Parse the Entities array
    if (doc.HasMember("Entities") && doc["Entities"].IsArray())
    {
        const auto& entities = doc["Entities"];
        tempData.entityList.reserve(entities.Size());

        for (auto& entityData : entities.GetArray())
        {
            int oldID = -1;
            if (entityData.HasMember("MetadataComponent"))
            {
                auto& meta = entityData["MetadataComponent"];
                if (meta.HasMember("EntityID"))
                {
                    oldID = meta["EntityID"].GetInt();
                }
            }

            SceneData::EntityData ed;
            ed.oldEntityID = oldID;

            // Copy the entire object from doc to our local structure
            ed.entityJSON.SetObject();
            ed.entityJSON.CopyFrom(entityData, ed.entityJSON.GetAllocator());

            tempData.entityList.push_back(std::move(ed));
        }
    }

    // Lock & store the result
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_SceneData = std::move(tempData);
    }

    m_Loading = false;
    m_Done = true;
}
