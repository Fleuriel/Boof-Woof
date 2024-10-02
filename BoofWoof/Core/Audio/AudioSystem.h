#include <unordered_map>
#include <vector>
#include <string>
#include <fmod.hpp>
#include "../Utilities/Components/AudioComponent.hpp"
#include "../ECS/Coordinator.hpp"
#include <memory>  // For std::shared_ptr

class AudioSystem : public System {
public:
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

private:
    FMOD::System* system;  // FMOD system object
    std::unordered_map<Entity, std::vector<FMOD::Channel*>> channelMap;  // Channels per entity
    std::unordered_map<Entity, std::shared_ptr<FMOD::Sound>> audioMap;  // Sounds per entity
    std::unordered_map<std::string, std::shared_ptr<FMOD::Sound>> soundCache;  // Cached sounds
    std::unordered_map<Entity, float> volumeMap;  // Volume per entity
};
