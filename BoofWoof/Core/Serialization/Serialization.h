/**************************************************************************
 * @file Serialization.h
 * @author 	Liu Xujie
 * @param DP email: l.xujie@digipen.edu [2203183]
 * @param Course: CS 3401
 * @param Course: Game Project 3
 * @date  09/22/2024 (22 SEPT 2024)
 * @brief
 *
 * This file contains the Declaration of member functions of Serialization
 * Class
 *************************************************************************/
#ifndef SERIALIZATION_H
#define SERIALIZATION_H

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

    // Generate a new GUID for a scene
    static std::string GenerateGUID();

    // Retrieve the GUID of a loaded scene
    static std::string GetSceneGUID();

    // For storing overlayed scenes
    static std::vector<Entity> GetStored();

public:
    static std::vector<Entity> storedEnt;
    

private:
    static std::string currentSceneGUID;  // To store the GUID for the current scene
};

#endif // SERIALIZATION_H
