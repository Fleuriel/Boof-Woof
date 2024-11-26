#include <pch.h>
#include "LightComponent.hpp"


void LightComponent::RegisterProperties()
{
    static bool registered = false;
    if (!registered)
    {
        RegisterAllProperties();  // Call the generated RegisterAllProperties function
        registered = true;
    }
}