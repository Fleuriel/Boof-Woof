/**************************************************************************
 * @file Serialization.cpp
 * @author 	Liu Xujie
 * @param DP email: l.xujie@digipen.edu [2203183]
 * @param Course: CS 3401
 * @param Course: Game Project 3
 * @date  09/22/2024 (22 SEPT 2024)
 * @brief
 *
 * This file contains the definitions of member functions of Serialization
 * Class
 *************************************************************************/
#include "pch.h"

#include "Serialization.h"
#include "../EngineCore.h"
#include <cstdio>

#include "ResourceManager/ResourceManager.h"

// Initialize the static member variable
std::string Serialization::currentSceneGUID = "";


/**************************************************************************
 * @brief Retrieves the file path to the Scenes directory.
 *
 * This function constructs and returns the file path to the Scenes directory
 * located within the "BoofWoof/Assets/Scenes" folder relative to the project
 * root. It starts from the current working directory and navigates to the
 * project's root before appending the Scenes directory path.
 *
 * @return std::string The path to the Scenes directory as a string.
 *************************************************************************/

std::string GetScenesPath() {
    std::filesystem::path currentPath = std::filesystem::current_path();
    std::filesystem::path projectRoot = currentPath.parent_path();
    std::filesystem::path scenesPath = projectRoot / "BoofWoof" / "Assets" / "Scenes";

    return scenesPath.string();
}



/**************************************************************************
 * @brief Generates a random GUID (Globally Unique Identifier).
 *
 * This function generates a random GUID following the UUID version 4 format.
 * It uses a random number generator to create a hexadecimal string formatted
 * in the standard GUID format: 8-4-4-4-12. The version 4 UUID is specified
 * by inserting the "4" at the appropriate position, and the variant is set
 * according to the UUID standard.
 *
 * @return std::string A randomly generated GUID as a string.
 *************************************************************************/
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

/**************************************************************************
 * @brief Retrieves the GUID of the current scene.
 *
 * This function returns the GUID of the current scene, which is typically
 * generated during the save process.
 *
 * @return std::string The GUID of the current scene.
 *************************************************************************/
std::string Serialization::GetSceneGUID() {
    return currentSceneGUID;
}

/**************************************************************************
 * @brief Saves the current scene to a specified file path.
 *
 * This function saves the current scene's data, including entities and
 * their components, to a file in JSON format using the RapidJSON library.
 * The scene's GUID is generated and stored as part of the saved data.
 * It serializes components like `MetadataComponent`, `TransformComponent`,
 * `GraphicsComponent`, `AudioComponent`, and `BehaviorComponent` for
 * all active entities. If the save is successful, it returns `true`.
 *
 * @param filepath The file path where the scene should be saved.
 *
 * @return bool Returns `true` if the scene was successfully saved,
 *         `false` otherwise.
 *
 * @note The function creates necessary directories for the save path if
 * they do not already exist. It also uses a pretty writer to format the
 * output JSON file.
 *************************************************************************/

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
            
            // Model Name
            entityData.AddMember("ModelName", rapidjson::Value(graphicsComp.getModelName().c_str(), allocator), allocator);

            // Texture Name
            entityData.AddMember("Texture", rapidjson::Value(graphicsComp.getTextureName().c_str(), allocator), allocator);

            std::cout << "Graphics Comp Safve Texture: s" << graphicsComp.getTextureName() << '\n';


            //entityData.AddMember("", S)
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

        // Serialize BehaviorComponent
        if (g_Coordinator.HaveComponent<BehaviourComponent>(entity)) 
        {
            auto& behaviorComp = g_Coordinator.GetComponent<BehaviourComponent>(entity);
            rapidjson::Value behaviorNameKey("BehaviourName", allocator); 

            rapidjson::Value behaviorNameValue;
            behaviorNameValue.SetString(behaviorComp.GetBehaviourName().c_str(), allocator);

            entityData.AddMember(behaviorNameKey, behaviorNameValue, allocator);
        }

        // Serialize CollisionComponent
        if (g_Coordinator.HaveComponent<CollisionComponent>(entity)) {
            auto& collisionComp = g_Coordinator.GetComponent<CollisionComponent>(entity);

            rapidjson::Value collisionData(rapidjson::kObjectType);
            collisionData.AddMember("CollisionLayer", collisionComp.GetCollisionLayer(), allocator);

            entityData.AddMember("CollisionComponent", collisionData, allocator);
        }

        entities.PushBack(entityData, allocator);
    }

    doc.AddMember("Entities", entities, allocator);

    FILE* fp = fopen(fullSavePath.c_str(), "wb");
    if (fp) {
        std::vector<char> writeBuffer(65536); 
        rapidjson::FileWriteStream os(fp, writeBuffer.data(), writeBuffer.size());
        rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);
        doc.Accept(writer);
        fclose(fp);
        return true;
    }

    return false;
}

/**************************************************************************
 * @brief Loads a scene from the specified file path.
 *
 * This function loads a scene from a file using the RapidJSON library. It
 * parses the scene data and deserializes the entities and their components,
 * such as `MetadataComponent`, `TransformComponent`, `GraphicsComponent`,
 * `AudioComponent`, and `BehaviourComponent`. Each entity is created and
 * populated with its corresponding components. The scene's GUID is also
 * retrieved and set as the current scene GUID.
 *
 * @param filepath The path to the scene file to be loaded.
 *
 * @return bool Returns `true` if the scene was successfully loaded,
 *         `false` otherwise.
 *
 * @note The function logs errors if the file cannot be opened or if the
 * scene data is not in valid JSON format.
 *************************************************************************/

bool Serialization::LoadScene(const std::string& filepath) {
    FILE* fp = fopen(filepath.c_str(), "rb");
    if (!fp) {
        std::cerr << "Failed to open file for loading: " << filepath << std::endl;
        return false;
    }

    std::vector<char> readBuffer(65536);
    rapidjson::FileReadStream is(fp, readBuffer.data(), readBuffer.size());

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
                

                if (entityData.HasMember("ModelName"))
                {

                    std::string modelName = entityData["ModelName"].GetString();
                    std::string TextureName;
                    


                    if (entityData.HasMember("Texture"))
                    {
                        TextureName = entityData["Texture"].GetString();
                        std::cout << TextureName << '\n';

                    }
                    GraphicsComponent graphicsComponent(modelName, entity, TextureName);
                    graphicsComponent.SetModelID(modelID);
                    
                    std::cout << "graphics: " << graphicsComponent.getModelName() << '\n';

 //                   if (g_ResourceManager.getModel(graphicsComponent.getModelName())->texture_cnt < graphicsComponent.getTextureNumber()) {
                        Texture texture_add;
                        texture_add.id = g_ResourceManager.GetTextureDDS(graphicsComponent.getModelName());
                        if (g_ResourceManager.getModel(graphicsComponent.getModelName())->texture_cnt == 0)
                            texture_add.type = "texture_diffuse";
                        else if (g_ResourceManager.getModel(graphicsComponent.getModelName())->texture_cnt == 1)
                            texture_add.type = "texture_normal";
                        else
                            texture_add.type = "texture_specular";

                        texture_add.path = graphicsComponent.getModelName();

                        //std::cout << "mesh size: " << g_ResourceManager.getModel(graphicsComp.getModelName())->meshes.size() << "\n";
                        std::cout << "\n\n\n\n\n";
                        std::cout << graphicsComponent.getModelName() << '\n';


                        std::cout << "id type path " << texture_add.id << '\t' << texture_add.type << '\t' << texture_add.path << '\n';

                        for (auto& mesh : g_ResourceManager.getModel(graphicsComponent.getModelName())->meshes) {
                            std::cout << "asd\n";

                            mesh.textures.push_back(texture_add);

                        }

                        g_ResourceManager.getModel(graphicsComponent.getModelName())->texture_cnt++;
   //                 }


                    g_Coordinator.AddComponent(entity, graphicsComponent);
                }
            }

            // Deserialize AudioComponent
            if (entityData.HasMember("AudioFilePath")) {
                std::string filePath = entityData["AudioFilePath"].GetString();
                float volume = entityData["Volume"].GetFloat();
                bool shouldLoop = entityData["ShouldLoop"].GetBool();

                AudioComponent audioComponent(filePath, volume, shouldLoop, entity);
                g_Coordinator.AddComponent(entity, audioComponent);
            }

            // Deserialize BehaviourComponent
            if (entityData.HasMember("BehaviourName")) {
                std::string name = entityData["BehaviourName"].GetString();
                g_Coordinator.AddComponent(entity, BehaviourComponent(name, entity));
                //g_Coordinator.GetComponent<BehaviourComponent>(entity).SetBehaviourName(name);
                //BehaviourComponent behaviourComponent(name.c_str(), entity);
                //behaviourComponent.SetBehaviourName(name.c_str());
                
				//std::cout << "Serialization: " << g_Coordinator.GetEntityId(entity) << ". | " << g_Coordinator.GetComponent<BehaviourComponent>(entity).GetBehaviourName() << "." << std::endl;
            }

            // Deserialize CollisionComponent
            if (entityData.HasMember("CollisionComponent")) {
                const auto& collisionData = entityData["CollisionComponent"];

                int layer = collisionData["CollisionLayer"].GetInt();

                CollisionComponent collisionComponent(layer);
                g_Coordinator.AddComponent(entity, collisionComponent);
            }


            // Print out all entity components
			std::cout << "Entity: " << g_Coordinator.GetEntityId(entity) << std::endl;
			if (g_Coordinator.HaveComponent<MetadataComponent>(entity)) {
				std::cout << "MetadataComponent: " << g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() << std::endl;
			}
            if (g_Coordinator.HaveComponent<TransformComponent>(entity)) {
                std::cout << "TransformComponent: " << g_Coordinator.GetComponent<TransformComponent>(entity).GetPosition().x << std::endl;
                std::cout << "TransformComponent: " << g_Coordinator.GetComponent<TransformComponent>(entity).GetPosition().y << std::endl;
                std::cout << "TransformComponent: " << g_Coordinator.GetComponent<TransformComponent>(entity).GetPosition().z << std::endl;
            }
            if (g_Coordinator.HaveComponent<GraphicsComponent>(entity)) {
                std::cout << "GraphicsComponent: " << g_Coordinator.GetComponent<GraphicsComponent>(entity).getModelID() << std::endl;
            }
			if (g_Coordinator.HaveComponent<AudioComponent>(entity)) {
				std::cout << "AudioComponent: " << g_Coordinator.GetComponent<AudioComponent>(entity).GetFilePath().c_str() << std::endl;
				std::cout << "AudioComponent: " << g_Coordinator.GetComponent<AudioComponent>(entity).GetVolume() << std::endl;
                std::cout << "AudioComponent: " << g_Coordinator.GetComponent<AudioComponent>(entity).ShouldLoop() << std::endl;
			}
			if (g_Coordinator.HaveComponent<BehaviourComponent>(entity)) {
				std::cout << "BehaviourComponent: " << g_Coordinator.GetComponent<BehaviourComponent>(entity).GetBehaviourName() << std::endl;
			}
        }
    }

    return true;
}


