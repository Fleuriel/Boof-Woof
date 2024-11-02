//ReflectionManager.cpp
#include <pch.h>
#include "ReflectionManager.hpp"

// SerializationHelpers implementation for glm::vec3
namespace SerializationHelpers
{
    std::string SerializeVec3(const glm::vec3& vec)
    {
        std::ostringstream oss;
        oss << vec.x << "," << vec.y << "," << vec.z;
        return oss.str();
    }

    glm::vec3 DeserializeVec3(const std::string& str)
    {
        glm::vec3 vec;
        std::istringstream iss(str);
        char comma;
        iss >> vec.x >> comma >> vec.y >> comma >> vec.z;
        return vec;
    }
}

// Define non-template methods of ReflectionManager
ReflectionManager& ReflectionManager::Instance()
{
    static ReflectionManager instance;
    return instance;
}

const std::vector<ReflectionPropertyBase*>& ReflectionManager::GetProperties(const std::string& className) const
{
    static const std::vector<ReflectionPropertyBase*> empty;
    auto it = m_Properties.find(className);
    return it != m_Properties.end() ? it->second : empty;
}
