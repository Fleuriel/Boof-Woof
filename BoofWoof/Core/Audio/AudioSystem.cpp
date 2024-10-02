#include "AudioSystem.h"

// Constructor
AudioSystem::AudioSystem() {
    FMOD::System_Create(&system);  // Initialize FMOD system
    system->init(512, FMOD_INIT_NORMAL, nullptr);  // Initialize system with 512 channels
}

// Destructor
AudioSystem::~AudioSystem() {
    // Release all cached sounds
    for (auto& [filePath, sound] : soundCache) {
        if (sound) {
            sound->release();  // Release each cached sound
        }
    }

    system->close();  // Close FMOD system
    system->release();  // Release FMOD system
}

// Add an audio component to an entity and load its sound (or reuse an already loaded sound)
void AudioSystem::AddAudioComponent(Entity entity, const AudioComponent& audioComp) {
    const std::string& filePath = audioComp.GetFilePath();

    // Check if the sound has already been loaded and cached
    if (soundCache.find(filePath) != soundCache.end()) {
        audioMap[entity] = soundCache[filePath];  // Reuse the sound from the cache
        std::cout << "Reusing sound for entity " << entity << " from file: " << filePath << std::endl;
    }
    else {
        // Load the sound if it's not already in the cache
        FMOD::Sound* sound = nullptr;
        FMOD_RESULT result = system->createSound(filePath.c_str(), FMOD_DEFAULT, nullptr, &sound);

        if (result != FMOD_OK) {
            std::cerr << "Error loading sound for entity " << entity << " from file: " << filePath << std::endl;
            return;
        }

        // Set loop mode based on AudioComponent setting
        sound->setMode(audioComp.ShouldLoop() ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);

        // Store the sound in both the entity's audio map and the cache
        audioMap[entity] = sound;
        soundCache[filePath] = sound;  // Cache the sound to avoid reloading it
        std::cout << "Loaded and cached sound for entity " << entity << " from file: " << filePath << std::endl;
    }

    // Store the volume for the entity
    volumeMap[entity] = audioComp.GetVolume();
}

// Play the audio for the entity
void AudioSystem::Play(Entity entity) {
    auto it = audioMap.find(entity);
    if (it != audioMap.end()) {
        FMOD::Channel* channel = nullptr;
        FMOD_RESULT result = system->playSound(it->second, nullptr, false, &channel);  // Play the sound

        if (result != FMOD_OK) {
            std::cerr << "Error playing sound for entity " << entity << ": " << (result) << std::endl;
            return;
        }

        channel->setVolume(volumeMap[entity]);  // Set the volume for the sound
        channelMap[entity] = channel;  // Store the channel to control it later

        std::cout << "Playing sound for entity " << entity << std::endl;
    }
    else {
        std::cerr << "Audio component not found for entity " << entity << std::endl;
    }
}

// Stop the audio for the entity
void AudioSystem::Stop(Entity entity) {
    auto it = channelMap.find(entity);
    if (it != channelMap.end()) {
        it->second->stop();  // Stop the audio channel
    }
}

// Update the audio system
void AudioSystem::Update() {
    // Loop through all entities that have an AudioComponent
    for (auto const& entity : mEntities) {
        // If the entity doesn't already have a sound in the audioMap, add it
        if (audioMap.find(entity) == audioMap.end() && g_Coordinator.HaveComponent<AudioComponent>(entity)) {
            const auto& audioComp = g_Coordinator.GetComponent<AudioComponent>(entity);
            AddAudioComponent(entity, audioComp);
        }
        // If the entity no longer has an AudioComponent, remove it from the map
        else if (!g_Coordinator.HaveComponent<AudioComponent>(entity)) {
            if (audioMap.find(entity) != audioMap.end()) {
                audioMap[entity]->release();
                audioMap.erase(entity);
            }
        }
    }

    // Update the FMOD system to process audio
    system->update();
}

// Play background music (BGM)
void AudioSystem::PlayBGM(const std::string& filePath) {
    // Check if the BGM sound is already cached
    if (soundCache.find(filePath) == soundCache.end()) {
        // Load BGM as a looping sound
        FMOD::Sound* bgm = nullptr;
        FMOD_RESULT result = system->createSound(filePath.c_str(), FMOD_LOOP_NORMAL, nullptr, &bgm);

        if (result != FMOD_OK) {
            std::cerr << "Error loading BGM from file: " << filePath << std::endl;
            return;
        }

        soundCache[filePath] = bgm;  // Cache the BGM sound
    }

    // Play the cached BGM sound
    FMOD::Channel* channel = nullptr;
    system->playSound(soundCache[filePath], nullptr, false, &channel);
    channel->setVolume(1.0f);  // Set the volume for BGM
    channel->setMode(FMOD_LOOP_NORMAL);  // Ensure BGM loops

    // Update system to handle the audio
    system->update();
}
