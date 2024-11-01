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

// Template method to serialize a component
template <typename T>
std::string ReflectionManager::SerializeComponent(T* component, const std::string& className)
{
    const auto& properties = GetProperties(className);
    std::ostringstream oss;
    oss << "{";

    for (size_t i = 0; i < properties.size(); ++i)
    {
        const auto& property = properties[i];
        oss << "\"" << property->GetName() << "\":\"" << property->Serialize(component) << "\"";
        if (i < properties.size() - 1)
        {
            oss << ",";
        }
    }

    oss << "}";
    return oss.str();
}

// Template method to deserialize a component
template <typename T>
void ReflectionManager::DeserializeComponent(T* component, const std::string& className, const std::string& serializedData)
{
    const auto& properties = GetProperties(className);

    // Assuming serializedData is a simple JSON-like string: {"PropertyName":"Value",...}
    size_t pos = 0;
    while ((pos = serializedData.find("\"")) != std::string::npos)
    {
        size_t start = pos + 1;
        size_t end = serializedData.find("\"", start);
        std::string propertyName = serializedData.substr(start, end - start);

        // Find the colon and the value
        pos = serializedData.find(":", end);
        start = serializedData.find("\"", pos) + 1;
        end = serializedData.find("\"", start);
        std::string value = serializedData.substr(start, end - start);

        // Set the value using the reflection property
        for (const auto& property : properties)
        {
            if (property->GetName() == propertyName)
            {
                property->Deserialize(component, value);
                break;
            }
        }

        pos = end + 1;
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
