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
            m_AudioSystem->PlayFileOnNewChannel(m_FilePath, m_ShouldLoop);
        }
        else {
            std::cerr << "AudioSystem not set for AudioComponent!" << std::endl;
        }
    }

    void AudioComponent::StopAudio() const {
        if (m_AudioSystem) {
            m_AudioSystem->StopSpecificSound(m_FilePath);
        }
        else {
            std::cerr << "AudioSystem not set for AudioComponent!" << std::endl;
        }
    }