//MetaData.cpp
#include <pch.h>
#include "MetaData.hpp"
#include "../Core/Reflection/ReflectionManager.hpp"  // Include the ReflectionManager

// Implement the RegisterProperties function
void MetadataComponent::RegisterProperties()
{
    static bool registered = false;
    if (!registered)
    {
        RegisterAllProperties();  // Call the generated RegisterAllProperties function
        registered = true;
    }
}
