#pragma once
#include <EngineCore.h>
#include <string>

namespace SettingsManager {

    // Saves the current engine settings to a JSON file.
    void SaveSettingsFromEngine();

    void LoadSettingsToEngine();
}
