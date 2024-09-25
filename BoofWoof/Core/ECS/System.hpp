#pragma once

#include "pch.hpp"

class System
{
public:

	void ChangeEntitySet(std::vector<Entity> rhs) 
	{
		mEntities = rhs;
	}

	std::vector<Entity> mEntities;
};