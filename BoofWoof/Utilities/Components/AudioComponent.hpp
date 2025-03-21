#pragma once
#include <string>
#include "ECS/Coordinator.hpp"
#include "../Core/Reflection/ReflectionManager.hpp" 

enum class AudioType {
    BGM,  // Background Music
    SFX   // Sound Effect
};

class AudioSystem;

class AudioComponent {
public:
    // Default constructor
    AudioComponent() : m_FilePath(""), m_Volume(1.0f), m_ShouldLoop(false), m_EntityID(0) , m_AudioSystem(nullptr) {}

    // Parameterized constructor

    AudioComponent(const std::string& filePath, float volume, bool shouldLoop, Entity entity, AudioSystem* audioSystem, AudioType type)
        : m_FilePath(filePath), m_Volume(volume), m_ShouldLoop(shouldLoop), m_EntityID(entity), m_AudioSystem(audioSystem), m_Type(type) {}

    void SetType(AudioType type) { m_Type = type; }
    AudioType GetType() const { return m_Type; }

    void SetAudioSystem(AudioSystem* audioSystem);

    void PlayAudio() const;

    void StopAudio() const;
    void SetVolume(float volume) const;



    // Setter for Entity ID
    void SetComponentEntityID(Entity entity) { m_EntityID = entity; }

    // Getters
    std::string GetFilePath() const { return m_FilePath; }
    float GetVolume() const { return m_Volume; }
    bool ShouldLoop() const { return m_ShouldLoop; }

    // Setters
    void SetFilePath(const std::string& filePath) { m_FilePath = filePath; }
    void SetLoop(bool loop) { m_ShouldLoop = loop; }

    // Reflection integration
    REFLECT_COMPONENT(AudioComponent)
    {
        REGISTER_PROPERTY(AudioComponent, FilePath, std::string, SetFilePath, GetFilePath);
        REGISTER_PROPERTY(AudioComponent, Volume, float, SetVolume, GetVolume);
        REGISTER_PROPERTY(AudioComponent, ShouldLoop, bool, SetLoop, ShouldLoop);
    }

private:
    std::string m_FilePath;  // Path to the audio file
    float m_Volume;          // Volume level (0.0 to 1.0)
    bool m_ShouldLoop;       // Whether the sound should loop or not
    Entity m_EntityID;       // Associated entity ID
    AudioSystem* m_AudioSystem; // Pointer to the AudioSystem instance
    AudioType m_Type;        // Type of the audio (BGM or SFX)


};
