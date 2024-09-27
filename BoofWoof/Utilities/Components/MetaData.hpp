#pragma once
#ifndef METADATA_COMPONENT_H
#define METADATA_COMPONENT_H

#include "ECS/Coordinator.hpp"

class MetadataComponent
{
public:

	MetadataComponent() { }
	MetadataComponent(std::string name, Entity& entity)
		: m_Name(name), m_EntityID(g_Coordinator.GetEntityId(entity)) { /*empty by design*/ }

	void SetComponentEntityID(Entity& entity) { m_EntityID = entity; }

	const std::string& GetName() const { return m_Name; }
	void SetName(const std::string& newName) { m_Name = newName; }

private:
	Entity m_EntityID{};
	std::string m_Name{};
};

#endif  // METADATA_COMPONENT_H
