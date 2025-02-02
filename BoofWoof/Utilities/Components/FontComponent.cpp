#include <pch.h>
#include "FontComponent.hpp"

void FontComponent::RegisterProperties()
{
	static bool registered = false;
	if (!registered)
	{
		RegisterAllProperties();  // Call the generated RegisterAllProperties function
		registered = true;
	}
}