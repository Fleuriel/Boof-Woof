#include "AudioSystem.h"
#include <chrono>
#include <thread>

const char* FMODErrorToString(FMOD_RESULT result) {
    switch (result) {
    case FMOD_OK: return "No errors.";
    case FMOD_ERR_BADCOMMAND: return "FMOD_ERR_BADCOMMAND: The command issued was not supported.";
    case FMOD_ERR_CHANNEL_ALLOC: return "FMOD_ERR_CHANNEL_ALLOC: Error allocating a channel.";
    case FMOD_ERR_CHANNEL_STOLEN: return "FMOD_ERR_CHANNEL_STOLEN: The specified channel has been reused to play another sound.";
    case FMOD_ERR_DMA: return "FMOD_ERR_DMA: DMA Failure.";
    case FMOD_ERR_DSP_CONNECTION: return "FMOD_ERR_DSP_CONNECTION: DSP connection error.";
    case FMOD_ERR_DSP_DONTPROCESS: return "FMOD_ERR_DSP_DONTPROCESS: DSP not processing.";
        // Add more error cases based on the FMOD_RESULT if needed
    default: return "Unknown FMOD error.";
    }
}

// Constructor
AudioSystem::AudioSystem() {
    FMOD::System_Create(&system);  // Initialize FMOD system
    system->init(512, FMOD_INIT_NORMAL, nullptr);  // Initialize system with 512 channels
}

// Destructor
AudioSystem::~AudioSystem() {
    // Stop all channels first
    for (auto& [entity, channels] : channelMap) {
        if (!channels.empty()) {  // Check if the vector is not empty
            for (auto* channel : channels) {
                if (channel) {
                    channel->stop();  // Stop playing sounds
                }
            }
        }
    }
    channelMap.clear();  // Clear the channel map

    // Release all cached sounds
    for (auto& [filePath, sound] : soundCache) {
        sound.reset();  // Reset the shared_ptr to release FMOD::Sound
    }
    soundCache.clear();  // Clear the sound cache to avoid dangling references

    // Close and release FMOD system
    if (system) {
        system->close();
        system->release();
    }
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
        FMOD::Sound* rawSound = nullptr;  // Use a raw pointer initially
        FMOD_RESULT result = system->createSound(filePath.c_str(), FMOD_DEFAULT, nullptr, &rawSound);

        if (result != FMOD_OK) {
            std::cerr << "Error loading sound for entity " << entity << " from file: " << filePath << std::endl;
            return;
        }

        // Wrap the raw FMOD::Sound* in a shared_ptr with a custom deleter
        std::shared_ptr<FMOD::Sound> sound(rawSound, [](FMOD::Sound* s) {});

        // Store the sound in both the entity's audio map and the cache
        audioMap[entity] = sound;
        soundCache[filePath] = sound;  // Cache the sound using shared_ptr to avoid reloading it
        std::cout << "Loaded and cached sound for entity " << entity << " from file: " << filePath << std::endl;
    }

    // Check if the current loop mode is the desired mode before setting it
    FMOD_MODE currentMode;
    audioMap[entity]->getMode(&currentMode);  // Get the current mode

    FMOD_MODE desiredMode = audioComp.ShouldLoop() ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;

    // Only set the mode if it's different from the current mode
    if (currentMode != desiredMode) {
        audioMap[entity]->setMode(desiredMode);  // Set loop mode based on AudioComponent setting
        std::cout << "Updated loop mode for sound: " << filePath << std::endl;
    }

    // Store the volume for the entity
    volumeMap[entity] = audioComp.GetVolume();
}



// Play the audio for the entity with adjustable volume
void AudioSystem::Play(Entity entity, float volume) {
    auto it = audioMap.find(entity);
    if (it != audioMap.end()) {
        FMOD::Channel* channel = nullptr;
        FMOD_RESULT result = system->playSound(it->second.get(), nullptr, false, &channel);  // Play the sound

        if (result != FMOD_OK) {
            std::cerr << "Error playing sound for entity " << entity << ": " << FMODErrorToString(result) << std::endl;
            return;
        }

        // Set the initial volume for the sound
        channel->setVolume(volume);

        // Store the channel to control it later (dynamic volume, stopping, etc.)
        channelMap[entity].push_back(channel);

        std::cout << "Playing sound for entity " << entity << " at volume: " << volume << std::endl;
    }
    else {
        std::cerr << "Audio component not found for entity " << entity << std::endl;
    }
}

// Adjust volume for an entity's active sound channels
void AudioSystem::SetVolume(Entity entity, float volume) {
    auto it = channelMap.find(entity);
    if (it != channelMap.end()) {
        for (auto* channel : it->second) {
            if (channel) {
                channel->setVolume(volume);  // Set volume dynamically
                std::cout << "Volume for entity " << entity << " set to: " << volume << std::endl;
            }
        }
    }
    else {
        std::cerr << "No active channels found for entity " << entity << std::endl;
    }
}


// Stop the audio for the entity
void AudioSystem::Stop(Entity entity) {
    auto it = channelMap.find(entity);
    if (it != channelMap.end()) {
        // Stop all channels associated with the entity
        for (auto* channel : it->second) {
            if (channel) {
                channel->stop();
            }
        }
        // Clear the channel vector after stopping
        channelMap[entity].clear();

        std::cout << "Stopped all sounds for entity " << entity << std::endl;
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

        // Iterate over the channels and remove stopped channels
        auto& channels = channelMap[entity];
        channels.erase(std::remove_if(channels.begin(), channels.end(),
            [](FMOD::Channel* channel) {
                bool isPlaying;
                channel->isPlaying(&isPlaying);
                return !isPlaying;  // Remove the channel if it's no longer playing
            }), channels.end());
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

        // Wrap the raw FMOD::Sound* in a shared_ptr with a custom deleter
        std::shared_ptr<FMOD::Sound> bgmPtr(bgm, [](FMOD::Sound* sound) {
            if (sound) {
                sound->release();  // Release the sound when the shared_ptr is destroyed
            }
            });

        soundCache[filePath] = bgmPtr;  // Cache the BGM sound using the shared_ptr
    }

    // Play the cached BGM sound
    FMOD::Channel* channel = nullptr;
    system->playSound(soundCache[filePath].get(), nullptr, false, &channel);  // Use .get() to retrieve raw pointer
    channel->setVolume(1.0f);  // Set the volume for BGM
    channel->setMode(FMOD_LOOP_NORMAL);  // Ensure BGM loops

    // Update system to handle the audio
    system->update();
}

// Fade in the volume for a sound
void AudioSystem::FadeIn(Entity entity, float targetVolume, float duration) {
    auto it = channelMap.find(entity);
    if (it != channelMap.end()) {
        for (auto* channel : it->second) {
            if (channel) {
                float currentVolume;
                channel->getVolume(&currentVolume);  // Get the current volume

                float step = (targetVolume - currentVolume) / duration;  // Calculate the step to increase volume

                for (float t = 0.0f; t <= duration; t += 0.1f) {  // Increment volume gradually
                    currentVolume += step * 0.1f;
                    channel->setVolume(currentVolume);
                    system->update();
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Delay for smooth fading
                }
                channel->setVolume(targetVolume);  // Ensure we reach the exact target volume
            }
        }
    }
}

// Fade out the volume for a sound
void AudioSystem::FadeOut(Entity entity, float duration) {
    auto it = channelMap.find(entity);
    if (it != channelMap.end()) {
        for (auto* channel : it->second) {
            if (channel) {
                float currentVolume;
                channel->getVolume(&currentVolume);  // Get the current volume

                float step = currentVolume / duration;  // Calculate the step to decrease volume

                for (float t = 0.0f; t <= duration; t += 0.1f) {  // Decrement volume gradually
                    currentVolume -= step * 0.1f;
                    channel->setVolume(std::max(currentVolume, 0.0f));  // Ensure volume doesn't go negative
                    system->update();
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Delay for smooth fading
                }
                channel->setVolume(0.0f);  // Ensure the volume reaches 0
            }
        }
    }
}
