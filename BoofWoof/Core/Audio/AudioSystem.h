#pragma once

#include <fmod.hpp>
#include <unordered_map>
#include <string>
#include <iostream>
#include "../Utilities/Components/AudioComponent.hpp"
#include "../ECS/Coordinator.hpp"

class AudioSystem : public System {
public:
    AudioSystem();  // Constructor: Initializes FMOD
    ~AudioSystem();  // Destructor: Releases FMOD resources

    // Adds an AudioComponent to an entity and loads the associated audio file
    void AddAudioComponent(Entity entity, const AudioComponent& audioComp);

    // Plays the audio associated with the entity
    void Play(Entity entity);

    // Stops the audio associated with the entity
    void Stop(Entity entity);

    // Updates the FMOD system
    void Update();

    // Play background music (BGM)
    void PlayBGM(const std::string& filePath);

private:
    FMOD::System* system;  // FMOD system object

    // Maps for managing audio data per entity
    std::unordered_map<Entity, FMOD::Sound*> audioMap;
    std::unordered_map<Entity, FMOD::Channel*> channelMap;
    std::unordered_map<Entity, float> volumeMap;

    // Cache for storing loaded sounds keyed by file paths to avoid multiple loads
    std::unordered_map<std::string, FMOD::Sound*> soundCache;
};
