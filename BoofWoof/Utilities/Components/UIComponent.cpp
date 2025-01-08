#include <pch.h>
#include "UIComponent.hpp"

void UIComponent::RegisterProperties()
{
	static bool registered = false;
	if (!registered)
	{
		RegisterAllProperties();  // Call the generated RegisterAllProperties function
		registered = true;
	}
}