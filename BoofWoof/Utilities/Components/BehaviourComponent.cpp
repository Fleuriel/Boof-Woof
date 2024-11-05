#include <pch.h>
#include "BehaviourComponent.hpp"

void BehaviourComponent::RegisterProperties()
{
    static bool registered = false;
    if (!registered)
    {
        RegisterAllProperties();  // Call the generated RegisterAllProperties function
        registered = true;
    }
}
