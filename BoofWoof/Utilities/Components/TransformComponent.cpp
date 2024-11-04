//TransformComponent.cpp
#include <pch.h>
#include "TransformComponent.hpp"

void TransformComponent::RegisterProperties()
{
    static bool registered = false;
    if (!registered)
    {
        RegisterAllProperties();  // Call the generated RegisterAllProperties function
        registered = true;
    }
}
