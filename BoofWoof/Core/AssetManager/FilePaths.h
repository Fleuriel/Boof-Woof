#ifndef FILEPATHS_H
#define FILEPATHS_H

#include <string>
#include <filesystem>

namespace FilePaths
{
    // Flags and path variables
    extern bool GameCheck;

    // Narrow string paths (std::string)
    extern std::string ExecutableDirectory;
    extern std::string AssetPath;
    extern std::string DescriptorPath;
    extern std::string ResourcePath;

    // Wide string paths (std::wstring)
    extern std::wstring ExecutableDirectoryW;
    extern std::wstring AssetPathW;
    extern std::wstring DescriptorPathW;
    extern std::wstring ResourcePathW;

    // Initialization functions
    void InitPaths();              // Initializes both narrow and wide paths
    void InitWidePaths();          // Initializes only wide paths
    void InitNarrowPaths();        // Initializes only narrow paths
}

// Fallback paths for narrow strings
#define FILEPATH_ASSETS                          (FilePaths::AssetPath)
#define FILEPATH_ASSET_ART                      (FilePaths::AssetPath + "\\Art")
#define FILEPATH_ASSET_TEXTURES                 (FilePaths::AssetPath + "\\Art\\Textures")
#define FILEPATH_ASSET_SPRITES                  (FilePaths::AssetPath + "\\Art\\Sprites")
#define FILEPATH_ASSET_SCENES                   (FilePaths::AssetPath + "\\Scenes")
#define FILEPATH_ASSET_OBJECTS                  (FilePaths::AssetPath + "\\Objects")
#define FILEPATH_ASSET_SHADERS                  (FilePaths::AssetPath + "\\Shaders")
#define FILEPATH_ASSET_FONTS                    (FilePaths::AssetPath + "\\Fonts")
#define FILEPATH_ASSET_AUDIO                    (FilePaths::AssetPath + "\\Audio")
#define FILEPATH_ASSET_MATERIAL                 (FilePaths::AssetPath + "\\Material")
#define FILEPATH_ASSET_ANIMATIONS               (FilePaths::AssetPath + "\\Animations")
#define FILEPATH_ASSET_TRASHBIN                 (FilePaths::AssetPath + "\\Trashbin")

#define FILEPATH_DESCRIPTORS                    (FilePaths::DescriptorPath)
#define FILEPATH_DESCRIPTOR_TEXTURES            (FilePaths::DescriptorPath + "\\Textures")
#define FILEPATH_DESCRIPTOR_FONTS               (FilePaths::DescriptorPath + "\\Fonts")
#define FILEPATH_DESCRIPTOR_OBJECTS             (FilePaths::DescriptorPath + "\\Objects")
#define FILEPATH_DESCRIPTOR_MATERIAL            (FilePaths::DescriptorPath + "\\Material")

#define FILEPATH_RESOURCES                      (FilePaths::ResourcePath)
#define FILEPATH_RESOURCE_TEXTURES              (FilePaths::ResourcePath + "\\Textures")
#define FILEPATH_RESOURCE_OBJECTS               (FilePaths::ResourcePath + "\\Objects")
#define FILEPATH_RESOURCE_FONTS                 (FilePaths::ResourcePath + "\\Fonts")
#define FILEPATH_RESOURCE_MATERIAL              (FilePaths::ResourcePath + "\\Material")

// Fallback paths for wide strings (suffix _W)
#define FILEPATH_ASSETS_W                        (FilePaths::AssetPathW)
#define FILEPATH_ASSET_ART_W                    (FilePaths::AssetPathW + L"\\Art")
#define FILEPATH_ASSET_TEXTURES_W               (FilePaths::AssetPathW + L"\\Art\\Textures")
#define FILEPATH_ASSET_SPRITES_W                (FilePaths::AssetPathW + L"\\Art\\Sprites")
#define FILEPATH_ASSET_SCENES_W                 (FilePaths::AssetPathW + L"\\Scenes")
#define FILEPATH_ASSET_OBJECTS_W                (FilePaths::AssetPathW + L"\\Objects")
#define FILEPATH_ASSET_SHADERS_W                (FilePaths::AssetPathW + L"\\Shaders")
#define FILEPATH_ASSET_FONTS_W                  (FilePaths::AssetPathW + L"\\Fonts")
#define FILEPATH_ASSET_AUDIO_W                  (FilePaths::AssetPathW + L"\\Audio")
#define FILEPATH_ASSET_MATERIAL_W               (FilePaths::AssetPathW + L"\\Material")
#define FILEPATH_ASSET_ANIMATIONS_W             (FilePaths::AssetPathW + L"\\Animations")
#define FILEPATH_ASSET_TRASHBIN_W               (FilePaths::AssetPathW + L"\\Trashbin")

#define FILEPATH_DESCRIPTORS_W                  (FilePaths::DescriptorPathW)
#define FILEPATH_DESCRIPTOR_TEXTURES_W          (FilePaths::DescriptorPathW + L"\\Textures")
#define FILEPATH_DESCRIPTOR_FONTS_W             (FilePaths::DescriptorPathW + L"\\Fonts")
#define FILEPATH_DESCRIPTOR_OBJECTS_W           (FilePaths::DescriptorPathW + L"\\Objects")
#define FILEPATH_DESCRIPTOR_MATERIAL_W          (FilePaths::DescriptorPathW + L"\\Material")

#define FILEPATH_RESOURCES_W                    (FilePaths::ResourcePathW)
#define FILEPATH_RESOURCE_TEXTURES_W            (FilePaths::ResourcePathW + L"\\Textures")
#define FILEPATH_RESOURCE_OBJECTS_W             (FilePaths::ResourcePathW + L"\\Objects")
#define FILEPATH_RESOURCE_FONTS_W               (FilePaths::ResourcePathW + L"\\Fonts")
#define FILEPATH_RESOURCE_MATERIAL_W            (FilePaths::ResourcePathW + L"\\Material")

#endif // FILEPATHS_H
