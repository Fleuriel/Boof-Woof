//AudioComponent.cpp
#include <pch.h>
#include "AudioComponent.hpp"

void AudioComponent::RegisterProperties()
{
    static bool registered = false;
    if (!registered)
    {
        RegisterAllProperties();  // Call the generated RegisterAllProperties function
        registered = true;
    }
}
