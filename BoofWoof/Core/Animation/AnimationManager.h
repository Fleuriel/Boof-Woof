#ifndef ANIMATIONMANAGER_H
#define ANIMATIONMANAGER_H

#include "pch.h"
#include "../Utilities/Components/AnimationComponent.h"
#include "EntityAnimator.h"

class AnimationManager {
public:
    static AnimationManager& getInstance() {
        static AnimationManager instance;
        return instance;
    }

    // Delete copy constructor and assignment operator
    AnimationManager(const AnimationManager&) = delete;
    AnimationManager& operator=(const AnimationManager&) = delete;

    // Core functions
    void initialize();
    void shutdown();
    void update(float deltaTime);

    // Animation management
    bool loadAnimation(const std::string& filename, const std::string& animName = "");
    AnimationClip* getAnimation(const std::string& name);
    void unloadAnimation(const std::string& name);
    void unloadAllAnimations();

    // Animation pool management
    void preloadAnimation(const std::string& filename);
    void purgeUnusedAnimations();

    // Settings
    void setGlobalAnimationSpeed(float speed) { m_globalSpeedMultiplier = speed; }
    float getGlobalAnimationSpeed() const { return m_globalSpeedMultiplier; }

    // Debug/Statistics
    size_t getLoadedAnimationCount() const { return m_animationLibrary.size(); }
    size_t getTotalMemoryUsage() const;
    void printStatistics() const;

    // Create a new entity animator
    std::shared_ptr<EntityAnimator> createEntityAnimator(const std::string& entityId) {
        auto animator = std::make_shared<EntityAnimator>(entityId);
        m_entityAnimators[entityId] = animator;
        return animator;
    }

    // Get an existing entity animator
    std::shared_ptr<EntityAnimator> getEntityAnimator(const std::string& entityId) {
        auto it = m_entityAnimators.find(entityId);
        return it != m_entityAnimators.end() ? it->second : nullptr;
    }

    // Add an animation to an entity
    void addAnimationToEntity(const std::string& entityId, const std::string& animName) {
        auto animator = getEntityAnimator(entityId);
        auto clip = getAnimation(animName);
        if (animator && clip) {
            animator->addAnimation(animName, clip);
        }
    }

    std::vector<std::string> getAnimationNames() const {
        std::vector<std::string> names;
        for (const auto& [name, clip] : m_animationLibrary) {
            names.push_back(name);
        }
        return names;
    }


    AnimationManager() : m_globalSpeedMultiplier(1.0f) {}
    ~AnimationManager() { shutdown(); }
private:

    // Internal helper functions
    bool processAnimationFile(const std::string& filename, const std::string& animName);
    void cleanupResources();

    // Storage
    std::unordered_map<std::string, std::unique_ptr<AnimationClip>> m_animationLibrary;
    std::vector<std::weak_ptr<AnimationComponent>> m_activeComponents;

    // Settings
    float m_globalSpeedMultiplier;

    // Assimp importer (kept as member to prevent reallocation)
    Assimp::Importer m_importer;

    std::unordered_map<std::string, std::shared_ptr<EntityAnimator>> m_entityAnimators;
};

extern AnimationManager g_AnimationManager;

#endif // !1