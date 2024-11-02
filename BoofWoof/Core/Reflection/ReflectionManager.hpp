//ReflectionManager.hpp
#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <vector>
#include <type_traits>
#include <glm/glm.hpp>
#include <sstream>
#include <typeindex>

// Base class for ReflectionProperty to enable polymorphism
class ReflectionPropertyBase
{
public:
    virtual ~ReflectionPropertyBase() = default;
    virtual const std::string& GetName() const = 0;
    virtual void SetValue(void* instance, const std::string& value) const = 0;
    virtual std::string GetValue(void* instance) const = 0;

    virtual std::string Serialize(void* instance) const = 0;
    virtual void Deserialize(void* instance, const std::string& serializedValue) const = 0;
};

// Helper functions for glm::vec3 serialization
namespace SerializationHelpers
{
    std::string SerializeVec3(const glm::vec3& vec);
    glm::vec3 DeserializeVec3(const std::string& str);
}

// Template class for ReflectionProperty (must be in the header)
template <typename T, typename FieldType>
class ReflectionProperty : public ReflectionPropertyBase
{
public:
    using SetterFunc = std::function<void(T*, FieldType)>;
    using GetterFunc = std::function<FieldType(T*)>;

    ReflectionProperty(const std::string& name, SetterFunc setter, GetterFunc getter)
        : m_Name(name), m_Setter(setter), m_Getter(getter) {}

    const std::string& GetName() const override { return m_Name; }

    void SetValue(void* instance, const std::string& value) const override
    {
        if constexpr (std::is_same<FieldType, std::string>::value)
        {
            m_Setter(static_cast<T*>(instance), value);
        }
        else if constexpr (std::is_same<FieldType, bool>::value)
        {
            // Special handling for boolean values
            bool convertedValue = (value == "true");
            m_Setter(static_cast<T*>(instance), convertedValue);
        }
        else if constexpr (std::is_arithmetic<FieldType>::value)
        {
            FieldType convertedValue = static_cast<FieldType>(std::stod(value));
            m_Setter(static_cast<T*>(instance), convertedValue);
        }
        else if constexpr (std::is_same<FieldType, glm::vec3>::value)
        {
            glm::vec3 convertedValue = SerializationHelpers::DeserializeVec3(value);
            m_Setter(static_cast<T*>(instance), convertedValue);
        }
        else
        {
            std::cerr << "Unsupported type for reflection property: " << m_Name << std::endl;
        }
    }

    std::string GetValue(void* instance) const override
    {
        if constexpr (std::is_same<FieldType, std::string>::value)
        {
            return m_Getter(static_cast<T*>(instance));
        }
        else if constexpr (std::is_same<FieldType, bool>::value)
        {
            // Special handling for boolean values
            return m_Getter(static_cast<T*>(instance)) ? "true" : "false";
        }
        else if constexpr (std::is_arithmetic<FieldType>::value)
        {
            return std::to_string(m_Getter(static_cast<T*>(instance)));
        }
        else if constexpr (std::is_same<FieldType, glm::vec3>::value)
        {
            return SerializationHelpers::SerializeVec3(m_Getter(static_cast<T*>(instance)));
        }
        else
        {
            return "Unsupported type";
        }
    }

    std::string Serialize(void* instance) const override { return GetValue(instance); }
    void Deserialize(void* instance, const std::string& serializedValue) const override { SetValue(instance, serializedValue); }

private:
    std::string m_Name;
    SetterFunc m_Setter;
    GetterFunc m_Getter;
};


// ReflectionManager class
class ReflectionManager
{
public:
    static ReflectionManager& Instance();

    template <typename T>
    void RegisterProperty(const std::string& className, ReflectionPropertyBase* property)
    {
        m_Properties[className].emplace_back(property);
    }

    const std::vector<ReflectionPropertyBase*>& GetProperties(const std::string& className) const;

    // Register component type
    template <typename T>
    void RegisterComponentType(const std::string& className)
    {
        m_ComponentTypes.emplace(className, []() -> void* { return new T(); });
        m_TypeNames[typeid(T)] = className;
    }

    // Get all registered component types
    const std::unordered_map<std::string, std::function<void* ()>>& GetComponentTypes() const
    {
        return m_ComponentTypes;
    }

    // Get the name of a type
    template <typename T>
    std::string GetTypeName()
    {
        return m_TypeNames[typeid(T)];
    }

private:
    std::unordered_map<std::string, std::vector<ReflectionPropertyBase*>> m_Properties;
    std::unordered_map<std::string, std::function<void* ()>> m_ComponentTypes; // Registered component types
    std::unordered_map<std::type_index, std::string> m_TypeNames;            // Mapping from type to class name
};

#define REGISTER_PROPERTY(ClassType, PropertyName, FieldType, Setter, Getter)          \
    ReflectionManager::Instance().RegisterProperty<ClassType>(                         \
        #ClassType,                                                                    \
        new ReflectionProperty<ClassType, FieldType>(                                  \
            #PropertyName,                                                             \
            [](ClassType* instance, FieldType value) { instance->Setter(value); },     \
            [](ClassType* instance) -> FieldType { return instance->Getter(); }))

#define REFLECT_COMPONENT(ClassType) \
    void RegisterProperties();       \
    static void RegisterAllProperties()
