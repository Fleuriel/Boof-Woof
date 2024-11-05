//GraphicsComponent.cpp
#include <pch.h>
#include "GraphicsComponent.hpp"

void GraphicsComponent::RegisterProperties()
{
    static bool registered = false;
    if (!registered)
    {
        RegisterAllProperties();  // Call the generated RegisterAllProperties function
        registered = true;
    }
}
