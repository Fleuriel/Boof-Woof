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

#define g_Json Serialization::GetInstance()

class Serialization {
public:

	static Serialization& GetInstance() {
		static Serialization instance;
		return instance;
	}

    static void SaveEngineState(const std::string& filepath);
    static void LoadEngineState(const std::string& filepath);
};

#endif // SERIALIZATION_H
