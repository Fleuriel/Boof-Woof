#ifndef ANIMATION_MANAGER_H
#define ANIMATION_MANAGER_H

#include "Coordinator.hpp"
#include "../Utilities/Components/AnimationComponent.h"
#include "../Core/Graphics/Model.h"
#include "../Core/Graphics/Mesh.h"



class AnimationManager {
public:
    // Loads an animation from a file and associates it with a name
    bool LoadAnimation(const std::string& animationName, const std::string& filePath) {
        Animation animation;
        if (animation.LoadAnimation(filePath)) {
            animations[animationName] = animation;
            
            std::cout << "yea\n";

            return true;
        }

        return false;
    }

    // Checks if an animation is loaded
    bool IsAnimationLoaded(const std::string& animationName) const {
        return animations.find(animationName) != animations.end();
    }

    // Retrieves an animation by name
    const Animation& GetAnimation(const std::string& animationName) const {
        auto it = animations.find(animationName);
        if (it != animations.end()) {
            return it->second;
        }
        throw std::runtime_error("Animation not found: " + animationName);
    }

    // Updates all animation components
    void Update(double deltaTime) {
        auto allEntities = g_Coordinator.GetAliveEntitiesSet();
        for (auto& entity : allEntities)
            if (g_Coordinator.HaveComponent<AnimationComponent>(entity))
                g_Coordinator.GetComponent<AnimationComponent>(entity).Update(deltaTime);
    }

    void PrintSize() { std::cout << "Size is " << animations.size() << std::endl; }

    void PrintAllAnimations() const {
        if (animations.empty()) {
            std::cout << "No animations loaded." << std::endl;
            return;
        }

        std::cout << "Loaded Animations (" << animations.size() << "):" << std::endl;
        for (const auto& pair : animations) {
            const std::string& name = pair.first;
            const Animation& anim = pair.second;

            std::cout << " - Animation Name: " << name << std::endl;
            std::cout << "   Duration: " << anim.GetDuration() << " seconds" << std::endl;
            std::cout << "   Tick Rate: " << anim.GetTicksPerSecond() << " ticks/sec" << std::endl;
            std::cout << "   Bone Count: " << anim.GetBoneInfoMap().size() << std::endl;
            std::cout << "   Mesh Count: " << anim.GetMeshes().size() << std::endl;


    
            std::cout << "Mesh Data Anim: \t" << anim.meshDataMesh.size() << '\n';

            std::cout << "MeshD ata Vertices : \t" << anim.meshDataMesh[0].vertices.size() << '\n';
            std::cout << "Mesh Data indices: '\t" <<anim.meshDataMesh[0].indices.size() << '\n';


            // Print bone names
            std::cout << "   Bones: ";
            for (const auto& bone : anim.GetBoneInfoMap()) {
                std::cout << bone.first << " ";
            }
            std::cout << std::endl;
        }
    }



private:
    std::unordered_map<std::string, Animation> animations; // Stores loaded animations
};

extern AnimationManager animationManager;

#endif