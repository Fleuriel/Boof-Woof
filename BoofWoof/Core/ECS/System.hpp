#pragma once

#include "pch.h"

class System
{
public:

	void ChangeEntitySet(std::vector<Entity> rhs) 
	{
		mEntities = rhs;
	}

	std::vector<Entity> mEntities;
};