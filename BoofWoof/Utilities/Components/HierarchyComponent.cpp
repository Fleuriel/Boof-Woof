#include <pch.h>
#include "HierarchyComponent.hpp"

void HierarchyComponent::RegisterProperties()
{
    static bool registered = false;
    if (!registered)
    {
        RegisterAllProperties();  // Call the generated RegisterAllProperties function
        registered = true;
    }
}
