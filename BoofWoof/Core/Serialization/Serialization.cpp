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
#include "ResourceManager/ResourceManager.h"
#include "AssetManager/FilePaths.h"

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
    // Get the current executable directory
    std::filesystem::path currentPath = std::filesystem::current_path();

    // Check if "Assets" folder exists in the executable's directory
    std::filesystem::path assetsPath = currentPath / "Assets";
    if (FilePaths::GameCheck == true) {
        // Use the "Assets/Scenes" folder in the current executable directory
        std::filesystem::path scenesPath = assetsPath / "Scenes";
        return scenesPath.string();
    }

    // If no "Assets" folder is found, fallback to navigating to the project root
    std::filesystem::path projectRoot = currentPath.parent_path();
    std::filesystem::path fallbackScenesPath = projectRoot / "BoofWoof" / "Assets" / "Scenes";
    return fallbackScenesPath.string();
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
      //      Grafics.AddMember("Texture", rapidjson::Value(graphicsComp.getTextureName().c_str(), allocator), allocator);
      //
      //      std::cout << "Graphics Comp Safve Texture: s" << graphicsComp.getTextureName() << '\n';
          
            // Follow Camera
            Grafics.AddMember("FollowCamera", graphicsComp.getFollowCamera(), allocator);

            // Add the TransformComponent to the entityData
            entityData.AddMember("GraphicsComponent", Grafics, allocator);

        }

        if (g_Coordinator.HaveComponent<AnimationComponent>(entity))
        {
            rapidjson::Value Anim(rapidjson::kObjectType);

            auto& animationComp = g_Coordinator.GetComponent<AnimationComponent>(entity);

            // Store the number of animation points
            Anim.AddMember("Number of Animation Points",
                rapidjson::Value(static_cast<int>(animationComp.animationVector.size())),
                allocator);

            // Create an array to hold all animation data
            rapidjson::Value animationArray(rapidjson::kArrayType);

            for (const auto& anim : animationComp.animationVector)
            {
                rapidjson::Value animData(rapidjson::kObjectType);
                animData.AddMember("Animation ID", rapidjson::Value(std::get<0>(anim)), allocator);
                animData.AddMember("Start Time", rapidjson::Value(std::get<1>(anim)), allocator);
                animData.AddMember("End Time", rapidjson::Value(std::get<2>(anim)), allocator);

                animationArray.PushBack(animData, allocator);
            }

            // Add the array of animations to the JSON object
            Anim.AddMember("Animations", animationArray, allocator);

            // Now, Anim contains all animation data in JSON format
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
            collisionData.AddMember("IsKinematic", collisionComp.IsKinematic(), allocator);
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

            glm::vec3 particle_color = particleComp.getParticleColor();
            float particle_alpha = particleComp.getOpacity();
            rapidjson::Value colorObj(rapidjson::kObjectType);
            colorObj.AddMember("r", particle_color.r, allocator);  
            colorObj.AddMember("g", particle_color.g, allocator);  
            colorObj.AddMember("b", particle_color.b, allocator);  
            colorObj.AddMember("a", particle_alpha, allocator);
            particles.AddMember("ParticleColor", colorObj, allocator);

            std::string particle_type = particleComp.getTypeString();
			particles.AddMember("ParticleType", rapidjson::Value(particle_type.c_str(), allocator), allocator);

            std::string particle_model = particleComp.getParticleModelname();
			particles.AddMember("ParticleModel", rapidjson::Value(particle_model.c_str(), allocator), allocator);

			std::string particle_texture = particleComp.getParticleTexturename();
			particles.AddMember("ParticleTexture", rapidjson::Value(particle_texture.c_str(), allocator), allocator);
			
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

			light.AddMember("LightShadow", lightComp.getShadow(), allocator);

			light.AddMember("LightDirectionX", lightComp.getDirection().x, allocator);
			light.AddMember("LightDirectionY", lightComp.getDirection().y, allocator);
			light.AddMember("LightDirectionZ", lightComp.getDirection().z, allocator);

			light.AddMember("LightRange", lightComp.getRange(), allocator);

			entityData.AddMember("LightComponent", light, allocator);
		}
        if (g_Coordinator.HaveComponent<MaterialComponent>(entity))
        {

            rapidjson::Value Material(rapidjson::kObjectType);

            auto& materialComponent = g_Coordinator.GetComponent<MaterialComponent>(entity);
            auto& graphicsComponent = g_Coordinator.GetComponent<GraphicsComponent>(entity);

            if (graphicsComponent.material.GetMaterialName() == "")
            {
                graphicsComponent.material.SetMaterialName("Default Material");
            }



            Material.AddMember("name", rapidjson::Value(graphicsComponent.material.GetMaterialName().c_str(), allocator), allocator);
            Material.AddMember("shader", rapidjson::Value(graphicsComponent.material.GetShaderName().c_str(), allocator), allocator);
            Material.AddMember("shaderIdx", graphicsComponent.GetShaderIdx(), allocator);


            // Add properties
            rapidjson::Value properties(rapidjson::kObjectType);

            // Add color (array)
            rapidjson::Value colorArray(rapidjson::kArrayType);
            glm::vec4 color = graphicsComponent.material.GetColor();
            colorArray.PushBack(color.r, allocator);
            colorArray.PushBack(color.g, allocator);
            colorArray.PushBack(color.b, allocator);
            colorArray.PushBack(color.a, allocator);
            properties.AddMember("color", colorArray, allocator);

            // Add finalAlpha (float)
            properties.AddMember("finalAlpha", graphicsComponent.material.GetFinalAlpha(), allocator);

            // Add textures (strings)
            properties.AddMember("Diffuse", rapidjson::Value(graphicsComponent.material.GetDiffuseName().c_str(), allocator), allocator);
            properties.AddMember("Normal", rapidjson::Value(graphicsComponent.material.GetNormalName().c_str(), allocator), allocator);
            properties.AddMember("Height", rapidjson::Value(graphicsComponent.material.GetHeightName().c_str(), allocator), allocator);

            // Add metallic and shininess (floats)
            properties.AddMember("metallic", graphicsComponent.material.GetMetallic(), allocator);
            properties.AddMember("shininess", graphicsComponent.material.GetSmoothness(), allocator);

            // Add properties to the Material
            Material.AddMember("properties", properties, allocator);


            //std::cout << materialComponent.GetMaterialName() << '\n' << materialComponent.GetShaderName() << '\n' << materialComponent.GetShaderIndex() << '\n';

            //graphicsComponent.material = materialComponent;


            entityData.AddMember("MaterialComponent", Material, allocator);
        }


		// Serialize UIComponent
        if (g_Coordinator.HaveComponent<UIComponent>(entity)) {
			rapidjson::Value UI(rapidjson::kObjectType);

			auto& uiComp = g_Coordinator.GetComponent<UIComponent>(entity);

			UI.AddMember("TextureName", rapidjson::Value(uiComp.get_texturename().c_str(), allocator), allocator);
			UI.AddMember("PositionX", uiComp.get_position().x, allocator);
			UI.AddMember("PositionY", uiComp.get_position().y, allocator);
			UI.AddMember("ScaleX", uiComp.get_scale().x, allocator);
			UI.AddMember("ScaleY", uiComp.get_scale().y, allocator);
			UI.AddMember("Layer", uiComp.get_UI_layer(), allocator);
			UI.AddMember("Selectable", uiComp.get_selectable(), allocator);
			UI.AddMember("Opcaity", uiComp.get_opacity(), allocator);
            
            UI.AddMember("Animated", uiComp.get_animate(), allocator);
            UI.AddMember("Rows", uiComp.get_rows(), allocator);
            UI.AddMember("Cols", uiComp.get_cols(), allocator);
            // UI.AddMember("CurrentRow", uiComp.get_curr_row(), allocator);
            // UI.AddMember("CurrentCol", uiComp.get_curr_col(), allocator);
            UI.AddMember("FrameInterval", uiComp.get_frame_interval(), allocator);
            // UI.AddMember("Timer", uiComp.get_timer(), allocator);
            UI.AddMember("StayOnRow", uiComp.get_stay_on_row(), allocator);

			entityData.AddMember("UIComponent", UI, allocator);

        }
		// Serialize FontComponent
        if (g_Coordinator.HaveComponent<FontComponent>(entity)) {
            rapidjson::Value Font(rapidjson::kObjectType);

            auto& fontComp = g_Coordinator.GetComponent<FontComponent>(entity);


            Font.AddMember("Family", rapidjson::Value(fontComp.get_family().c_str(), allocator), allocator);
            Font.AddMember("PositionX", fontComp.get_pos().x, allocator);
            Font.AddMember("PositionY", fontComp.get_pos().y, allocator);
            Font.AddMember("ScaleX", fontComp.get_scale().x, allocator);
			Font.AddMember("ScaleY", fontComp.get_scale().y, allocator);
            Font.AddMember("ColorR", fontComp.get_color().x, allocator);
            Font.AddMember("ColorG", fontComp.get_color().y, allocator);
            Font.AddMember("ColorB", fontComp.get_color().z, allocator);
            Font.AddMember("Text", rapidjson::Value(fontComp.get_text().c_str(), allocator), allocator);
			Font.AddMember("Layer", fontComp.get_layer(), allocator);

            entityData.AddMember("FontComponent", Font, allocator);
        }


        // Serialization for PathfindingComponent
        if (g_Coordinator.HaveComponent<PathfindingComponent>(entity)) {
            rapidjson::Value pathfindingData(rapidjson::kObjectType);

            auto& pathfindingComponent = g_Coordinator.GetComponent<PathfindingComponent>(entity);

            // Serialize start and goal nodes
            pathfindingData.AddMember("StartNode", static_cast<int>(pathfindingComponent.GetStartNode()), allocator);
            pathfindingData.AddMember("GoalNode", static_cast<int>(pathfindingComponent.GetGoalNode()), allocator);

            // Serialize path (optional, as it might be recalculated at runtime)
            rapidjson::Value pathArray(rapidjson::kArrayType);
            for (const auto& point : pathfindingComponent.GetPath()) {
                rapidjson::Value pointData(rapidjson::kObjectType);
                pointData.AddMember("x", point.x, allocator);
                pointData.AddMember("y", point.y, allocator);
                pointData.AddMember("z", point.z, allocator);
                pathArray.PushBack(pointData, allocator);
            }
            pathfindingData.AddMember("Path", pathArray, allocator);

            // Add PathfindingComponent to entity data
            entityData.AddMember("PathfindingComponent", pathfindingData, allocator);
        }

        // Serialization for NodeComponent
        if (g_Coordinator.HaveComponent<NodeComponent>(entity)) {
            rapidjson::Value nodeData(rapidjson::kObjectType);

            auto& nodeComponent = g_Coordinator.GetComponent<NodeComponent>(entity);

            // Serialize position
            rapidjson::Value position(rapidjson::kObjectType);
            position.AddMember("x", nodeComponent.GetPosition().x, allocator);
            position.AddMember("y", nodeComponent.GetPosition().y, allocator);
            position.AddMember("z", nodeComponent.GetPosition().z, allocator);
            nodeData.AddMember("Position", position, allocator);

            // Serialize walkable status
            nodeData.AddMember("IsWalkable", nodeComponent.IsWalkable(), allocator);

            // Add NodeComponent to entity data
            entityData.AddMember("NodeComponent", nodeData, allocator);
        }
        
        // Serialization for EdgeComponent
        if (g_Coordinator.HaveComponent<EdgeComponent>(entity)) {
            rapidjson::Value edgeData(rapidjson::kObjectType);

            auto& edgeComponent = g_Coordinator.GetComponent<EdgeComponent>(entity);

            // Serialize start and end nodes
            edgeData.AddMember("StartNode", static_cast<int>(edgeComponent.GetStartNode()), allocator);
            edgeData.AddMember("EndNode", static_cast<int>(edgeComponent.GetEndNode()), allocator);

            // Serialize cost
            edgeData.AddMember("Cost", edgeComponent.GetCost(), allocator);

            // Add EdgeComponent to entity data
            entityData.AddMember("EdgeComponent", edgeData, allocator);
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

                    GraphicsComponent graphicsComponent(modelName, entity, isFollowing);

                    graphicsComponent.SetModel(&g_ResourceManager.ModelMap[modelName]);

       //             if (textureID > 0)
       //                 graphicsComponent.AddTexture(textureID);

                    std::cout << "graphics: " << graphicsComponent.getModelName() << '\n';

         //           std::cout << "model text number: " << g_ResourceManager.getModel(graphicsComponent.getModelName())->texture_cnt << '\n';
         //           std::cout << "comp  text number: " << graphicsComponent.getTextureNumber() << '\n';

                    g_Coordinator.AddComponent(entity, graphicsComponent);
                }
            }

            if (entityData.HasMember("AnimationComponent"))
            {
                const rapidjson::Value& Anim = entityData["AnimationComponent"];

                if (Anim.HasMember("Number of Animation Points") && Anim.HasMember("Animations"))
                {
                    // Retrieve the animation component
                    auto& animationComp = g_Coordinator.GetComponent<AnimationComponent>(entity);

                    // Clear existing animations before loading new ones
                    animationComp.animationVector.clear();

                    int numPoints = Anim["Number of Animation Points"].GetInt();
                    const rapidjson::Value& animationArray = Anim["Animations"];

                    if (animationArray.IsArray())
                    {
                        for (rapidjson::SizeType i = 0; i < animationArray.Size(); i++)
                        {
                            const rapidjson::Value& animData = animationArray[i];

                            if (animData.HasMember("Animation ID") && animData.HasMember("Start Time") && animData.HasMember("End Time"))
                            {
                                int animID = animData["Animation ID"].GetInt();
                                float startTime = animData["Start Time"].GetFloat();
                                float endTime = animData["End Time"].GetFloat();

                                animationComp.animationVector.emplace_back(animID, startTime, endTime);
                            }
                        }
                    }

                    std::cout << "Loaded " << numPoints << " animation points.\n";
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

                    AudioComponent audioComponent(filePath, volume, shouldLoop, entity,nullptr, AudioType::SFX);
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
                bool isKinematic = collisionData.HasMember("IsKinematic") ? collisionData["IsKinematic"].GetBool() : false;
                bool isPlayer = collisionData.HasMember("IsPlayer") ? collisionData["IsPlayer"].GetBool() : false;

                CollisionComponent collisionComponent(layer);
                collisionComponent.SetIsDynamic(isDynamic);
                collisionComponent.SetIsKinematic(isKinematic);
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

                    glm::vec3 particleColor(r, g, b);
					float opacity = a;

                    std::string particle_type = "POINT";
					if (PData.HasMember("ParticleType") ) particle_type = PData["ParticleType"].GetString();

					ParticleType type = ParticleComponent::stringToParticleType(particle_type);

                    std::string particle_model = "Bed";
					if (PData.HasMember("ParticleModel")) particle_model = PData["ParticleModel"].GetString();
                       

                    std::string particle_texture = "Bed";
					if (PData.HasMember("ParticleTexture")) particle_texture = PData["ParticleTexture"].GetString();

					ParticleComponent particleComponent(density, positionMin, positionMax, velocityMin, velocityMax, target_positions, particleSize, particleColor, opacity, type, particle_model, particle_texture);
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
                    bool shadow = false;
					if (LData.HasMember("LightShadow"))
					{
						shadow = LData["LightShadow"].GetBool();
					}
                    glm::vec3 dire = glm::vec3(1.0f, 0.0f, 0.0f);
					if (LData.HasMember("LightDirectionX"))
					{
						dire = glm::vec3(
							LData["LightDirectionX"].GetFloat(),
							LData["LightDirectionY"].GetFloat(),
							LData["LightDirectionZ"].GetFloat()
						);
					}
					float range = 0.0f;
					if (LData.HasMember("LightRange"))
					{
						range = LData["LightRange"].GetFloat();
					}

					LightComponent lightComponent (intensity, color, shadow, dire, range);
                    g_Coordinator.AddComponent(entity, lightComponent);


                }
               
			}
          //  if (entityData.HasMember("MaterialComponent"))
          //  {
          //
          //      const auto& MatData = entityData["MaterialComponent"];
          //
          //
          //
          //      std::string materialName = MatData["name"].GetString();
          //      std::string shaderName = MatData["shader"].GetString();
          //      int shaderIndex = MatData["shaderIdx"].GetInt();
          //
          //
          //
          //
          //      std::string diffuseName = "NothingDiffuse";
          //      std::string normalName = "NothingNormal";
          //      std::string heightName = "NothingHeight";
          //
          //      glm::vec4 color(1.0f);
          //      float metallic = 0.0f;
          //      float shininess = 0.0f;
          //      float finalAlpha = 0.0f;
          //
          //
          //
          //      // Extract material properties
          //      const auto& properties = MatData["properties"];
          //      if (properties.HasMember("color"))
          //      {
          //          const auto& colorArray = properties["color"];
          //          if (colorArray.Size() == 4)
          //          {
          //              color =
          //                  glm::vec4(
          //                      colorArray[0].GetFloat(),
          //                      colorArray[1].GetFloat(),
          //                      colorArray[2].GetFloat(),
          //                      colorArray[3].GetFloat()
          //                  );
          //          }
          //      }
          //
          //      if (properties.HasMember("finalAlpha"))
          //          finalAlpha = properties["finalAlpha"].GetFloat();
          //
          //      if (properties.HasMember("Diffuse"))
          //          diffuseName = properties["Diffuse"].GetString();
          //
          //      if (properties.HasMember("Normal"))
          //          normalName = properties["Normal"].GetString();
          //
          //      if (properties.HasMember("Height"))
          //          heightName = properties["Height"].GetString();
          //
          //      if (properties.HasMember("metallic"))
          //          metallic = properties["metallic"].GetFloat();
          //
          //      if (properties.HasMember("shininess"))
          //          shininess = properties["shininess"].GetFloat();
          //
          //
          //
          //      //std::cout << materialName << '\t' << shaderName << '\t' << shaderIndex << '\t' << diffuseName << '\t' << normalName << '\n' << heightName << '\t' << metallic << '\t' << shininess << '\t' << finalAlpha << '\t' << diffuseName << '\n' <<
          //      //    normalName << '\t' << heightName << '\t' << metallic << '\t' << shininess << '\n';
          //
          //
          //
          //
          //
          //
          //
          //      MaterialComponent materialComponent;
          //
          //
          //
          //
          //      // In Order:
          //
          //      materialComponent.SetMaterialName(materialName);
          //      materialComponent.SetShaderName(shaderName);
          //      materialComponent.SetShaderIndex(shaderIndex);
          //
          //
          //      materialComponent.SetColor(color);
          //
          //      materialComponent.SetFinalAlpha(finalAlpha);
          //      materialComponent.SetDiffuseName(diffuseName);
          //      materialComponent.SetDiffuseID(g_ResourceManager.GetTextureDDS(diffuseName));
          //      materialComponent.SetNormalName(normalName);
          //      materialComponent.SetHeightName(heightName);
          //
          //      materialComponent.SetMetallic(metallic);
          //      materialComponent.SetSmoothness(shininess);
          //
          //      std::cout << materialComponent.GetMaterialName() << '\t' << materialComponent.GetShaderName() << '\t' << materialComponent.GetShaderIndex() << '\t' << '\n';
          //
          //      if (g_Coordinator.HaveComponent<GraphicsComponent>(entity))
          //      {
          //          auto& graphicsComp = g_Coordinator.GetComponent<GraphicsComponent>(entity);
          //   //       graphicsComp.material = materialComponent;
          //
      //  //           graphicsComp.setTexture(materialComponent.GetDiffuseName());
          //
        ////            graphicsComp.SetDiffuse(materialComponent.GetDiffuseID());
          //
          //          //                    graphicsComp.AddTexture(g_ResourceManager.GetTextureDDS(graphicsComp.material.GetDiffuseName()));
          //
          //
          //                             // graphicsComp.material.SetDiffuseID(g_ResourceManager.GetTextureDDS(graphicsComp.material.GetDiffuseName()));
          //                             // graphicsComp.material.SetDiffuseName(graphicsComp.material.GetDiffuseName());
          //      //    ;
          //
          //     //     std::cout << graphicsComp.material.GetMaterialName() << '\t' << graphicsComp.material.GetShaderName() << '\t' << graphicsComp.material.GetShaderIndex() << '\t' << '\n';
          //          
          //      }
          //
          //
          //      g_Coordinator.AddComponent(entity, materialComponent);
          //      std::cout << "material component deserialized" << std::endl;;
          //
          //  }
			// Deserialize UIComponent
			if (entityData.HasMember("UIComponent")) 
            {
				const auto& UIData = entityData["UIComponent"];
				
                std::string textureName{};
                /*if (UIData.HasMember("TextureID")) {
                    int textureID = UIData["TextureID"].GetInt();
					textureName = g_ResourceManager.GetTextureDDSFileName(textureID);
                }*/

					
				if (UIData.HasMember("TextureName")) {
					textureName = UIData["TextureName"].GetString();
				}

				glm::vec2 position = glm::vec2(0.0f);
				if (UIData.HasMember("PositionX") && UIData.HasMember("PositionY"))
					position = glm::vec2(UIData["PositionX"].GetFloat(), UIData["PositionY"].GetFloat());

				glm::vec2 scale = glm::vec2(1.0f);
				if (UIData.HasMember("ScaleX") && UIData.HasMember("ScaleY"))
					scale = glm::vec2(UIData["ScaleX"].GetFloat(), UIData["ScaleY"].GetFloat());


				float layer = 0.0f;
				if (UIData.HasMember("Layer"))
					layer = UIData["Layer"].GetFloat();

				bool selectable = false;
				if (UIData.HasMember("Selectable"))
					selectable = UIData["Selectable"].GetBool();

				float opacity = 1.0f;
				if (UIData.HasMember("Opcaity"))                
					opacity = UIData["Opcaity"].GetFloat();

				UIComponent uiComponent(textureName, position, scale, layer, selectable, opacity);

				if (UIData.HasMember("Animated"))
					uiComponent.set_animate(UIData["Animated"].GetBool());

				if (UIData.HasMember("Rows"))
					uiComponent.set_rows(UIData["Rows"].GetInt());

				if (UIData.HasMember("Cols"))
					uiComponent.set_cols(UIData["Cols"].GetInt());

				if (UIData.HasMember("FrameInterval"))
					uiComponent.set_frame_interval(UIData["FrameInterval"].GetFloat());

				if (UIData.HasMember("StayOnRow"))
					uiComponent.set_stay_on_row(UIData["StayOnRow"].GetBool());

				g_Coordinator.AddComponent(entity, uiComponent);
				
			}

			if (entityData.IsObject() && entityData.HasMember("FontComponent"))
			{
				const auto& fontData = entityData["FontComponent"];


                std::string family = "Arial";
				if (fontData.HasMember("Family"))
                    family = fontData["Family"].GetString();

                glm::vec2 pos{};
				if (fontData.HasMember("PositionX") && fontData.HasMember("PositionY"))
                    pos = glm::vec2(fontData["PositionX"].GetFloat(), fontData["PositionY"].GetFloat());

				glm::vec2 scale = { 1.0f, 1.0f };
				if (fontData.HasMember("ScaleX") && fontData.HasMember("ScaleY"))
					scale = glm::vec2(fontData["ScaleX"].GetFloat(), fontData["ScaleY"].GetFloat());

                glm::vec3 color = { 1.0f, 1.0f, 1.0f };
				if (fontData.HasMember("ColorR") && fontData.HasMember("ColorG") && fontData.HasMember("ColorB"))
                    color= glm::vec3(fontData["ColorR"].GetFloat(), fontData["ColorG"].GetFloat(), fontData["ColorB"].GetFloat());
                std::string text{};
				if (fontData.HasMember("Text"))
                    text= fontData["Text"].GetString();
				float layer = 0.0f;
				if (fontData.HasMember("Layer"))
					layer = fontData["Layer"].GetFloat();

				FontComponent fontComponent(family, pos, scale, color, text, layer);
				g_Coordinator.AddComponent(entity, fontComponent);
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

            // Deserialization for PathfindingComponent
            if (entityData.HasMember("PathfindingComponent")) 
            {
                const auto& pathfindingData = entityData["PathfindingComponent"];

                Entity startNode = INVALID_ENTITY;
                if (pathfindingData.HasMember("StartNode")) {
                    startNode = static_cast<Entity>(pathfindingData["StartNode"].GetInt());
                }

                Entity goalNode = INVALID_ENTITY;
                if (pathfindingData.HasMember("GoalNode")) {
                    goalNode = static_cast<Entity>(pathfindingData["GoalNode"].GetInt());
                }

                std::vector<glm::vec3> path;
                if (pathfindingData.HasMember("Path")) {
                    const auto& pathArray = pathfindingData["Path"];
                    for (const auto& pointData : pathArray.GetArray()) {
                        glm::vec3 point(
                            pointData["x"].GetFloat(),
                            pointData["y"].GetFloat(),
                            pointData["z"].GetFloat()
                        );
                        path.push_back(point);
                    }
                }

                PathfindingComponent pathfindingComponent;
                pathfindingComponent.SetStartNode(startNode);
                pathfindingComponent.SetGoalNode(goalNode);
                pathfindingComponent.SetPath(path);

                g_Coordinator.AddComponent(entity, pathfindingComponent);
            }

            // Deserialization for NodeComponent
            if (entityData.HasMember("NodeComponent")) {
                const auto& nodeData = entityData["NodeComponent"];

                glm::vec3 position(0.0f);
                if (nodeData.HasMember("Position")) {
                    position = glm::vec3(
                        nodeData["Position"]["x"].GetFloat(),
                        nodeData["Position"]["y"].GetFloat(),
                        nodeData["Position"]["z"].GetFloat()
                    );
                }

                bool isWalkable = true;
                if (nodeData.HasMember("IsWalkable")) {
                    isWalkable = nodeData["IsWalkable"].GetBool();
                }

                NodeComponent nodeComponent;
                nodeComponent.SetPosition(position);
                nodeComponent.SetWalkable(isWalkable);

                g_Coordinator.AddComponent(entity, nodeComponent);
            }

            // Deserialization for EdgeComponent
            if (entityData.HasMember("EdgeComponent")) {
                const auto& edgeData = entityData["EdgeComponent"];

                Entity startNode = INVALID_ENTITY;
                if (edgeData.HasMember("StartNode")) {
                    startNode = static_cast<Entity>(edgeData["StartNode"].GetInt());
                }

                Entity endNode = INVALID_ENTITY;
                if (edgeData.HasMember("EndNode")) {
                    endNode = static_cast<Entity>(edgeData["EndNode"].GetInt());
                }

                float cost = 1.0f;
                if (edgeData.HasMember("Cost")) {
                    cost = edgeData["Cost"].GetFloat();
                }

                EdgeComponent edgeComponent;
                edgeComponent.SetStartNode(startNode);
                edgeComponent.SetEndNode(endNode);
                edgeComponent.SetCost(cost);

                g_Coordinator.AddComponent(entity, edgeComponent);
            }


            // Store the entity
            storedEnt.push_back(entity);
        }

        // Second pass: Deserialize components that reference other entities
        for (const auto& entityData : entities.GetArray()) 
        {
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


void Serialization::FinalizeEntitiesFromSceneData(const SceneData& data)
{
    // If you want to clear the current ECS entities before finalizing:
    // g_Coordinator.ResetEntities();

    // 1) Map from oldEntityID -> new ECS Entity
    std::unordered_map<int, Entity> oldToNewEntityMap;

    // First pass: create all ECS entities
    // -----------------------------------
    for (auto& ed : data.entityList)
    {
        const int oldID = ed.oldEntityID;
        Entity newE = g_Coordinator.CreateEntity();
        oldToNewEntityMap[oldID] = newE;
    }

    // 2) Attach non-hierarchy components (second pass)
    // ------------------------------------------------
    // (This is analogous to the �for each entityData in doc['Entities']� part
    //  in your original LoadScene, except we read from ed.entityJSON here.)
    for (auto& ed : data.entityList)
    {
        Entity newE = oldToNewEntityMap[ed.oldEntityID];
        auto& jsonObj = ed.entityJSON;

        // -- MetadataComponent --
        if (jsonObj.HasMember("MetadataComponent"))
        {
            const auto& metaData = jsonObj["MetadataComponent"];
            if (metaData.HasMember("EntityName"))
            {
                std::string name = metaData["EntityName"].GetString();
                MetadataComponent metaComp(name, newE);
                g_Coordinator.AddComponent(newE, metaComp);
            }
        }

        // -- TransformComponent --
        if (jsonObj.HasMember("TransformComponent"))
        {
            const auto& TData = jsonObj["TransformComponent"];
            glm::vec3 position(0.0f), scale(1.0f), rotation(0.0f), rotPivot(0.0f);

            if (TData.HasMember("Position"))
            {
                position.x = TData["Position"]["x"].GetFloat();
                position.y = TData["Position"]["y"].GetFloat();
                position.z = TData["Position"]["z"].GetFloat();
            }

            if (TData.HasMember("Scale"))
            {
                scale.x = TData["Scale"]["x"].GetFloat();
                scale.y = TData["Scale"]["y"].GetFloat();
                scale.z = TData["Scale"]["z"].GetFloat();
            }

            if (TData.HasMember("Rotation"))
            {
                rotation.x = TData["Rotation"]["x"].GetFloat();
                rotation.y = TData["Rotation"]["y"].GetFloat();
                rotation.z = TData["Rotation"]["z"].GetFloat();
            }

            if (TData.HasMember("RotationPivotOffset"))
            {
                rotPivot.x = TData["RotationPivotOffset"]["x"].GetFloat();
                rotPivot.y = TData["RotationPivotOffset"]["y"].GetFloat();
                rotPivot.z = TData["RotationPivotOffset"]["z"].GetFloat();
            }

            TransformComponent transform(position, scale, rotation, rotPivot, newE);
            g_Coordinator.AddComponent(newE, transform);
        }

        // -- GraphicsComponent --
        if (jsonObj.HasMember("GraphicsComponent"))
        {
            const auto& GData = jsonObj["GraphicsComponent"];
            std::string modelName;
            std::string textureName;
            bool followCam = false;

            if (GData.HasMember("ModelName"))
            {
                modelName = GData["ModelName"].GetString();
            }
            if (GData.HasMember("Texture"))
            {
                textureName = GData["Texture"].GetString();
            }
            if (GData.HasMember("FollowCamera"))
            {
                followCam = GData["FollowCamera"].GetBool();
            }

            GraphicsComponent graphicsComp(modelName, newE, followCam);
            // If you want to retrieve textureID from ResourceManager:
            int textureID = g_ResourceManager.GetTextureDDS(textureName);
            if (textureID > 0)
            {
                graphicsComp.AddTexture(textureID);
            }

            g_Coordinator.AddComponent(newE, graphicsComp);
        }

        // -- AnimationComponent --
        if (jsonObj.HasMember("AnimationComponent"))
        {

            

            const auto& GData = jsonObj["AnimationComponent"];

            if (GData.HasMember("Number of Animation Points"))
            {
                
            }



            AnimationComponent animComp();


            g_Coordinator.AddComponent(newE, animComp);
        }

        // -- AudioComponent --
        if (jsonObj.HasMember("AudioComponent"))
        {
            const auto& AData = jsonObj["AudioComponent"];
            if (AData.HasMember("AudioFilePath"))
            {
                std::string filePath = AData["AudioFilePath"].GetString();
                float volume = AData["Volume"].GetFloat();
                bool loop = AData["ShouldLoop"].GetBool();

                AudioComponent audioC(filePath, volume, loop, newE, nullptr, AudioType::SFX);
                g_Coordinator.AddComponent(newE, audioC);
            }
        }

        // -- BehaviourComponent --
        if (jsonObj.HasMember("BehaviourComponent"))
        {
            const auto& BData = jsonObj["BehaviourComponent"];
            if (BData.HasMember("BehaviourName"))
            {
                std::string behaviour = BData["BehaviourName"].GetString();
                BehaviourComponent bComp(behaviour, newE);
                g_Coordinator.AddComponent(newE, bComp);
            }
        }

        // -- CollisionComponent --
        if (jsonObj.HasMember("CollisionComponent"))
        {
            const auto& CData = jsonObj["CollisionComponent"];
            int layer = CData["CollisionLayer"].GetInt();
            bool isDynamic = CData["IsDynamic"].GetBool();
            bool isKinematic = CData["IsKinematic"].GetBool();
            bool isPlayer = CData["IsPlayer"].GetBool();

            CollisionComponent colComp(layer);
            colComp.SetIsDynamic(isDynamic);
            colComp.SetIsKinematic(isKinematic);
            colComp.SetIsPlayer(isPlayer);

            if (CData.HasMember("AABBSize"))
            {
                glm::vec3 sz;
                sz.x = CData["AABBSize"]["x"].GetFloat();
                sz.y = CData["AABBSize"]["y"].GetFloat();
                sz.z = CData["AABBSize"]["z"].GetFloat();
                colComp.SetAABBSize(sz);
            }

            if (CData.HasMember("AABBOffset"))
            {
                glm::vec3 off;
                off.x = CData["AABBOffset"]["x"].GetFloat();
                off.y = CData["AABBOffset"]["y"].GetFloat();
                off.z = CData["AABBOffset"]["z"].GetFloat();
                colComp.SetAABBOffset(off);
            }

            if (CData.HasMember("Mass"))
            {
                float mass = CData["Mass"].GetFloat();
                colComp.SetMass(mass);
            }

            g_Coordinator.AddComponent(newE, colComp);
        }

        // -- CameraComponent --
        if (jsonObj.HasMember("CameraComponent"))
        {
            const auto& CC = jsonObj["CameraComponent"];
            glm::vec3 pos(0), up(0, 1, 0);
            float yaw = 0.f;
            float pitch = 0.f;
            bool active = false;

            if (CC.HasMember("Position"))
            {
                pos.x = CC["Position"]["x"].GetFloat();
                pos.y = CC["Position"]["y"].GetFloat();
                pos.z = CC["Position"]["z"].GetFloat();
            }

            if (CC.HasMember("Up"))
            {
                up.x = CC["Up"]["x"].GetFloat();
                up.y = CC["Up"]["y"].GetFloat();
                up.z = CC["Up"]["z"].GetFloat();
            }

            if (CC.HasMember("Yaw"))   yaw = CC["Yaw"].GetFloat();
            if (CC.HasMember("Pitch")) pitch = CC["Pitch"].GetFloat();
            if (CC.HasMember("Active")) active = CC["Active"].GetBool();

            CameraComponent camC(pos, up, yaw, pitch, active);
            g_Coordinator.AddComponent(newE, camC);
        }

        // -- ParticleComponent --
        if (jsonObj.HasMember("ParticleComponent"))
        {
            const auto& PData = jsonObj["ParticleComponent"];
            glm::vec3 posMin(0), posMax(0);
            float density = 1.0f;
            float velMin = 0.f;
            float velMax = 0.f;
            float size = 1.f;
            glm::vec3 color(1.f);
			float opacity = 1.f;

            std::vector<glm::vec3> targetPositions;

            if (PData.HasMember("PositionMin"))
            {
                posMin.x = PData["PositionMin"]["x"].GetFloat();
                posMin.y = PData["PositionMin"]["y"].GetFloat();
                posMin.z = PData["PositionMin"]["z"].GetFloat();
            }
            if (PData.HasMember("PositionMax"))
            {
                posMax.x = PData["PositionMax"]["x"].GetFloat();
                posMax.y = PData["PositionMax"]["y"].GetFloat();
                posMax.z = PData["PositionMax"]["z"].GetFloat();
            }
            if (PData.HasMember("Density"))
            {
                density = PData["Density"].GetFloat();
            }
            if (PData.HasMember("VelocityMin"))
            {
                velMin = PData["VelocityMin"].GetFloat();
            }
            if (PData.HasMember("VelocityMax"))
            {
                velMax = PData["VelocityMax"].GetFloat();
            }
            if (PData.HasMember("TargetPositions"))
            {
                const auto& arr = PData["TargetPositions"].GetArray();
                for (auto& item : arr)
                {
                    glm::vec3 temp(
                        item["x"].GetFloat(),
                        item["y"].GetFloat(),
                        item["z"].GetFloat()
                    );
                    targetPositions.push_back(temp);
                }
            }
            if (PData.HasMember("ParticleSize"))
            {
                size = PData["ParticleSize"].GetFloat();
            }
            if (PData.HasMember("ParticleColor"))
            {
                const auto& cObj = PData["ParticleColor"];
                color.r = cObj["r"].GetFloat();
                color.g = cObj["g"].GetFloat();
                color.b = cObj["b"].GetFloat();
                opacity = cObj["a"].GetFloat();
            }
            std::string particle_type = "POINT";
            if (PData.HasMember("ParticleType")) particle_type = PData["ParticleType"].GetString();

            ParticleType type = ParticleComponent::stringToParticleType(particle_type);

            std::string particle_model = "Bed";
            if (PData.HasMember("ParticleModel")) particle_model = PData["ParticleModel"].GetString();


            std::string particle_texture = "Bed";
            if (PData.HasMember("ParticleTexture")) particle_texture = PData["ParticleTexture"].GetString();

            ParticleComponent partC(density, posMin, posMax, velMin, velMax, targetPositions, size, color, opacity, type, particle_model, particle_texture);
            g_Coordinator.AddComponent(newE, partC);
        }

        // -- LightComponent --
        if (jsonObj.HasMember("LightComponent"))
        {
            const auto& LData = jsonObj["LightComponent"];
            float intensity = 1.f;
            glm::vec3 lightCol(1.f);
			bool shadow = false;
			glm::vec3 direction(1.f, 0.f, 0.f);
			float range = 10.f;

            if (LData.HasMember("LightIntensity"))
            {
                intensity = LData["LightIntensity"].GetFloat();
            }
            if (LData.HasMember("LightColor"))
            {
                lightCol.x = LData["LightColor"]["r"].GetFloat();
                lightCol.y = LData["LightColor"]["g"].GetFloat();
                lightCol.z = LData["LightColor"]["b"].GetFloat();
            }
			if (LData.HasMember("LightShadow"))
			{
				shadow = LData["LightShadow"].GetBool();
			}
			if (LData.HasMember("LightDirectionX"))
			{
				direction.x = LData["LightDirectionX"].GetFloat();
				direction.y = LData["LightDirectionY"].GetFloat();
				direction.z = LData["LightDirectionZ"].GetFloat();

			}
			if (LData.HasMember("LightRange"))
			{
				range = LData["LightRange"].GetFloat();
			}

            LightComponent lightC(intensity, lightCol, shadow, direction, range);
            g_Coordinator.AddComponent(newE, lightC);
        }

        // -- MaterialComponent --
        if (jsonObj.HasMember("MaterialComponent"))
        {
            const auto& MatData = jsonObj["MaterialComponent"];
            std::string matName, shaderName;
            int shaderIdx = 0;

            if (MatData.HasMember("name"))
                matName = MatData["name"].GetString();
            if (MatData.HasMember("shader"))
                shaderName = MatData["shader"].GetString();
            if (MatData.HasMember("shaderIdx"))
                shaderIdx = MatData["shaderIdx"].GetInt();

            MaterialComponent matC;
            matC.SetMaterialName(matName);
            matC.SetShaderName(shaderName);
            matC.SetShaderIndex(shaderIdx);

            if (MatData.HasMember("properties"))
            {
                const auto& props = MatData["properties"];

                // color
                if (props.HasMember("color"))
                {
                    const auto& cArr = props["color"].GetArray();
                    glm::vec4 col(
                        cArr[0].GetFloat(),
                        cArr[1].GetFloat(),
                        cArr[2].GetFloat(),
                        cArr[3].GetFloat()
                    );
                    matC.SetColor(col);
                }
                // finalAlpha
                if (props.HasMember("finalAlpha"))
                {
                    matC.SetFinalAlpha(props["finalAlpha"].GetFloat());
                }
                // Diffuse / Normal / Height
                if (props.HasMember("Diffuse"))
                {
                    std::string diff = props["Diffuse"].GetString();
                    matC.SetDiffuseName(diff);
                    matC.SetDiffuseID(g_ResourceManager.GetTextureDDS(diff));
                }
                if (props.HasMember("Normal"))
                {
                    std::string norm = props["Normal"].GetString();
                    matC.SetNormalName(norm);
                }
                if (props.HasMember("Height"))
                {
                    std::string hgt = props["Height"].GetString();
                    matC.SetHeightName(hgt);
                }
                // metallic
                if (props.HasMember("metallic"))
                {
                    matC.SetMetallic(props["metallic"].GetFloat());
                }
                // shininess
                if (props.HasMember("shininess"))
                {
                    matC.SetSmoothness(props["shininess"].GetFloat());
                }
            }

            // If the entity also has a GraphicsComponent, attach the mat
            if (g_Coordinator.HaveComponent<GraphicsComponent>(newE))
            {
                auto& gComp = g_Coordinator.GetComponent<GraphicsComponent>(newE);
                gComp.material = matC;
                // If there's a diffuse name, set the texture
                gComp.setTexture(matC.GetDiffuseName());
                gComp.SetDiffuse(matC.GetDiffuseID());
            }

            g_Coordinator.AddComponent(newE, matC);
        }

        // -- UIComponent --
        if (jsonObj.HasMember("UIComponent"))
        {
            const auto& UIData = jsonObj["UIComponent"];
            std::string textureName;
            // Prefer the string key:
            if (UIData.HasMember("TextureName"))
            {
                textureName = UIData["TextureName"].GetString();
            }
            else if (UIData.HasMember("TextureID"))
            {
                int textureID = UIData["TextureID"].GetInt();
                textureName = g_ResourceManager.GetTextureDDSFileName(textureID);
            }

            glm::vec2 pos(0.f), scl(1.f);
            float layer = 0.f;
            bool selectable = false;
            float opacity = 1.f;
            bool animated = false;
            int rows = 1;
            int cols = 1;
            float frameInt = 0.f;

            if (UIData.HasMember("PositionX") && UIData.HasMember("PositionY"))
            {
                pos.x = UIData["PositionX"].GetFloat();
                pos.y = UIData["PositionY"].GetFloat();
            }
            if (UIData.HasMember("ScaleX") && UIData.HasMember("ScaleY"))
            {
                scl.x = UIData["ScaleX"].GetFloat();
                scl.y = UIData["ScaleY"].GetFloat();
            }
            if (UIData.HasMember("Layer"))
                layer = UIData["Layer"].GetFloat();
            if (UIData.HasMember("Selectable"))
                selectable = UIData["Selectable"].GetBool();
            if (UIData.HasMember("Opcaity"))
                opacity = UIData["Opcaity"].GetFloat();
            if (UIData.HasMember("Animated"))
                animated = UIData["Animated"].GetBool();
            if (UIData.HasMember("Rows"))
                rows = UIData["Rows"].GetInt();
            if (UIData.HasMember("Cols"))
                cols = UIData["Cols"].GetInt();
            if (UIData.HasMember("FrameInterval"))
                frameInt = UIData["FrameInterval"].GetFloat();

            UIComponent uiC(textureName, pos, scl, layer, selectable, opacity);
            uiC.set_animate(animated);
            uiC.set_rows(rows);
            uiC.set_cols(cols);
            uiC.set_frame_interval(frameInt);

            g_Coordinator.AddComponent(newE, uiC);
        }
    }

    // 3) Third pass: Hierarchy references or other entity references
    // --------------------------------------------------------------
    for (auto& ed : data.entityList)
    {
        Entity newE = oldToNewEntityMap[ed.oldEntityID];
        auto& jsonObj = ed.entityJSON;

        if (jsonObj.HasMember("HierarchyComponent"))
        {
            const auto& HCData = jsonObj["HierarchyComponent"];
            Entity parentEntity = MAX_ENTITIES;

            if (HCData.HasMember("Parent"))
            {
                int oldParentID = HCData["Parent"].GetInt();
                if (oldParentID != MAX_ENTITIES)
                {
                    parentEntity = oldToNewEntityMap[oldParentID];
                }
            }

            std::vector<Entity> childrenEntities;
            if (HCData.HasMember("Children") && HCData["Children"].IsArray())
            {
                for (auto& childVal : HCData["Children"].GetArray())
                {
                    int oldChildID = childVal.GetInt();
                    Entity childE = oldToNewEntityMap[oldChildID];
                    childrenEntities.push_back(childE);
                }
            }

            HierarchyComponent hierC;
            hierC.parent = parentEntity;
            hierC.children = childrenEntities;
            g_Coordinator.AddComponent(newE, hierC);
        }

        // If you have other components that store references to old entity IDs,
        // handle them here as well, using oldToNewEntityMap to convert.
    }
}
