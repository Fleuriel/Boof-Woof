//AudioComponent.cpp
#include <pch.h>
#include "AudioComponent.hpp"
#include "../Core/Audio/AudioSystem.h" 


void AudioComponent::RegisterProperties()
{
    static bool registered = false;
    if (!registered)
    {
        RegisterAllProperties();  // Call the generated RegisterAllProperties function
        registered = true;
    }

}

    void AudioComponent::SetAudioSystem(AudioSystem * audioSystem) {
        m_AudioSystem = audioSystem;
    }

    void AudioComponent::PlayAudio() const {
        if (m_AudioSystem) {
            m_AudioSystem->PlayEntityAudio(m_EntityID, m_FilePath, m_ShouldLoop);
        }
        else {
            std::cerr << "AudioSystem not set for AudioComponent!" << std::endl;
        }
    }


    void AudioComponent::StopAudio() const {
        if (m_AudioSystem) {
            m_AudioSystem->StopEntitySound(m_EntityID);
        }
        else {
            std::cerr << "AudioSystem not set for AudioComponent!" << std::endl;
        }
    }

    void AudioComponent::SetVolume(float volume) const {
        if (m_AudioSystem) {
            m_AudioSystem->SetEntityVolume(m_EntityID, volume);
        }
        else {
            std::cerr << "AudioSystem not set for AudioComponent!" << std::endl;
        }
    }