#include "Serialization.h"
#include "../EngineCore.h"
#include <cstdio>
#include <iostream>
#include <filesystem>
#include <random>
#include <sstream>
#include <iomanip>

#include "Serialization.h"

// Initialize the static member variable
std::string Serialization::currentSceneGUID = "";


std::string GetScenesPath() {
    std::filesystem::path currentPath = std::filesystem::current_path();
    std::filesystem::path projectRoot = currentPath.parent_path();
    std::filesystem::path scenesPath = projectRoot / "BoofWoof" / "Assets" / "Scenes";

    return scenesPath.string();
}



// Helper function to generate a new GUID
std::string Serialization::GenerateGUID() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, 15);

    std::stringstream ss;
    ss << std::hex;

    for (int i = 0; i < 8; ++i) ss << dis(gen);
    ss << "-";
    for (int i = 0; i < 4; ++i) ss << dis(gen);
    ss << "-4"; // Version 4 UUID
    for (int i = 0; i < 3; ++i) ss << dis(gen);
    ss << "-";
    ss << ((dis(gen) & 0x3) | 0x8); // variant
    for (int i = 0; i < 3; ++i) ss << dis(gen);
    ss << "-";
    for (int i = 0; i < 12; ++i) ss << dis(gen);

    return ss.str();
}

// Retrieve the GUID of the currently loaded scene
std::string Serialization::GetSceneGUID() {
    return currentSceneGUID;
}

bool Serialization::SaveScene(const std::string& filepath) {
    currentSceneGUID = GenerateGUID();

    std::string fullSavePath = GetScenesPath() + "/" + filepath;
    std::filesystem::create_directories(GetScenesPath());

    rapidjson::Document doc;
    doc.SetObject();
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

    // Add the GUID to the scene data
    rapidjson::Value guidValue;
    guidValue.SetString(currentSceneGUID.c_str(), allocator);
    doc.AddMember("SceneGUID", guidValue, allocator);

    // Serialize entities with components
    rapidjson::Value entities(rapidjson::kArrayType);
    for (const auto& entity : g_Coordinator.GetAliveEntitiesSet()) {
        rapidjson::Value entityData(rapidjson::kObjectType);

        // Serialize MetadataComponent
        if (g_Coordinator.HaveComponent<MetadataComponent>(entity)) {
            auto& name = g_Coordinator.GetComponent<MetadataComponent>(entity);
            rapidjson::Value entityName;
            entityName.SetString(name.GetName().c_str(), allocator);
            entityData.AddMember("EntityName", entityName, allocator);
        }

        // Serialize TransformComponent
        if (g_Coordinator.HaveComponent<TransformComponent>(entity)) {
            auto& transformComp = g_Coordinator.GetComponent<TransformComponent>(entity);

            rapidjson::Value position(rapidjson::kObjectType);
            position.AddMember("x", transformComp.GetPosition().x, allocator);
            position.AddMember("y", transformComp.GetPosition().y, allocator);
            position.AddMember("z", transformComp.GetPosition().z, allocator);
            entityData.AddMember("Position", position, allocator);

            rapidjson::Value scale(rapidjson::kObjectType);
            scale.AddMember("x", transformComp.GetScale().x, allocator);
            scale.AddMember("y", transformComp.GetScale().y, allocator);
            scale.AddMember("z", transformComp.GetScale().z, allocator);
            entityData.AddMember("Scale", scale, allocator);

            rapidjson::Value rotation(rapidjson::kObjectType);
            rotation.AddMember("x", transformComp.GetRotation().x, allocator);
            rotation.AddMember("y", transformComp.GetRotation().y, allocator);
            rotation.AddMember("z", transformComp.GetRotation().z, allocator);
            entityData.AddMember("Rotation", rotation, allocator);
        }

        // Serialize GraphicsComponent
        if (g_Coordinator.HaveComponent<GraphicsComponent>(entity)) {
            auto& graphicsComp = g_Coordinator.GetComponent<GraphicsComponent>(entity);
            entityData.AddMember("ModelID", graphicsComp.getModelID(), allocator);
            entityData.AddMember("EntityID", static_cast<int>(entity), allocator);
        }

        // Serialize AudioComponent
        if (g_Coordinator.HaveComponent<AudioComponent>(entity)) {
            auto& audioComp = g_Coordinator.GetComponent<AudioComponent>(entity);

            // Add audio file path
            rapidjson::Value audioFilePath;
            audioFilePath.SetString(audioComp.GetFilePath().c_str(), allocator);
            entityData.AddMember("AudioFilePath", audioFilePath, allocator);

            // Add volume
            entityData.AddMember("Volume", audioComp.GetVolume(), allocator);

            // Add loop status
            entityData.AddMember("ShouldLoop", audioComp.ShouldLoop(), allocator);
        }

        entities.PushBack(entityData, allocator);
    }

    doc.AddMember("Entities", entities, allocator);

    FILE* fp = fopen(fullSavePath.c_str(), "wb");
    if (fp) {
        char writeBuffer[65536];
        rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
        rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);
        doc.Accept(writer);
        fclose(fp);
        return true;
    }
    return false;
}


bool Serialization::LoadScene(const std::string& filepath) {
    FILE* fp = fopen(filepath.c_str(), "rb");
    if (!fp) {
        std::cerr << "Failed to open file for loading: " << filepath << std::endl;
        return false;
    }

    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    rapidjson::Document doc;
    doc.ParseStream(is);
    fclose(fp);

    if (!doc.IsObject()) {
        std::cerr << "Invalid JSON format in: " << filepath << std::endl;
        return false;
    }

    // Deserialize the scene GUID
    if (doc.HasMember("SceneGUID")) {
        currentSceneGUID = doc["SceneGUID"].GetString();
        std::cout << "Loaded scene with GUID: " << currentSceneGUID << std::endl;
    }
    else {
        std::cerr << "No GUID found in scene file" << std::endl;
    }

    // Deserialize entities
    if (doc.HasMember("Entities") && doc["Entities"].IsArray()) {
        const auto& entities = doc["Entities"];
        for (const auto& entityData : entities.GetArray()) {
            Entity entity = g_Coordinator.CreateEntity();

            // Deserialize MetadataComponent
            if (entityData.HasMember("EntityName")) {
                std::string name = entityData["EntityName"].GetString();
                MetadataComponent metadataComponent(name, entity);
                g_Coordinator.AddComponent(entity, metadataComponent);
            }

            // Deserialize TransformComponent
            if (entityData.HasMember("Position")) {
                glm::vec3 position(
                    entityData["Position"]["x"].GetFloat(),
                    entityData["Position"]["y"].GetFloat(),
                    entityData["Position"]["z"].GetFloat()
                );

                glm::vec3 scale(
                    entityData["Scale"]["x"].GetFloat(),
                    entityData["Scale"]["y"].GetFloat(),
                    entityData["Scale"]["z"].GetFloat()
                );

                glm::vec3 rotation(
                    entityData["Rotation"]["x"].GetFloat(),
                    entityData["Rotation"]["y"].GetFloat(),
                    entityData["Rotation"]["z"].GetFloat()
                );

                TransformComponent transformComponent(position, scale, rotation, entity);
                g_Coordinator.AddComponent(entity, transformComponent);
            }

            // Deserialize GraphicsComponent
            if (entityData.HasMember("ModelID")) {
                int modelID = entityData["ModelID"].GetInt();
                GraphicsComponent graphicsComponent(&g_AssetManager.ModelMap["sphere"], entity);
                graphicsComponent.SetModelID(modelID);
                g_Coordinator.AddComponent(entity, graphicsComponent);
            }

            // Deserialize AudioComponent
            if (entityData.HasMember("AudioFilePath")) {
                std::string filePath = entityData["AudioFilePath"].GetString();
                float volume = entityData["Volume"].GetFloat();
                bool shouldLoop = entityData["ShouldLoop"].GetBool();

                AudioComponent audioComponent(filePath, volume, shouldLoop, entity);
                g_Coordinator.AddComponent(entity, audioComponent);
            }
        }
    }

    return true;
}


