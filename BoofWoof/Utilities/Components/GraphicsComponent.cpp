//GraphicsComponent.cpp
#include <pch.h>
#include "GraphicsComponent.hpp"
#include "../Core/Reflection/ReflectionManager.hpp"  // Include the ReflectionManager

// Implement the RegisterProperties function
void GraphicsComponent::RegisterProperties()
{
    static bool registered = false;
    if (!registered)
    {
        RegisterAllProperties();  // Call the generated RegisterAllProperties function
        registered = true;
    }
}
