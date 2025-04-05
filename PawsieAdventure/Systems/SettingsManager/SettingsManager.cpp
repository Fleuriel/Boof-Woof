#include "SettingsManager.h"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/stringbuffer.h>
#include <fstream>
#include <iostream>

extern std::shared_ptr<GraphicsSystem> mGraphicsSys; // For accessing gamma value.

namespace SettingsManager {

    void SaveSettingsFromEngine()
    {
        // Read current settings from the engine
        float sfxVolume = g_Audio.GetSFXVolume();
        float bgmVolume = g_Audio.GetBGMVolume();
        float masterVolume = g_Audio.GetMasterVolume();
        float gammaValue = mGraphicsSys->gammaValue; // Assumes gammaValue is public

        // Create a RapidJSON document and mark it as an object
        rapidjson::Document document;
        document.SetObject();
        rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

        // Add settings values to the document
        document.AddMember("SFXVolume", sfxVolume, allocator);
        document.AddMember("BGMVolume", bgmVolume, allocator);
        document.AddMember("MasterVolume", masterVolume, allocator);
        document.AddMember("GammaValue", gammaValue, allocator);

        // Serialize the document to a JSON string
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        document.Accept(writer);

        // Write the JSON string to a file named "settings.json"
        std::ofstream ofs("settings.json");
        if (!ofs.is_open())
        {
            std::cerr << "Failed to open settings.json for writing." << std::endl;
            return;
        }
        ofs << buffer.GetString();
        ofs.close();

        std::cout << "Settings saved successfully." << std::endl;
    }

    void LoadSettingsToEngine()
    {
        // Open the settings file for reading
        std::ifstream ifs("settings.json");
        if (!ifs.is_open())
        {
            std::cerr << "Failed to open settings.json for reading." << std::endl;
            return;
        }

        // Wrap the input stream for RapidJSON
        rapidjson::IStreamWrapper isw(ifs);
        rapidjson::Document document;
        document.ParseStream(isw);
        if (document.HasParseError())
        {
            std::cerr << "Error parsing settings.json." << std::endl;
            return;
        }

        if (document.HasMember("SFXVolume") && document["SFXVolume"].IsNumber())
        {
            float sfxVolume = document["SFXVolume"].GetFloat();
            g_Audio.SetSFXVolume(sfxVolume);
        }
        else
        {
            std::cerr << "SFXVolume not found or invalid in settings.json." << std::endl;
        }

        if (document.HasMember("BGMVolume") && document["BGMVolume"].IsNumber())
        {
            float bgmVolume = document["BGMVolume"].GetFloat();
            g_Audio.SetBGMVolume(bgmVolume);
        }
        else
        {
            std::cerr << "BGMVolume not found or invalid in settings.json." << std::endl;
        }

        if (document.HasMember("MasterVolume") && document["MasterVolume"].IsNumber())
        {
            float masterVolume = document["MasterVolume"].GetFloat();
            g_Audio.SetMasterVolume(masterVolume);
        }
        else
        {
            std::cerr << "MasterVolume not found or invalid in settings.json." << std::endl;
        }

        if (document.HasMember("GammaValue") && document["GammaValue"].IsNumber())
        {
            float gammaValue = document["GammaValue"].GetFloat();
            mGraphicsSys->gammaValue = gammaValue;
        }
        else
        {
            std::cerr << "GammaValue not found or invalid in settings.json." << std::endl;
        }

        std::cout << "Settings loaded successfully." << std::endl;
    }

} // namespace SettingsManager
