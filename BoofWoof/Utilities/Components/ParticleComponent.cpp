#include <pch.h>
#include "ParticleComponent.hpp"


void ParticleComponent::RegisterProperties()
{
    static bool registered = false;
    if (!registered)
    {
        RegisterAllProperties();  // Call the generated RegisterAllProperties function
        registered = true;
    }
}