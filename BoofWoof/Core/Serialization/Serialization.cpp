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
std::vector<Entity> Serialization::storedEnt;

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

std::vector<Entity> Serialization::GetStored()
{
    return storedEnt;
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
        if (g_Coordinator.HaveComponent<MetadataComponent>(entity)) 
        {
            rapidjson::Value metadataComponent(rapidjson::kObjectType);
            auto& name = g_Coordinator.GetComponent<MetadataComponent>(entity);
            rapidjson::Value entityName;
            entityName.SetString(name.GetName().c_str(), allocator);

            // Add the EntityName inside the MetadataComponent
            metadataComponent.AddMember("EntityName", entityName, allocator);
            metadataComponent.AddMember("EntityID", static_cast<int>(entity), allocator);

            // Add the MetadataComponent to the entityData
            entityData.AddMember("MetadataComponent", metadataComponent, allocator);
        }

        // Serialize TransformComponent
        if (g_Coordinator.HaveComponent<TransformComponent>(entity)) 
        {
            rapidjson::Value Trans(rapidjson::kObjectType);

            auto& transformComp = g_Coordinator.GetComponent<TransformComponent>(entity);

            rapidjson::Value position(rapidjson::kObjectType);
            position.AddMember("x", transformComp.GetPosition().x, allocator);
            position.AddMember("y", transformComp.GetPosition().y, allocator);
            position.AddMember("z", transformComp.GetPosition().z, allocator);
            // Add the Position inside the TransformComponent
            Trans.AddMember("Position", position, allocator);

            rapidjson::Value scale(rapidjson::kObjectType);
            scale.AddMember("x", transformComp.GetScale().x, allocator);
            scale.AddMember("y", transformComp.GetScale().y, allocator);
            scale.AddMember("z", transformComp.GetScale().z, allocator);
            Trans.AddMember("Scale", scale, allocator);

            rapidjson::Value rotation(rapidjson::kObjectType);
            rotation.AddMember("x", transformComp.GetRotation().x, allocator);
            rotation.AddMember("y", transformComp.GetRotation().y, allocator);
            rotation.AddMember("z", transformComp.GetRotation().z, allocator);
            Trans.AddMember("Rotation", rotation, allocator);

            // Serialize Rotation Pivot Offset
            rapidjson::Value rotationPivotOffset(rapidjson::kObjectType);
            rotationPivotOffset.AddMember("x", transformComp.GetRotationPivotOffset().x, allocator);
            rotationPivotOffset.AddMember("y", transformComp.GetRotationPivotOffset().y, allocator);
            rotationPivotOffset.AddMember("z", transformComp.GetRotationPivotOffset().z, allocator);
            Trans.AddMember("RotationPivotOffset", rotationPivotOffset, allocator);

            // Add the TransformComponent to the entityData
            entityData.AddMember("TransformComponent", Trans, allocator);
        }

        // Serialize GraphicsComponent
        if (g_Coordinator.HaveComponent<GraphicsComponent>(entity)) 
        {
            rapidjson::Value Grafics(rapidjson::kObjectType);

            auto& graphicsComp = g_Coordinator.GetComponent<GraphicsComponent>(entity);
            
            // Model Name
            Grafics.AddMember("ModelName", rapidjson::Value(graphicsComp.getModelName().c_str(), allocator), allocator);



            // Texture Name
            Grafics.AddMember("Texture", rapidjson::Value(graphicsComp.getTextureName().c_str(), allocator), allocator);

            std::cout << "Graphics Comp Safve Texture: s" << graphicsComp.getTextureName() << '\n';
          
            // Follow Camera
            Grafics.AddMember("FollowCamera", graphicsComp.getFollowCamera(), allocator);

            // Add the TransformComponent to the entityData
            entityData.AddMember("GraphicsComponent", Grafics, allocator);
        }

        // Serialize AudioComponent
        if (g_Coordinator.HaveComponent<AudioComponent>(entity)) 
        {
            rapidjson::Value Aud(rapidjson::kObjectType);

            auto& audioComp = g_Coordinator.GetComponent<AudioComponent>(entity);

            // Add audio file path
            rapidjson::Value audioFilePath;
            audioFilePath.SetString(audioComp.GetFilePath().c_str(), allocator);
            Aud.AddMember("AudioFilePath", audioFilePath, allocator);

            // Add volume
            Aud.AddMember("Volume", audioComp.GetVolume(), allocator);

            // Add loop status
            Aud.AddMember("ShouldLoop", audioComp.ShouldLoop(), allocator);

            // Add the AudioComponent to the entityData
            entityData.AddMember("AudioComponent", Aud, allocator);
        }

        // Serialize BehaviorComponent
        if (g_Coordinator.HaveComponent<BehaviourComponent>(entity)) 
        {
            rapidjson::Value Behave(rapidjson::kObjectType);

            auto& behaviorComp = g_Coordinator.GetComponent<BehaviourComponent>(entity);
            rapidjson::Value behaviorNameKey("BehaviourName", allocator); 

            rapidjson::Value behaviorNameValue;
            behaviorNameValue.SetString(behaviorComp.GetBehaviourName().c_str(), allocator);

            Behave.AddMember(behaviorNameKey, behaviorNameValue, allocator);

            // Add the BehaviourComponent to the entityData
            entityData.AddMember("BehaviourComponent", Behave, allocator);
        }

        // Serialize HierarchyComponent
        if (g_Coordinator.HaveComponent<HierarchyComponent>(entity))
        {
            rapidjson::Value hierarchyData(rapidjson::kObjectType);
            auto& hierarchyComp = g_Coordinator.GetComponent<HierarchyComponent>(entity);

            // Serialize parent entity ID
            hierarchyData.AddMember("Parent", static_cast<int>(hierarchyComp.parent), allocator);

            // Serialize children entity IDs
            rapidjson::Value childrenArray(rapidjson::kArrayType);
            for (auto childEntity : hierarchyComp.children)
            {
                childrenArray.PushBack(static_cast<int>(childEntity), allocator);
            }
            hierarchyData.AddMember("Children", childrenArray, allocator);

            // Add the HierarchyComponent to the entityData
            entityData.AddMember("HierarchyComponent", hierarchyData, allocator);
        }


        // Serialize CollisionComponent
        if (g_Coordinator.HaveComponent<CollisionComponent>(entity)) 
        {
            auto& collisionComp = g_Coordinator.GetComponent<CollisionComponent>(entity);

            rapidjson::Value collisionData(rapidjson::kObjectType);
            collisionData.AddMember("CollisionLayer", collisionComp.GetCollisionLayer(), allocator);
            collisionData.AddMember("IsDynamic", collisionComp.IsDynamic(), allocator);
            collisionData.AddMember("IsPlayer", collisionComp.IsPlayer(), allocator);

            // Save AABB size
            rapidjson::Value aabbSize(rapidjson::kObjectType);
            aabbSize.AddMember("x", collisionComp.GetAABBSize().x, allocator);
            aabbSize.AddMember("y", collisionComp.GetAABBSize().y, allocator);
            aabbSize.AddMember("z", collisionComp.GetAABBSize().z, allocator);
            collisionData.AddMember("AABBSize", aabbSize, allocator);

            // Save AABB offset
            rapidjson::Value aabbOffset(rapidjson::kObjectType);
            aabbOffset.AddMember("x", collisionComp.GetAABBOffset().x, allocator);
            aabbOffset.AddMember("y", collisionComp.GetAABBOffset().y, allocator);
            aabbOffset.AddMember("z", collisionComp.GetAABBOffset().z, allocator);
            collisionData.AddMember("AABBOffset", aabbOffset, allocator);

            // Save mass
            collisionData.AddMember("Mass", collisionComp.GetMass(), allocator);

            entityData.AddMember("CollisionComponent", collisionData, allocator);
        }

        // Serialize CameraComponent
        if (g_Coordinator.HaveComponent<CameraComponent>(entity))
        {
            rapidjson::Value Cam(rapidjson::kObjectType);

            auto& cameraComp = g_Coordinator.GetComponent<CameraComponent>(entity);

            rapidjson::Value position(rapidjson::kObjectType);
            position.AddMember("x", cameraComp.GetCameraPosition().x, allocator);
            position.AddMember("y", cameraComp.GetCameraPosition().y, allocator);
            position.AddMember("z", cameraComp.GetCameraPosition().z, allocator);
            // Add the Position inside the CameraComponent
            Cam.AddMember("Position", position, allocator);

            // Add Yaw & Pitch
            Cam.AddMember("Yaw", cameraComp.GetCameraYaw(), allocator);
            Cam.AddMember("Pitch", cameraComp.GetCameraPitch(), allocator);

            rapidjson::Value Up(rapidjson::kObjectType);
            Up.AddMember("x", cameraComp.GetCameraUp().x, allocator);
            Up.AddMember("y", cameraComp.GetCameraUp().y, allocator);
            Up.AddMember("z", cameraComp.GetCameraUp().z, allocator);
            // Add the Position inside the CameraComponent
            Cam.AddMember("Up", Up, allocator);

            Cam.AddMember("Active", cameraComp.GetCameraActive(), allocator);

            // Add the CameraComponent to the entityData
            entityData.AddMember("CameraComponent", Cam, allocator);
        }

        if (g_Coordinator.HaveComponent<ParticleComponent>(entity))
        {
            rapidjson::Value particles(rapidjson::kObjectType);

            auto& particleComp = g_Coordinator.GetComponent<ParticleComponent>(entity);

            rapidjson::Value positionMin(rapidjson::kObjectType);
            positionMin.AddMember("x", particleComp.getPosMin().x, allocator);
            positionMin.AddMember("y", particleComp.getPosMin().y, allocator);
            positionMin.AddMember("z", particleComp.getPosMin().z, allocator);
            particles.AddMember("PositionMin", positionMin, allocator);

            rapidjson::Value positionMax(rapidjson::kObjectType);
            positionMax.AddMember("x", particleComp.getPosMax().x, allocator);
            positionMax.AddMember("y", particleComp.getPosMax().y, allocator);
            positionMax.AddMember("z", particleComp.getPosMax().z, allocator);
            particles.AddMember("PositionMax", positionMax, allocator);

            particles.AddMember("Density", particleComp.getDensity(), allocator);
            particles.AddMember("VelocityMin", particleComp.getVelocityMin(), allocator);
            particles.AddMember("VelocityMax", particleComp.getVelocityMax(), allocator);

            std::vector<glm::vec3> target_positions = particleComp.getTargetPositions();
            rapidjson::Value targetPositionsArray(rapidjson::kArrayType);   // Create an array for target positions

            for (const auto& pos : target_positions) {
                rapidjson::Value targetPos(rapidjson::kObjectType);  // Create an object for each target position

                targetPos.AddMember("x", pos.x, allocator);
                targetPos.AddMember("y", pos.y, allocator);
                targetPos.AddMember("z", pos.z, allocator);

                targetPositionsArray.PushBack(targetPos, allocator);  // Add each position object to the array
            }
            particles.AddMember("TargetPositions", targetPositionsArray, allocator);

            particles.AddMember("ParticleSize", particleComp.getParticleSize(), allocator);

            glm::vec4 particle_color = particleComp.getParticleColor();
            rapidjson::Value colorObj(rapidjson::kObjectType);
            colorObj.AddMember("r", particle_color.r, allocator);  
            colorObj.AddMember("g", particle_color.g, allocator);  
            colorObj.AddMember("b", particle_color.b, allocator);  
            colorObj.AddMember("a", particle_color.a, allocator);  
            particles.AddMember("ParticleColor", colorObj, allocator);

            // Add the CameraComponent to the entityData
            entityData.AddMember("ParticleComponent", particles, allocator);
        }
		if (g_Coordinator.HaveComponent<LightComponent>(entity))
		{
			rapidjson::Value light(rapidjson::kObjectType);

			auto& lightComp = g_Coordinator.GetComponent<LightComponent>(entity);

			light.AddMember("LightIntensity", lightComp.getIntensity(), allocator);

			rapidjson::Value color(rapidjson::kObjectType);
			color.AddMember("r", lightComp.getColor().r, allocator);
			color.AddMember("g", lightComp.getColor().g, allocator);
			color.AddMember("b", lightComp.getColor().b, allocator);

			light.AddMember("LightColor", color, allocator);

			entityData.AddMember("LightComponent", light, allocator);
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
bool Serialization::LoadScene(const std::string& filepath)
{
    storedEnt.clear();

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

    // Declare mapping from old entity IDs to new entities
    std::unordered_map<int, Entity> oldToNewEntityMap;

    // Deserialize entities
    if (doc.HasMember("Entities") && doc["Entities"].IsArray()) {
        const auto& entities = doc["Entities"];

        // First pass: Create all entities and store mapping
        for (const auto& entityData : entities.GetArray()) {
            Entity entity = g_Coordinator.CreateEntity();

            // Get the old EntityID
            int oldEntityID = -1;
            if (entityData.HasMember("MetadataComponent"))
            {
                const auto& metadataComponentData = entityData["MetadataComponent"];
                if (metadataComponentData.HasMember("EntityID"))
                {
                    oldEntityID = metadataComponentData["EntityID"].GetInt();
                }
            }

            // Store mapping from oldID to newEntity
            if (oldEntityID != -1)
            {
                oldToNewEntityMap[oldEntityID] = entity;
            }
            else
            {
                // Handle error: EntityID not found
                std::cerr << "EntityID not found in MetadataComponent during deserialization.\n";
                continue;
            }

            // Deserialize MetadataComponent
            if (entityData.HasMember("MetadataComponent"))
            {
                const auto& metadataComponentData = entityData["MetadataComponent"];
                if (metadataComponentData.HasMember("EntityName"))
                {
                    std::string name = metadataComponentData["EntityName"].GetString();

                    MetadataComponent metadataComponent(name, entity);
                    g_Coordinator.AddComponent(entity, metadataComponent);
                }
            }

            // Deserialize TransformComponent
            if (entityData.HasMember("TransformComponent"))
            {
                const auto& TData = entityData["TransformComponent"];
                glm::vec3 position(0.0f), scale(1.0f), rotation(0.0f), rotationPivotOffset(0.0f);

                // Check and load Position
                if (TData.HasMember("Position")) {
                    position = glm::vec3(
                        TData["Position"]["x"].GetFloat(),
                        TData["Position"]["y"].GetFloat(),
                        TData["Position"]["z"].GetFloat()
                    );
                }

                // Check and load Scale
                if (TData.HasMember("Scale")) {
                    scale = glm::vec3(
                        TData["Scale"]["x"].GetFloat(),
                        TData["Scale"]["y"].GetFloat(),
                        TData["Scale"]["z"].GetFloat()
                    );
                }

                // Check and load Rotation
                if (TData.HasMember("Rotation")) {
                    rotation = glm::vec3(
                        TData["Rotation"]["x"].GetFloat(),
                        TData["Rotation"]["y"].GetFloat(),
                        TData["Rotation"]["z"].GetFloat()
                    );
                }

                // Deserialize Rotation Pivot Offset
                if (TData.HasMember("RotationPivotOffset")) {
                    rotationPivotOffset = glm::vec3(
                        TData["RotationPivotOffset"]["x"].GetFloat(),
                        TData["RotationPivotOffset"]["y"].GetFloat(),
                        TData["RotationPivotOffset"]["z"].GetFloat()
                    );
                }

                TransformComponent transformComponent(position, scale, rotation, rotationPivotOffset, entity);
                g_Coordinator.AddComponent(entity, transformComponent);
            }

            // Deserialize GraphicsComponent
            if (entityData.HasMember("GraphicsComponent"))
            {
                const auto& GData = entityData["GraphicsComponent"];

                if (GData.HasMember("ModelName"))
                {

                    std::string modelName = GData["ModelName"].GetString();
                    std::string TextureName;
                    bool isFollowing{};

                    std::cout << "has member tex" << GData.HasMember("Texture") << '\n';

                    if (GData.HasMember("Texture"))
                    {
                        TextureName = GData["Texture"].GetString();
                        std::cout << "Texture: " << TextureName << '\n';

                    }

                    int textureID = g_ResourceManager.GetTextureDDS(TextureName);

                    if (GData.HasMember("FollowCamera"))
                    {
                        isFollowing = GData["FollowCamera"].GetBool();
                    }

                    GraphicsComponent graphicsComponent(modelName, entity, TextureName, isFollowing);

                    if (textureID > 0)
                        graphicsComponent.AddTexture(textureID);

                    std::cout << "graphics: " << graphicsComponent.getModelName() << '\n';

                    std::cout << "model text number: " << g_ResourceManager.getModel(graphicsComponent.getModelName())->texture_cnt << '\n';
                    std::cout << "comp  text number: " << graphicsComponent.getTextureNumber() << '\n';

                    g_Coordinator.AddComponent(entity, graphicsComponent);
                }
            }

            // Deserialize AudioComponent
            if (entityData.HasMember("AudioComponent"))
            {
                const auto& AData = entityData["AudioComponent"];
                if (AData.HasMember("AudioFilePath")) {
                    std::string filePath = AData["AudioFilePath"].GetString();
                    float volume = AData["Volume"].GetFloat();
                    bool shouldLoop = AData["ShouldLoop"].GetBool();

                    AudioComponent audioComponent(filePath, volume, shouldLoop, entity);
                    g_Coordinator.AddComponent(entity, audioComponent);
                }
            }

            // Deserialize BehaviourComponent
            if (entityData.HasMember("BehaviourComponent"))
            {
                const auto& BData = entityData["BehaviourComponent"];
                if (BData.HasMember("BehaviourName"))
                {
                    std::string name = BData["BehaviourName"].GetString();
                    g_Coordinator.AddComponent(entity, BehaviourComponent(name, entity));
                }
            }

            // Deserialize CollisionComponent
            if (entityData.HasMember("CollisionComponent")) {
                const auto& collisionData = entityData["CollisionComponent"];

                int layer = collisionData["CollisionLayer"].GetInt();
                bool isDynamic = collisionData.HasMember("IsDynamic") ? collisionData["IsDynamic"].GetBool() : false;
                bool isPlayer = collisionData.HasMember("IsPlayer") ? collisionData["IsPlayer"].GetBool() : false;

                CollisionComponent collisionComponent(layer);
                collisionComponent.SetIsDynamic(isDynamic);
                collisionComponent.SetIsPlayer(isPlayer);

                // Load AABB size
                if (collisionData.HasMember("AABBSize")) {
                    const auto& aabbSize = collisionData["AABBSize"];
                    glm::vec3 loadedAABBSize(
                        aabbSize["x"].GetFloat(),
                        aabbSize["y"].GetFloat(),
                        aabbSize["z"].GetFloat()
                    );
                    collisionComponent.SetAABBSize(loadedAABBSize);
                }

                // Load AABB offset
                if (collisionData.HasMember("AABBOffset")) {
                    const auto& aabbOffset = collisionData["AABBOffset"];
                    glm::vec3 loadedAABBOffset(
                        aabbOffset["x"].GetFloat(),
                        aabbOffset["y"].GetFloat(),
                        aabbOffset["z"].GetFloat()
                    );
                    collisionComponent.SetAABBOffset(loadedAABBOffset);
                }

                // Load mass
                if (collisionData.HasMember("Mass")) {
                    float mass = collisionData["Mass"].GetFloat();
                    collisionComponent.SetMass(mass);
                }

                g_Coordinator.AddComponent(entity, collisionComponent);
            }

            // Deserialize CameraComponent
            if (entityData.HasMember("CameraComponent"))
            {
                const auto& CData = entityData["CameraComponent"];
                if (CData.HasMember("Position"))
                {
                    glm::vec3 camPosition(
                        CData["Position"]["x"].GetFloat(),
                        CData["Position"]["y"].GetFloat(),
                        CData["Position"]["z"].GetFloat()
                    );

                    glm::vec3 Up(
                        CData["Up"]["x"].GetFloat(),
                        CData["Up"]["y"].GetFloat(),
                        CData["Up"]["z"].GetFloat()
                    );

                    float yaw = CData["Yaw"].GetFloat();
                    float pitch = CData["Pitch"].GetFloat();
                    bool active = CData["Active"].GetBool();

                    CameraComponent cameraComponent(camPosition, Up, yaw, pitch, active);
                    g_Coordinator.AddComponent(entity, cameraComponent);
                }
            }

            // Deserialize ParticleComponent
            if (entityData.HasMember("ParticleComponent"))
            {
                const auto& PData = entityData["ParticleComponent"];
                if (PData.HasMember("PositionMin"))
                {
                    glm::vec3 positionMin(
                        PData["PositionMin"]["x"].GetFloat(),
                        PData["PositionMin"]["y"].GetFloat(),
                        PData["PositionMin"]["z"].GetFloat()
                    );

                    glm::vec3 positionMax(
                        PData["PositionMax"]["x"].GetFloat(),
                        PData["PositionMax"]["y"].GetFloat(),
                        PData["PositionMax"]["z"].GetFloat()
                    );

                    float density = PData["Density"].GetFloat();
                    float velocityMin = PData["VelocityMin"].GetFloat();
                    float velocityMax = PData["VelocityMax"].GetFloat();

                    const rapidjson::Value& targetPositionsArray = PData["TargetPositions"];
                    std::vector<glm::vec3> target_positions;

                    for (rapidjson::SizeType i = 0; i < targetPositionsArray.Size(); i++) {
                        const rapidjson::Value& targetPos = targetPositionsArray[i];

                        float x = targetPos["x"].GetFloat();
                        float y = targetPos["y"].GetFloat();
                        float z = targetPos["z"].GetFloat();

                        target_positions.push_back(glm::vec3(x, y, z));
                    }

                    float particleSize = PData["ParticleSize"].GetFloat();

                    const rapidjson::Value& colorObj = PData["ParticleColor"];

                    float r = colorObj["r"].GetFloat();
                    float g = colorObj["g"].GetFloat();
                    float b = colorObj["b"].GetFloat();
                    float a = colorObj["a"].GetFloat();

                    glm::vec4 particleColor(r, g, b, a);

                    ParticleComponent particleComponent(density, positionMin, positionMax, velocityMin, velocityMax, target_positions, particleSize, particleColor);
                    g_Coordinator.AddComponent(entity, particleComponent);
                }
            }

            if(entityData.HasMember("LightComponent"))
			{
				const auto& LData = entityData["LightComponent"];
                if (LData.HasMember("LightIntensity")) {

                    float intensity = LData["LightIntensity"].GetFloat();

					glm::vec3 color(
						LData["LightColor"]["r"].GetFloat(),
						LData["LightColor"]["g"].GetFloat(),
						LData["LightColor"]["b"].GetFloat()
					);

					LightComponent lightComponent (intensity, color);
                    g_Coordinator.AddComponent(entity, lightComponent);


                }
               
			}

            // Print out all entity components
			//std::cout << "Entity: " << g_Coordinator.GetEntityId(entity) << std::endl;
			//if (g_Coordinator.HaveComponent<MetadataComponent>(entity)) {
			//	std::cout << "MetadataComponent: " << g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() << std::endl;
			//}
   //         if (g_Coordinator.HaveComponent<TransformComponent>(entity)) {
   //             std::cout << "TransformComponent: " << g_Coordinator.GetComponent<TransformComponent>(entity).GetPosition().x << std::endl;
   //             std::cout << "TransformComponent: " << g_Coordinator.GetComponent<TransformComponent>(entity).GetPosition().y << std::endl;
   //             std::cout << "TransformComponent: " << g_Coordinator.GetComponent<TransformComponent>(entity).GetPosition().z << std::endl;
   //         }
   //         if (g_Coordinator.HaveComponent<GraphicsComponent>(entity)) {
   //             std::cout << "GraphicsComponent: " << g_Coordinator.GetComponent<GraphicsComponent>(entity).getModelID() << std::endl;
   //         }
			//if (g_Coordinator.HaveComponent<AudioComponent>(entity)) {
			//	std::cout << "AudioComponent: " << g_Coordinator.GetComponent<AudioComponent>(entity).GetFilePath().c_str() << std::endl;
			//	std::cout << "AudioComponent: " << g_Coordinator.GetComponent<AudioComponent>(entity).GetVolume() << std::endl;
   //             std::cout << "AudioComponent: " << g_Coordinator.GetComponent<AudioComponent>(entity).ShouldLoop() << std::endl;
			//}
			//if (g_Coordinator.HaveComponent<BehaviourComponent>(entity)) {
			//	std::cout << "BehaviourComponent: " << g_Coordinator.GetComponent<BehaviourComponent>(entity).GetBehaviourName() << std::endl;
			//}

            // Store the entity
            storedEnt.push_back(entity);
        }

        // Second pass: Deserialize components that reference other entities
        for (const auto& entityData : entities.GetArray()) {
            // Get the old EntityID
            int oldEntityID = -1;
            if (entityData.HasMember("MetadataComponent"))
            {
                const auto& metadataComponentData = entityData["MetadataComponent"];
                if (metadataComponentData.HasMember("EntityID"))
                {
                    oldEntityID = metadataComponentData["EntityID"].GetInt();
                }
            }

            // Get the new entity using the mapping
            Entity newEntity = oldToNewEntityMap[oldEntityID];

            // Deserialize HierarchyComponent
            if (entityData.HasMember("HierarchyComponent"))
            {
                const auto& hierarchyData = entityData["HierarchyComponent"];

                Entity parentEntity = MAX_ENTITIES;
                if (hierarchyData.HasMember("Parent"))
                {
                    int oldParentID = hierarchyData["Parent"].GetInt();
                    if (oldParentID != MAX_ENTITIES)
                    {
                        // Get the new parent entity
                        parentEntity = oldToNewEntityMap[oldParentID];
                    }
                }

                std::vector<Entity> childrenEntities;
                if (hierarchyData.HasMember("Children") && hierarchyData["Children"].IsArray())
                {
                    const auto& childrenArray = hierarchyData["Children"];
                    for (const auto& childIDValue : childrenArray.GetArray())
                    {
                        int oldChildID = childIDValue.GetInt();
                        Entity childEntity = oldToNewEntityMap[oldChildID];
                        childrenEntities.push_back(childEntity);
                    }
                }

                // Create HierarchyComponent and add to entity
                HierarchyComponent hierarchyComponent;
                hierarchyComponent.parent = parentEntity;
                hierarchyComponent.children = childrenEntities;

                g_Coordinator.AddComponent(newEntity, hierarchyComponent);
            }

            // If you have other components that reference entities, deserialize them here

            // Print out all entity components (optional debugging)
            /*
            std::cout << "Entity: " << g_Coordinator.GetEntityId(newEntity) << std::endl;
            if (g_Coordinator.HaveComponent<MetadataComponent>(newEntity)) {
                std::cout << "MetadataComponent: " << g_Coordinator.GetComponent<MetadataComponent>(newEntity).GetName() << std::endl;
            }
            if (g_Coordinator.HaveComponent<TransformComponent>(newEntity)) {
                auto& transformComp = g_Coordinator.GetComponent<TransformComponent>(newEntity);
                std::cout << "TransformComponent Position: " << transformComp.GetPosition().x << ", " << transformComp.GetPosition().y << ", " << transformComp.GetPosition().z << std::endl;
            }
            if (g_Coordinator.HaveComponent<GraphicsComponent>(newEntity)) {
                std::cout << "GraphicsComponent: " << g_Coordinator.GetComponent<GraphicsComponent>(newEntity).getModelName() << std::endl;
            }
            if (g_Coordinator.HaveComponent<AudioComponent>(newEntity)) {
                auto& audioComp = g_Coordinator.GetComponent<AudioComponent>(newEntity);
                std::cout << "AudioComponent FilePath: " << audioComp.GetFilePath() << ", Volume: " << audioComp.GetVolume() << ", ShouldLoop: " << audioComp.ShouldLoop() << std::endl;
            }
            if (g_Coordinator.HaveComponent<BehaviourComponent>(newEntity)) {
                std::cout << "BehaviourComponent: " << g_Coordinator.GetComponent<BehaviourComponent>(newEntity).GetBehaviourName() << std::endl;
            }
            if (g_Coordinator.HaveComponent<HierarchyComponent>(newEntity)) {
                auto& hierarchyComp = g_Coordinator.GetComponent<HierarchyComponent>(newEntity);
                std::cout << "HierarchyComponent Parent: " << hierarchyComp.parent << ", Children Count: " << hierarchyComp.children.size() << std::endl;
            }
            */
        }
    }

    return true;
}
