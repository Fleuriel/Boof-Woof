#include <pch.h>
#include "TransformComponent.hpp"
#include "../Core/Reflection/ReflectionManager.hpp"  // Include the ReflectionManager

// Implement the RegisterProperties function
void TransformComponent::RegisterProperties()
{
    static bool registered = false;
    if (!registered)
    {
        RegisterAllProperties();  // Call the generated RegisterAllProperties function
        registered = true;
    }
}
