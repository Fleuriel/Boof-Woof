#include "pch.h"

#ifdef APIENTRY
#undef APIENTRY
#endif

#include "FilePaths.h"
#include <iostream>
#include <windows.h> // For MultiByteToWideChar

namespace FilePaths
{
    // Define narrow string paths
    std::string ExecutableDirectory;
    std::string AssetPath;
    std::string DescriptorPath;
    std::string ResourcePath;
    bool GameCheck = false;

    // Define wide string paths
    std::wstring ExecutableDirectoryW;
    std::wstring AssetPathW;
    std::wstring DescriptorPathW;
    std::wstring ResourcePathW;

    // Helper function to convert std::string to std::wstring using UTF-8 encoding
    std::wstring StringToWString(const std::string& s)
    {
        if (s.empty()) return std::wstring();

        int size_needed = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), NULL, 0);
        if (size_needed <= 0)
        {
            // Handle error
            std::cerr << "Error converting string to wstring.\n";
            return std::wstring();
        }

        std::wstring wstrTo(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), &wstrTo[0], size_needed);
        return wstrTo;
    }

    void FilePaths::InitNarrowPaths()
    {
        using namespace std::filesystem;

        // Get the current executable directory
        ExecutableDirectory = current_path().string();

        // Paths in the executable directory
        std::string exeAssets = ExecutableDirectory + "\\Assets";
        std::string exeDescriptors = ExecutableDirectory + "\\Descriptors";
        std::string exeResources = ExecutableDirectory + "\\Resources";

        // Check if the paths exist
        if (exists(exeAssets) && exists(exeDescriptors) && exists(exeResources))
        {
            AssetPath = exeAssets;
            DescriptorPath = exeDescriptors;
            ResourcePath = exeResources;
            GameCheck = true;
            std::cout << "Assets Path: " << exeAssets << std::endl;
            std::cout << "Descriptors Path: " << exeDescriptors << std::endl;
            std::cout << "Resources Path: " << exeResources << std::endl;
        }
        else
        {
            // Fallback to default hardcoded paths
            AssetPath = "..\\BoofWoof\\Assets";
            DescriptorPath = "..\\BoofWoof\\Descriptors";
            ResourcePath = "..\\BoofWoof\\Resources";

            std::cout << "Using fallback paths:" << std::endl;
            std::cout << "Assets Path: " << AssetPath << std::endl;
            std::cout << "Descriptors Path: " << DescriptorPath << std::endl;
            std::cout << "Resources Path: " << ResourcePath << std::endl;
        }
    }

    void FilePaths::InitWidePaths()
    {
        // Convert narrow string paths to wide string paths
        ExecutableDirectoryW = StringToWString(ExecutableDirectory);
        AssetPathW = StringToWString(AssetPath);
        DescriptorPathW = StringToWString(DescriptorPath);
        ResourcePathW = StringToWString(ResourcePath);
    }

    void FilePaths::InitPaths()
    {
        InitNarrowPaths();
        InitWidePaths();
    }
}
