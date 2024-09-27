#include "Serialization.h"
#include "../EngineCore.h"
#include <cstdio>
#include <iostream>
#include <filesystem>

void Serialization::SaveEngineState(const std::string& filepath) {
    // Ensure the Saves directory exists
    std::filesystem::create_directory("Saves");

    rapidjson::Document doc;
    doc.SetObject();
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

    // Serialize entities with components
    rapidjson::Value entities(rapidjson::kArrayType);
    for (const auto& entity : g_Coordinator.GetAliveEntitiesSet()) {
        rapidjson::Value entityData(rapidjson::kObjectType);

        // Serialize MetadataComponent
        if (g_Coordinator.HaveComponent<MetadataComponent>(entity)) 
        {
            auto& name = g_Coordinator.GetComponent<MetadataComponent>(entity);
            rapidjson::Value entityName;
            entityName.SetString(name.GetName().c_str(), allocator);
            entityData.AddMember("EntityName", entityName, allocator);
        }

        // Serialize TransformComponent
        if (g_Coordinator.HaveComponent<TransformComponent>(entity)) {
            auto& transformComp = g_Coordinator.GetComponent<TransformComponent>(entity);

            // Serialize Position
            rapidjson::Value position(rapidjson::kObjectType);
            position.AddMember("x", transformComp.GetPosition().x, allocator);
            position.AddMember("y", transformComp.GetPosition().y, allocator);
            position.AddMember("z", transformComp.GetPosition().z, allocator);
            entityData.AddMember("Position", position, allocator);

            // Serialize Scale
            rapidjson::Value scale(rapidjson::kObjectType);
            scale.AddMember("x", transformComp.GetScale().x, allocator);
            scale.AddMember("y", transformComp.GetScale().y, allocator);
            scale.AddMember("z", transformComp.GetScale().z, allocator);
            entityData.AddMember("Scale", scale, allocator);

            // Serialize Rotation
            rapidjson::Value rotation(rapidjson::kObjectType);
            rotation.AddMember("x", transformComp.GetRotation().x, allocator);
            rotation.AddMember("y", transformComp.GetRotation().y, allocator);
            rotation.AddMember("z", transformComp.GetRotation().z, allocator);
            entityData.AddMember("Rotation", rotation, allocator);
        }

        // Serialize GraphicsComponent
        if (g_Coordinator.HaveComponent<GraphicsComponent>(entity)) {
            auto& graphicsComp = g_Coordinator.GetComponent<GraphicsComponent>(entity);

            // Serialize Model ID
            entityData.AddMember("ModelID", graphicsComp.getModelID(), allocator);

            // Serialize Entity ID
            entityData.AddMember("EntityID", static_cast<int>(entity), allocator);
        }

        entities.PushBack(entityData, allocator);
    }

    doc.AddMember("Entities", entities, allocator);

    // Write the document to a file in the "Saves" folder
    FILE* fp = fopen("Saves/engine_state.json", "wb");
    if (fp) {
        char writeBuffer[65536];
        rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
        rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);
        doc.Accept(writer);
        fclose(fp);
    }
}

void Serialization::LoadEngineState(const std::string& filepath) {
    FILE* fp = fopen(filepath.c_str(), "rb");
    if (!fp) {
        std::cerr << "Failed to open file for loading: " << filepath << std::endl;
        return;
    }

    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    rapidjson::Document doc;
    doc.ParseStream(is);
    fclose(fp);

    if (!doc.IsObject()) {
        std::cerr << "Invalid JSON format in: " << filepath << std::endl;
        return;
    }

    // Deserialize entities with components
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

                // Create GraphicsComponent with the deserialized model ID
                GraphicsComponent graphicsComponent(&g_AssetManager.Models[modelID], entity);
                graphicsComponent.SetModelID(modelID);
                g_Coordinator.AddComponent(entity, graphicsComponent);
            }

            // Set the Entity ID
            int entityID = entityData["EntityID"].GetInt();
        }
    }

    std::cout << "Engine state loaded from " << filepath << std::endl;
}
