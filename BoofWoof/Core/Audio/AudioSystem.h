/**************************************************************************
 * @file AudioSystem.cpp
 * @author 	Liu Xujie
 * @param DP email: l.xujie@digipen.edu [2203183]
 * @param Course: CS 3401
 * @param Course: Game Project 3
 * @date  09/25/2024 (25 SEPT 2024)
 * @brief
 *
 * This file contains the declaration of member functions of AudioSystem
 * Class
 *************************************************************************/
#include <fmod.hpp>
#include "../Utilities/Components/AudioComponent.hpp"
#include "ECS/Coordinator.hpp"


#define g_Audio AudioSystem::GetInstance()

class AudioSystem : public System {
public:

    static AudioSystem& GetInstance() {
        static AudioSystem instance;
        return instance;
    }

    AudioSystem();  // Constructor
    ~AudioSystem();  // Destructor

    void AddAudioComponent(Entity entity, const AudioComponent& audioComp);
    void Play(Entity entity, float volume = 1.0f);
    void Stop(Entity entity);
    void Update();
    void PlayBGM(const std::string& filePath);
    // Adjust volume for an entity's active sound channels
    void SetVolume(Entity entity, float volume);
    //utility functions
    // Fade in the volume for a sound
    void FadeIn(Entity entity, float targetVolume, float duration);

    // Fade out the volume for a sound
    void FadeOut(Entity entity, float duration);

    void PlayFile(const std::string& filePath);

    void StopBGM();

    void PauseBGM();   // Function to pause the current audio
    void ResumeBGM();  // Function to resume the current audio
    bool IsPaused() const;  // Check if the audio is currently paused
    bool IsPlaying() const; // Check if audio is playing

    void PlayFileOnNewChannel(const std::string& filePath, bool loop, const std::string& soundType);
        void StopSpecificSound(const std::string& filePath);

    void SetBGMVolume(float volume);
    void SetSFXVolume(float volume);
    float GetBGMVolume() const;
    float GetSFXVolume() const;

    void PlayEntityAudio(Entity entity, const std::string& filePath, bool loop);
    void StopEntitySound(Entity entity);
    void SetEntityVolume(Entity entity, float volume);

    void PlayEntity3DAudio(Entity entity, const std::string& filePath, bool loop, const std::string& soundType);
    void Update3DSoundPositions();
    void SetListenerPosition(const glm::vec3& position, const glm::vec3& rotation);
    void SetSoundVolume(const std::string& filePath, float volume);
    void SetMasterVolume(float volume);
    float GetMasterVolume() const;



private:
    FMOD::System* system;  // FMOD system object
    std::unordered_map<Entity, std::vector<FMOD::Channel*>> channelMap;  // Channels per entity
    std::unordered_map<Entity, std::shared_ptr<FMOD::Sound>> audioMap;  // Sounds per entity
    std::unordered_map<std::string, std::shared_ptr<FMOD::Sound>> soundCache;  // Cached sounds
    std::unordered_map<Entity, float> volumeMap;  // Volume per entity
    std::vector<FMOD::Channel*> additionalChannels;
    std::unordered_map<FMOD::Channel*, std::string> channelToFileMap;
    float bgmVolume = 1.0f;  // Default volume for BGM
    float sfxVolume = 1.0f;  // Default volume for SFX
    float masterVolume = 1.0f; // New master volume (default 100%)

    std::unordered_map<Entity, std::string> entitySoundTypeMap;




};
