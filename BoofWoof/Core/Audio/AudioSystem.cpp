#include "AudioSystem.h"

// Constructor
AudioSystem::AudioSystem() {
    FMOD::System_Create(&system);  // Initialize FMOD system
    system->init(512, FMOD_INIT_NORMAL, nullptr);  // Initialize system with 512 channels
}

// Destructor
AudioSystem::~AudioSystem() {
    for (auto& [entity, sound] : audioMap) {
        if (sound) {
            sound->release();  // Release each sound
        }
    }
    system->close();  // Close FMOD system
    system->release();  // Release FMOD system
}

// Add an audio component to an entity and load its sound
void AudioSystem::AddAudioComponent(Entity entity, const AudioComponent& audioComp) {
    // If sound already exists for this entity, release it
    if (audioMap.find(entity) != audioMap.end()) {
        audioMap[entity]->release();
    }

    FMOD::Sound* sound = nullptr;
    FMOD_RESULT result = system->createSound(audioComp.GetFilePath().c_str(), FMOD_DEFAULT, nullptr, &sound);

    if (result != FMOD_OK) {
        std::cerr << "Error loading sound for entity " << entity << " from file: " << audioComp.GetFilePath() << std::endl;
        return;
    }

    // Set loop mode based on AudioComponent setting
    sound->setMode(audioComp.ShouldLoop() ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);

    // Store the sound and volume in the maps
    audioMap[entity] = sound;
    volumeMap[entity] = audioComp.GetVolume();
}

// Play the audio for the entity
void AudioSystem::Play(Entity entity) {
    auto it = audioMap.find(entity);
    if (it != audioMap.end()) {
        FMOD::Channel* channel = nullptr;
        system->playSound(it->second, nullptr, false, &channel);  // Play the sound
        channel->setVolume(volumeMap[entity]);  // Set volume level
        channelMap[entity] = channel;  // Store the channel for future control
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
    system->update();  // Update FMOD system
}

void AudioSystem::PlayBGM(const std::string& filePath) {
    // Load BGM as a looping sound
    FMOD::Sound* bgm = nullptr;
    FMOD_RESULT result = system->createSound(filePath.c_str(), FMOD_LOOP_NORMAL, nullptr, &bgm);

    if (result != FMOD_OK) {
        std::cerr << "Error loading BGM from file: " << filePath << std::endl;
        return;
    }

    // Store BGM and play
    FMOD::Channel* channel = nullptr;
    system->playSound(bgm, nullptr, false, &channel);
    channel->setVolume(1.0f);  // Set the volume for BGM
    channel->setMode(FMOD_LOOP_NORMAL);  // Ensure BGM loops

    // Update system to handle the audio
    system->update();
}
