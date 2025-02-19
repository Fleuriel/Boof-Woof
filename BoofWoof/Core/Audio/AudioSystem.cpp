/**************************************************************************
 * @file AudioSystem.cpp
 * @author 	Liu Xujie
 * @param DP email: l.xujie@digipen.edu [2203183]
 * @param Course: CS 3401
 * @param Course: Game Project 3
 * @date  09/25/2024 (25 SEPT 2024)
 * @brief
 *
 * This file contains the definitions of member functions of AudioSystem 
 * Class
 *************************************************************************/
#include "AudioSystem.h"
#include "pch.h"
#include <EngineCore.h>
#include "../Core/AssetManager/FilePaths.h"



#define UNREFERENCED_PARAMETER(P)          (P)

FMOD::Channel* currentChannel = nullptr; // Store the current playing channel

 /**************************************************************************
  * @brief Converts an FMOD result code to a human-readable string.
  *
  * This function takes an FMOD result code (FMOD_RESULT) and returns
  * a human-readable string that describes the corresponding FMOD error.
  * It covers common FMOD errors, but will return "Unknown FMOD error" for
  * any unspecified result codes.
  *
  * @param result The FMOD_RESULT code to be converted to a string.
  * @return const char* The human-readable string corresponding to the FMOD error.
  *************************************************************************/

static const char* FMODErrorToString(FMOD_RESULT result) {
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

/**************************************************************************
 * @brief Constructor for the AudioSystem class.
 *
 * This constructor initializes the FMOD system by creating an instance
 * of the FMOD system and setting it up with 512 audio channels.
 * It configures the system with normal initialization settings.
 *
 * @note Make sure to handle any errors that may occur during system
 * creation or initialization.
 *
 * @param None
 * @return None
 *************************************************************************/
AudioSystem::AudioSystem() {
    FMOD::System_Create(&system);  // Initialize FMOD system
    system->init(512, FMOD_INIT_NORMAL, nullptr);  // Initialize system with 512 channels

    // Enable 3D listener settings
    system->set3DSettings(1.0f, 1.0f, 1.0f);  // Doppler scale, distance factor, rolloff scale
    system->set3DNumListeners(1);  // One listener (usually the player or camera)
}

/**************************************************************************
 * @brief Destructor for the AudioSystem class.
 *
 * This destructor stops all active audio channels, clears the channel map,
 * releases all cached sounds, and properly shuts down the FMOD system.
 * It ensures that all FMOD resources, such as sounds and channels,
 * are released before closing and releasing the FMOD system.
 *
 * @note Make sure that all sounds and channels are properly stopped and
 * released to prevent memory leaks.
 *
 * @param None
 * @return None
 *************************************************************************/
AudioSystem::~AudioSystem() {
    // 1. Stop all channels.
    for (auto& [entity, channels] : channelMap) {
        for (auto* channel : channels) {
            if (channel) {
                channel->stop();
            }
        }
    }
    channelMap.clear();

    // 2. Clear any other FMOD–dependent containers.
    additionalChannels.clear();
    channelToFileMap.clear();
    audioMap.clear();
    volumeMap.clear();

    // 3. Release all cached sounds.
    // Clearing the container causes each shared_ptr's destructor to run,
    // which will call the custom deleter (s->release()) while the FMOD system is still valid.
    soundCache.clear();

    // 4. Now safely close and release the FMOD system.
    if (system) {
        system->close();
        system->release();
        system = nullptr;
    }
}



/**************************************************************************
 * @brief Adds an AudioComponent to an entity and manages sound loading.
 *
 * This function adds an audio component to the specified entity, loading
 * the associated sound from a file if it hasn't been cached yet. If the
 * sound has already been cached, it reuses the existing sound to avoid
 * redundant loading. It also sets the loop mode and volume based on
 * the properties of the AudioComponent.
 *
 * @param entity The entity to which the audio component is being added.
 * @param audioComp The AudioComponent containing the audio settings and file path.
 *
 * @note This function ensures that sounds are cached and reloaded efficiently
 * to avoid excessive file I/O operations. It also checks and sets the loop
 * mode only if needed, optimizing performance.
 *
 * @return None
 *************************************************************************/
void AudioSystem::AddAudioComponent(Entity entity, const AudioComponent& audioComp) {
    // Get the original file path from the audio component.
    const std::string& originalFilePath = audioComp.GetFilePath();

    // --- Process the file path to use FILEPATH_ASSET_AUDIO ---
    std::string finalFilePath = originalFilePath;
    std::string key = "Assets/Audio";
    size_t pos = originalFilePath.find(key);
    if (pos != std::string::npos) {
        // Calculate the position after "Assets/Audio"
        size_t offset = pos + key.length();
        // Skip an extra slash if present.
        if (offset < originalFilePath.size() &&
            (originalFilePath[offset] == '/' || originalFilePath[offset] == '\\')) {
            offset++;
        }
        // Extract the relative part (e.g., "mainmenu musicBGM.wav")
        std::string relativePath = originalFilePath.substr(offset);
        // Build the final file path by appending the relative path to FILEPATH_ASSET_AUDIO.
        finalFilePath = FILEPATH_ASSET_AUDIO + "\\" + relativePath;
    }

    // --- Check if the sound has already been loaded and cached ---
    if (soundCache.find(finalFilePath) != soundCache.end()) {
        audioMap[entity] = soundCache[finalFilePath];  // Reuse the sound from the cache
        std::cout << "Reusing sound for entity " << entity << " from file: " << finalFilePath << std::endl;
    }
    else {
        // Load the sound if it's not already in the cache
        FMOD::Sound* rawSound = nullptr;
        FMOD_RESULT result = system->createSound(finalFilePath.c_str(), FMOD_DEFAULT, nullptr, &rawSound);

        if (result != FMOD_OK) {
            std::cerr << "Error loading sound for entity " << entity << " from file: " << finalFilePath << std::endl;
            return;
        }

        // Wrap the raw FMOD::Sound* in a shared_ptr with a custom deleter that calls release()
        std::shared_ptr<FMOD::Sound> sound(rawSound, [](FMOD::Sound* s) { s->release(); });

        // Store the sound in both the entity's audio map and the cache
        audioMap[entity] = sound;
        soundCache[finalFilePath] = sound;  // Cache the sound to avoid reloading it later
        std::cout << "Loaded and cached sound for entity " << entity << " from file: " << finalFilePath << std::endl;
    }

    // --- Check and update the loop mode if needed ---
    FMOD_MODE currentMode;
    audioMap[entity]->getMode(&currentMode);  // Get the current mode

    FMOD_MODE desiredMode = audioComp.ShouldLoop() ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;

    // Only set the mode if it's different from the current mode
    if (currentMode != desiredMode) {
        audioMap[entity]->setMode(desiredMode);  // Set loop mode based on AudioComponent setting
        std::cout << "Updated loop mode for sound: " << finalFilePath << std::endl;
    }

    // --- Store the volume for the entity ---
    volumeMap[entity] = audioComp.GetVolume();
}


/**************************************************************************
 * @brief Plays the audio associated with an entity at a specified volume.
 *
 * This function plays the sound linked to the specified entity by retrieving
 * it from the audio map. If the sound is successfully played, the volume
 * is set to the provided value, and the FMOD channel is stored in the
 * channel map for future control (e.g., adjusting volume or stopping).
 *
 * @param entity The entity for which the sound should be played.
 * @param volume The initial volume level to set for the sound (range: 0.0 to 1.0).
 *
 * @note If the audio component is not found for the entity or if there is an
 * error while playing the sound, an error message will be logged.
 *
 * @return None
 *************************************************************************/
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

/**************************************************************************
 * @brief Sets the volume for the audio associated with an entity.
 *
 * This function dynamically adjusts the volume for all active channels
 * associated with the specified entity. It iterates through all the
 * channels currently playing for the entity and updates their volume
 * to the provided level.
 *
 * @param entity The entity for which the volume should be adjusted.
 * @param volume The new volume level to set (range: 0.0 to 1.0).
 *
 * @note If no active channels are found for the entity, an error message
 * will be logged.
 *
 * @return None
 *************************************************************************/
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


/**************************************************************************
 * @brief Stops all sounds associated with an entity.
 *
 * This function stops all active channels that are currently playing
 * sounds for the specified entity. It iterates through the list of
 * channels for the entity, stopping each one. After all channels
 * are stopped, the channel vector for the entity is cleared.
 *
 * @param entity The entity for which all sounds should be stopped.
 *
 * @note This function ensures that no sounds continue to play after
 * being stopped, and it clears the associated channels for the entity.
 *
 * @return None
 *************************************************************************/
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


/**************************************************************************
 * @brief Updates the AudioSystem by managing audio components and channels.
 *
 * This function is responsible for updating the AudioSystem by checking
 * for entities with or without AudioComponents, adding or removing
 * sounds accordingly. It also removes channels that are no longer playing
 * sounds. Finally, it updates the FMOD system to ensure audio is processed
 * correctly.
 *
 * @note This function ensures that entities are synchronized with their
 * audio components and that channels are cleaned up if their sounds stop playing.
 *
 * @return None
 *************************************************************************/
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


/**************************************************************************
 * @brief Plays background music (BGM) from a specified file.
 *
 * This function plays the background music (BGM) from the provided file path.
 * If the BGM sound is not already cached, it loads the sound as a looping
 * sound and caches it for future use. The cached sound is then played
 * on a new channel, with the volume set to 1.0 and loop mode enabled.
 *
 * @param filePath The path to the BGM file to be played.
 *
 * @note If the BGM file cannot be loaded, an error message is logged,
 * and the function returns without playing any sound.
 *
 * @return None
 *************************************************************************/
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

    FMOD::Channel* channel = nullptr;
    system->playSound(soundCache[filePath].get(), nullptr, false, &channel);  // Use .get() to retrieve raw pointer

    if (channel) {
        channel->setVolume(1.0f);  // Set the volume for BGM
        channel->setMode(FMOD_LOOP_NORMAL);  // Ensure BGM loops
        currentChannel = channel;  // Store the channel globally
    }

    // Update system to handle the audio
    system->update();
}


/**************************************************************************
 * @brief Gradually fades in the audio for a specific entity to a target volume.
 *
 * This function gradually increases the volume of the sound playing on
 * the channels associated with the specified entity, reaching a target
 * volume over a specified duration. The fade-in effect is achieved by
 * incrementally adjusting the volume in small steps over time.
 *
 * @param entity The entity whose audio should be faded in.
 * @param targetVolume The target volume to reach by the end of the fade-in (range: 0.0 to 1.0).
 * @param duration The duration of the fade-in effect in seconds.
 *
 * @note This function assumes that audio is already playing on the entity's channels.
 * It uses a loop to incrementally change the volume, ensuring a smooth fade-in effect.
 *
 * @return None
 *************************************************************************/
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

/**************************************************************************
 * @brief Gradually fades out the audio for a specific entity to silence.
 *
 * This function gradually decreases the volume of the sound playing on
 * the channels associated with the specified entity, reducing the volume
 * to zero over a specified duration. The fade-out effect is achieved by
 * incrementally reducing the volume in small steps over time.
 *
 * @param entity The entity whose audio should be faded out.
 * @param duration The duration of the fade-out effect in seconds.
 *
 * @note This function ensures that the volume never goes below zero during
 * the fade-out. It uses a loop to decrement the volume gradually, ensuring
 * a smooth fade-out effect.
 *
 * @return None
 *************************************************************************/
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

/**************************************************************************
 * @brief Plays a sound file from the given file path, ensuring no overlap.
 *
 * This function plays an audio file from the provided file path. If the
 * audio is already playing on the current channel, it does not trigger
 * the playback again. If the sound is not cached, it loads the sound and
 * caches it for future use. The function also sets the volume for the
 * sound and updates the FMOD system.
 *
 * @param filePath The path to the audio file to be played.
 *
 * @note This function ensures that audio playback does not overlap. It
 * will only trigger playback if the current channel is not already playing.
 * If the sound fails to load or play, an error message is logged.
 *
 * @return None
 *************************************************************************/

void AudioSystem::PlayFile(const std::string& filePath) {
    if (currentChannel) {
        bool isPlaying = false;
        currentChannel->isPlaying(&isPlaying);
        if (isPlaying) {
            // Audio is already playing, so don't trigger PlayFile again
            return;
        }
    }

    // Check if the sound is already cached
    if (soundCache.find(filePath) == soundCache.end()) {
        // Load the sound as it’s not already in the cache
        FMOD::Sound* sound = nullptr;
        FMOD_RESULT result = system->createSound(filePath.c_str(), FMOD_DEFAULT, nullptr, &sound);

        if (result != FMOD_OK) {
            std::cerr << "Error loading sound from file: " << filePath << std::endl;
            return;
        }

        // Cache the sound
        soundCache[filePath] = std::shared_ptr<FMOD::Sound>(sound, [](FMOD::Sound* s) {
            s->release();  // Release the sound when shared_ptr is destroyed
            });
    }

    // Play the cached sound
    FMOD_RESULT result = system->playSound(soundCache[filePath].get(), nullptr, false, &currentChannel);
    if (result != FMOD_OK || !currentChannel) {
        std::cerr << "Error playing sound: " << FMODErrorToString(result) << std::endl;
        return;
    }

    // Set volume and other settings for the channel
    currentChannel->setVolume(1.0f);  // Set to full volume

    // Update the FMOD system to process audio
    system->update();
}

/**************************************************************************
 * @brief Pauses the currently playing background music (BGM).
 *
 * This function pauses the background music (BGM) if it is currently
 * playing. It checks if the current channel is not already paused
 * before pausing the audio.
 *
 * @note This function assumes that a BGM is already playing on the
 * current channel. If the audio is already paused, no action is taken.
 *
 * @return None
 *************************************************************************/

void AudioSystem::PauseBGM() {
    if (currentChannel) {
        bool isPaused = false;
        currentChannel->getPaused(&isPaused);  // Check if already paused

        if (!isPaused) {
            currentChannel->setPaused(true);  // Pause the audio
        }
    }
}

/**************************************************************************
 * @brief Resumes the paused background music (BGM).
 *
 * This function resumes the background music (BGM) if it is currently
 * paused. It checks if the current channel is paused before resuming
 * the audio playback.
 *
 * @note This function assumes that a BGM has been previously paused
 * and can be resumed on the current channel.
 *
 * @return None
 *************************************************************************/

void AudioSystem::ResumeBGM() {
    if (currentChannel) {
        bool isPaused = false;
        currentChannel->getPaused(&isPaused);  // Check if paused

        if (isPaused) {
            currentChannel->setPaused(false);  // Resume the audio
        }
    }
}

/**************************************************************************
 * @brief Checks if the background music (BGM) is currently paused.
 *
 * This function checks whether the background music (BGM) is paused
 * by querying the state of the current audio channel. It returns `true`
 * if the BGM is paused, otherwise returns `false`.
 *
 * @note This function assumes that there is an active channel playing
 * or paused BGM. If no channel is available, it returns `false`.
 *
 * @return bool `true` if the BGM is paused, `false` otherwise.
 *************************************************************************/

bool AudioSystem::IsPaused() const {
    if (currentChannel) {
        bool isPaused = false;
        currentChannel->getPaused(&isPaused);
        return isPaused;
    }
    return false;
}

/**************************************************************************
 * @brief Checks if the background music (BGM) is currently playing.
 *
 * This function checks whether the background music (BGM) is actively
 * playing by querying the state of the current audio channel. It returns
 * `true` if the BGM is playing, otherwise returns `false`.
 *
 * @note This function assumes that there is an active channel for the BGM.
 * If no channel is available, it returns `false`.
 *
 * @return bool `true` if the BGM is playing, `false` otherwise.
 *************************************************************************/
bool AudioSystem::IsPlaying() const {
    if (currentChannel) {
        bool isPlaying = false;
        currentChannel->isPlaying(&isPlaying);
        return isPlaying;
    }
    return false;
}

/**************************************************************************
 * @brief Stops the currently playing background music (BGM).
 *
 * This function stops the background music (BGM) if it is currently playing.
 * It stops the current audio channel and resets the channel pointer to `nullptr`.
 * The FMOD system is then updated to process the stop event.
 *
 * @note This function assumes that there is an active channel playing BGM.
 * If no channel is playing, the function takes no action.
 *
 * @return None
 *************************************************************************/

void AudioSystem::StopBGM() {
    // Stop the current channel if it's playing
    if (currentChannel) {
        currentChannel->stop();
        currentChannel = nullptr;  // Reset the current channel after stopping
    }
    system->update();
}


void AudioSystem::PlayFileOnNewChannel(const std::string& filePath, bool loop, const std::string& soundType) {
    if (soundCache.find(filePath) == soundCache.end()) {
        FMOD::Sound* sound = nullptr;
        FMOD_MODE mode = loop ? FMOD_LOOP_NORMAL : FMOD_DEFAULT;

        FMOD_RESULT result = system->createSound(filePath.c_str(), mode, nullptr, &sound);
        if (result != FMOD_OK) {
            std::cerr << "Error loading sound from file: " << filePath << std::endl;
            return;
        }

        soundCache[filePath] = std::shared_ptr<FMOD::Sound>(sound, [](FMOD::Sound* s) { s->release(); });
    }

    FMOD::Channel* newChannel = nullptr;
    FMOD_RESULT result = system->playSound(soundCache[filePath].get(), nullptr, false, &newChannel);
    if (result != FMOD_OK || !newChannel) {
        std::cerr << "Error playing sound on new channel: " << FMODErrorToString(result) << std::endl;
        return;
    }

    // Set volume based on sound type
    if (soundType == "BGM") {
        newChannel->setVolume(bgmVolume);
    }
    else if (soundType == "SFX") {
        newChannel->setVolume(sfxVolume);
    }

    // Map channel to the file path (instead of sound type)
    channelToFileMap[newChannel] = filePath;

    additionalChannels.push_back(newChannel);
    system->update();
}


void AudioSystem::StopSpecificSound(const std::string& filePath) {
    auto it = channelToFileMap.begin();
    while (it != channelToFileMap.end()) {
        // Check if the file path matches
        if (it->second == filePath) {
            FMOD::Channel* channel = it->first;

            if (channel) {
                bool isPlaying = false;
                FMOD_RESULT result = channel->isPlaying(&isPlaying);
                if (result == FMOD_OK && isPlaying) {
                    channel->stop(); // Stop the specific sound
                }
            }

            // Safely erase the mapping and continue iteration
            it = channelToFileMap.erase(it);
        }
        else {
            ++it; // Move to the next item
        }
    }
}


void AudioSystem::SetBGMVolume(float volume) {
    bgmVolume = volume;

    // Update 2D BGM sounds
    for (auto& [channel, filePath] : channelToFileMap) {
        if (filePath.find("BGM") != std::string::npos) {  // Only update BGM sounds
            if (channel) {
                channel->setVolume(bgmVolume);
            }
        }
    }

    // ✅ NEW: Update 3D BGM sounds only
    for (auto& [entity, channels] : channelMap) {
        if (entitySoundTypeMap[entity] == "BGM") {  // Only update BGM sounds
            for (auto* channel : channels) {
                if (channel) {
                    channel->setVolume(bgmVolume);
                }
            }
        }
    }

    std::cout << "BGM volume set to: " << bgmVolume << " (Updated 2D & 3D BGM)\n";
}


void AudioSystem::SetSFXVolume(float volume) {
    sfxVolume = volume;

    // Update 2D SFX sounds
    for (auto& [channel, filePath] : channelToFileMap) {
        if (filePath.find("BGM") == std::string::npos) {  // Only update SFX sounds
            if (channel) {
                channel->setVolume(sfxVolume);
            }
        }
    }

    for (auto& [entity, channels] : channelMap) {
        if (entitySoundTypeMap[entity] == "SFX") {  // Only update SFX sounds
            for (auto* channel : channels) {
                if (channel) {
                    channel->setVolume(sfxVolume);
                }
            }
        }
    }

    std::cout << "SFX volume set to: " << sfxVolume << " (Updated 2D & 3D SFX)\n";
}





float AudioSystem::GetBGMVolume() const {
    return bgmVolume;
}

float AudioSystem::GetSFXVolume() const {
    return sfxVolume;
}

void AudioSystem::PlayEntityAudio(Entity entity, const std::string& filePath, bool loop) {
    // --- Process the filePath to use FILEPATH_ASSET_AUDIO ---
    // Default to the given filePath
    std::string finalFilePath = filePath;
    // Look for the substring "Assets/Audio" (works with either '/' or '\\')
    std::string key = "Assets/Audio";
    size_t pos = filePath.find(key);
    if (pos != std::string::npos) {
        // Calculate the position right after "Assets/Audio"
        size_t offset = pos + key.length();
        // Skip an extra slash if present
        if (offset < filePath.size() && (filePath[offset] == '/' || filePath[offset] == '\\')) {
            offset++;
        }
        // Extract the relative part (e.g. "mainmenu musicBGM.wav" or "subfolder\\file.wav")
        std::string relativePath = filePath.substr(offset);
        // Build the final file path by appending the relative path to FILEPATH_ASSET_AUDIO.
        // (Using "\\" as the directory separator; adjust as needed for your platform.)
        finalFilePath = FILEPATH_ASSET_AUDIO + "\\" + relativePath;
    }

    // --- Existing channel checking logic ---
    auto it = channelMap.find(entity);
    if (it != channelMap.end()) {
        for (auto* channel : it->second) {
            bool isPlaying = false;
            channel->isPlaying(&isPlaying);
            if (isPlaying) {
                std::cout << "Audio is already playing for entity " << entity << std::endl;
                return; // Prevent duplicate playback
            }
        }
    }

    // --- Load and cache the sound if not already cached ---
    if (soundCache.find(finalFilePath) == soundCache.end()) {
        FMOD::Sound* sound = nullptr;
        FMOD_MODE mode = loop ? FMOD_LOOP_NORMAL : FMOD_DEFAULT;

        FMOD_RESULT result = system->createSound(finalFilePath.c_str(), mode, nullptr, &sound);
        if (result != FMOD_OK) {
            std::cerr << "Error loading sound from file: " << finalFilePath << std::endl;
            return;
        }

        // Cache the sound with a custom deleter that calls release() on the FMOD::Sound pointer
        soundCache[finalFilePath] = std::shared_ptr<FMOD::Sound>(sound, [](FMOD::Sound* s) { s->release(); });
    }

    // --- Play the cached sound ---
    FMOD::Channel* newChannel = nullptr;
    FMOD_RESULT result = system->playSound(soundCache[finalFilePath].get(), nullptr, false, &newChannel);
    if (result != FMOD_OK || !newChannel) {
        std::cerr << "Error playing sound on new channel: " << FMODErrorToString(result) << std::endl;
        return;
    }

    // Set loop count if needed
    if (loop) {
        newChannel->setLoopCount(-1);
    }

    // Store the channel in the entity's channel map
    channelMap[entity].push_back(newChannel);

    // Apply the correct volume (BGM or SFX) based on the file name
    if (finalFilePath.find("BGM") != std::string::npos) {
        newChannel->setVolume(bgmVolume);
        std::cout << "Applied BGM volume: " << bgmVolume << "\n";
    }
    else {
        newChannel->setVolume(sfxVolume);
        std::cout << "Applied SFX volume: " << sfxVolume << "\n";
    }

    // Store channel mapping for future volume updates
    channelToFileMap[newChannel] = finalFilePath;

    // Log the channel association
    std::cout << "Entity " << entity << " now mapped to channel." << std::endl;

    // Update FMOD
    system->update();
}



void AudioSystem::StopEntitySound(Entity entity) {
    auto it = channelMap.find(entity);
    if (it != channelMap.end()) {
        for (auto* channel : it->second) {
            if (channel) {
                bool isPlaying = false;
                channel->isPlaying(&isPlaying);

                if (isPlaying) {
                    channel->stop();
                    std::cout << "Stopped audio for entity " << entity << "." << std::endl;
                }
            }
        }
        channelMap[entity].clear(); // Clear channels after stopping
    }
    else {
        std::cerr << "No active channels found for entity " << entity << std::endl;
    }
}


void AudioSystem::SetEntityVolume(Entity entity, float volume) {
    auto it = channelMap.find(entity);
    if (it != channelMap.end()) {
        for (auto* channel : it->second) {
            if (channel) {
                channel->setVolume(volume);
                std::cout << "Volume for entity " << entity << " set to: " << volume << std::endl;
            }
        }
    }
    else {
        std::cerr << "No active channels found for entity " << entity << std::endl;
    }
}
void AudioSystem::PlayEntity3DAudio(Entity entity, const std::string& filePath, bool loop, const std::string& soundType) {
    auto it = channelMap.find(entity);

    // Prevent duplicate playback
    if (it != channelMap.end()) {
        for (auto* channel : it->second) {
            bool isPlaying = false;
            channel->isPlaying(&isPlaying);
            if (isPlaying) {
                std::cout << "3D " << soundType << " Audio is already playing for entity " << entity << std::endl;
                return;
            }
        }
    }

    // Load and cache the sound
    if (soundCache.find(filePath) == soundCache.end()) {
        FMOD::Sound* sound = nullptr;
        FMOD_MODE mode = FMOD_3D | (loop ? FMOD_LOOP_NORMAL : FMOD_DEFAULT);

        FMOD_RESULT result = system->createSound(filePath.c_str(), mode, nullptr, &sound);
        if (result != FMOD_OK) {
            std::cerr << "Error loading 3D " << soundType << " sound: " << filePath << " - "
                << FMODErrorToString(result) << std::endl;
            return;
        }

        soundCache[filePath] = std::shared_ptr<FMOD::Sound>(sound, [](FMOD::Sound* s) { s->release(); });
    }

    // Play the cached sound
    FMOD::Channel* newChannel = nullptr;
    FMOD_RESULT result = system->playSound(soundCache[filePath].get(), nullptr, false, &newChannel);
    if (result != FMOD_OK || !newChannel) {
        std::cerr << "Error playing 3D " << soundType << " sound: " << FMODErrorToString(result) << std::endl;
        return;
    }

    // Set the loop count
    if (loop) {
        newChannel->setLoopCount(-1);
    }

    // Set 3D position
    if (g_Coordinator.HaveComponent<TransformComponent>(entity)) {
        auto& transform = g_Coordinator.GetComponent<TransformComponent>(entity);
        glm::vec3 position = transform.GetPosition();

        FMOD_VECTOR pos = { position.x, position.y, position.z };
        newChannel->set3DAttributes(&pos, nullptr);
    }

    // 🔹 Store the sound type (SFX or BGM) for this entity
    entitySoundTypeMap[entity] = soundType;

    // 🔊 Apply correct volume
    if (soundType == "BGM") {
        newChannel->setVolume(bgmVolume);
    }
    else {
        newChannel->setVolume(sfxVolume);
    }

    // Store the channel
    channelMap[entity].push_back(newChannel);

    std::cout << "3D " << soundType << " Audio for entity " << entity << " now mapped to channel." << std::endl;

    system->update();
}


void AudioSystem::Update3DSoundPositions() {
    for (auto& [entity, channels] : channelMap) {
        if (g_Coordinator.HaveComponent<TransformComponent>(entity)) {
            auto& transform = g_Coordinator.GetComponent<TransformComponent>(entity);
            glm::vec3 position = transform.GetPosition();  // ✅ Correct function call

            FMOD_VECTOR pos = { position.x, position.y, position.z };

            for (auto* channel : channels) {
                if (channel) {
                    channel->set3DAttributes(&pos, nullptr);
                }
            }
        }
    }
}

void AudioSystem::SetListenerPosition(const glm::vec3& position, const glm::vec3& rotation) {
    FMOD_VECTOR listenerPos = { position.x, position.y, position.z };

    // Convert rotation (Euler angles) into a forward vector
    glm::vec3 forward = {
        cos(glm::radians(rotation.y)) * cos(glm::radians(rotation.x)),
        sin(glm::radians(rotation.x)),
        sin(glm::radians(rotation.y)) * cos(glm::radians(rotation.x))
    };

    glm::vec3 up = { 0.0f, 1.0f, 0.0f }; // Assume up is always (0,1,0)

    FMOD_VECTOR fmodForward = { forward.x, forward.y, forward.z };
    FMOD_VECTOR fmodUp = { up.x, up.y, up.z };

    system->set3DListenerAttributes(0, &listenerPos, nullptr, &fmodForward, &fmodUp);
}
