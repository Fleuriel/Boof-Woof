//MetaData.cpp
#include <pch.h>
#include "MetaData.hpp"

void MetadataComponent::RegisterProperties()
{
    static bool registered = false;
    if (!registered)
    {
        RegisterAllProperties();  // Call the generated RegisterAllProperties function
        registered = true;
    }
}
