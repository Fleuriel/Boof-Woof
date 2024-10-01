#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>  // C++17 and later


// Disable warnings for MSVC
#ifdef _MSC_VER
#pragma warning(push, 0) // Temporarily disable all warnings
#endif

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/prettywriter.h>

// Re-enable warnings after including RapidJSON
#ifdef _MSC_VER
#pragma warning(pop)
#endif


class Serialization {
public:
    static bool SaveScene(const std::string& filepath);
    static bool LoadScene(const std::string& filepath);
};

#endif // SERIALIZATION_H
