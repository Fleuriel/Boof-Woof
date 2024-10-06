/**************************************************************************
 * @file AssetManager.cpp
 * @author 	Aaron Chan Jun Xiang
 * @param DP email: aaronjunxiang.chan@digipen.edu [2200880]
 * @param Course: CS 3401
 * @param Course: Game Project 3
 * @date  10/06/2024 (06 OCTOBER 2024)
 * @brief
 *
 * This file loads assets to be used
 *
 *************************************************************************/

#define MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS 0

#pragma warning(push)
#pragma warning(disable: 4244)
#pragma warning(disable: 4005)  // Disable macro redefinition warning

#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <filesystem>
#include <cstdlib>      // for system command
#include <random>       // For GUID generation

#include "AssetManager.h"
#include "windows.h"
#include "FilePaths.h"
#include "Compressonator.h"
#include "ResourceManager/ResourceManager.h"

#pragma warning(pop)

AssetManager g_AssetManager;

namespace fs = std::filesystem;


/**************************************************************************
 * @brief Converts a string to lowercase.
 *
 * This function takes an input string and converts all of its characters
 * to lowercase, returning the modified string.
 *
 * @param input The input string to be converted to lowercase.
 * @return std::string The resulting string where all characters are in
           lowercase.
 *************************************************************************/
std::string toLowerCase(const std::string& input) {
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
    return result;
}

/**************************************************************************
 * @brief Default Constructor
 *************************************************************************/
AssetManager::AssetManager()
{

}
/**************************************************************************
 * @brief Deconstructor
 *************************************************************************/
AssetManager::~AssetManager()
{

}

/**************************************************************************
 * @brief Loads all assets, including textures, sounds, fonts, and scenes.
 *
 * This function loads all the assets used in the project, including
 * textures, sounds, fonts, and scenes. It provides feedback about the
 * success or failure of each asset type load.
 *
 * @param None.
 * @return None.
 *************************************************************************/
void AssetManager::LoadAll() {



#ifdef _DEBUG
    bool loadTextures = AssetManager::LoadTextures(),
        //loadSprites = AssetManager::LoadSprites(),
        //loadSounds = AssetManager::LoadSounds(),
        //loadFonts = AssetManager::LoadFonts(),
        loadScenes = AssetManager::LoadScenes(),
        //loadPrefabs = AssetManager::LoadPrefabs(),
        loadShaders = AssetManager::LoadShaders();


    std::cout
        << ((loadTextures) ? "Textures loaded successfully" : "Failed to load textures") << std::endl
        //<< ((loadSprites) ? "Sprites loaded successfully" : "Failed to load sprites") << std::endl
        //<< ((loadSounds) ? "Sounds loaded successfully" : "Failed to load sounds") << std::endl
        //<< ((loadFonts) ? "Fonts loaded successfully" : "Failed to load fonts") << std::endl
        << ((loadScenes) ? "Scenes loaded successfully" : "Failed to load scenes") << std::endl
        //<< ((loadPrefabs) ? "Prefabs loaded successfully" : "Failed to load prefabs") << std::endl
        << ((loadShaders) ? "Shaders loaded successfully" : "Failed to load shaders") << std::endl;
#else
    AssetManager::LoadTextures(),
        //AssetManager::LoadSprites(),
        //AssetManager::LoadSounds(),
        //AssetManager::LoadFonts(),
        AssetManager::LoadScenes(),
        //AssetManager::LoadPrefabs(),
        AssetManager::LoadShaders();
#endif
}

/**************************************************************************
 * @brief Frees all loaded assets, including textures, sounds, fonts, and scenes.
 *
 * This function frees all the previously loaded assets to release resources.
 * It provides feedback about the success or failure of each asset type release.
 *
 * @param None.
 * @return None.
 *************************************************************************/
void AssetManager::FreeAll() {
#ifdef _DEBUG
    bool freeTextures = AssetManager::FreeTextures(),
        //freeSprites = AssetManager::FreeSprites(),
        //freeSounds = AssetManager::FreeSounds(),
        //freeFonts = AssetManager::FreeFonts(),
        //freeScenes = AssetManager::FreeScenes(),
        //freePrefabs = AssetManager::FreePrefabs(),
        freeShaders = AssetManager::FreeShaders();


    std::cout
        << ((freeTextures) ? "Textures freed successfully" : "Failed to free textures") << std::endl
        //<< ((freeSprites) ? "Sprites freed successfully" : "Failed to free sprites") << std::endl
        //<< ((freeSounds) ? "Sounds freed successfully" : "Failed to free sounds") << std::endl
        //<< ((freeFonts) ? "Fonts freed successfully" : "Failed to free fonts") << std::endl
        //<< ((freeScenes) ? "Scenes freed successfully" : "Failed to free scenes") << std::endl
        //<< ((freePrefabs) ? "Prefabs freed successfully" : "Failed to free prefabs") << std::endl
        << ((freeShaders) ? "Shaders freed successfully" : "Failed to free shaders") << std::endl;
#else
    AssetManager::FreeTextures(),
        //AssetManager::FreeSprites(),
        //AssetManager::FreeSounds(),
        //AssetManager::FreeFonts(),
        //AssetManager::FreeScenes(),
        //AssetManager::FreePrefabs(),
        AssetManager::FreeShaders();
#endif
}

/**************************************************************************
 * @brief Reloads all assets, including textures, sounds, fonts, and scenes.
 *
 * This function reloads all the assets, effectively refreshing the assets.
 * It provides feedback about the success or failure of each asset type reload.
 *
 * @param None.
 * @return None.
 *************************************************************************/
 //void AssetManager::ReloadAll() {
 //
 //
 //#ifdef _DEBUG
 //    bool reloadTextures = AssetManager::ReloadTextures(),
 //        reloadSprites = AssetManager::ReloadSprites(),
 //        //reloadSounds = AssetManager::ReloadSounds(),
 //        //reloadFonts = AssetManager::ReloadFonts(),
 //        //reloadScenes = AssetManager::ReloadScenes(),
 //        //reloadPrefabs = AssetManager::ReloadPrefabs(),
 //        //reloadShaders = AssetManager::ReloadShaders();
 //
 //    //std::cout
 //    //    << ((reloadTextures) ? "Textures reloaded successfully" : "Failed to reload textures") << std::endl
 //    //    << ((reloadSprites) ? "Sprites reloaded successfully" : "Failed to reload Sprites") << std::endl
 //    //    << ((reloadSounds) ? "Sounds reloaded successfully" : "Failed to reload sounds") << std::endl
 //    //    << ((reloadFonts) ? "Fonts reloaded successfully" : "Failed to reload fonts") << std::endl
 //    //    << ((reloadScenes) ? "Scenes reloaded successfully" : "Failed to reload scenes") << std::endl
 //    //    << ((reloadPrefabs) ? "Prefabs reloaded successfully" : "Failed to reload prefabs") << std::endl
 //    //    << ((reloadShaders) ? "Shaders reloaded successfully" : "Failed to reload shaders") << std::endl;
 //#else
 //    AssetManager::ReloadTextures();
 //    AssetManager::ReloadSprites();
 //    AssetManager::ReloadSounds();
 //    AssetManager::ReloadFonts();
 //    AssetManager::ReloadScenes();
 //    AssetManager::ReloadPrefabs();
 //    AssetManager::ReloadShaders();
 //#endif // DEBUG
 //}












// Function to generate a 64-bit GUID with the first bit set to 1 and next 5 bits set by the parameter
uint64_t generateGUID64(int valueFor5Bits) {
    // Ensure the provided value fits in 5 bits (0-31)
    valueFor5Bits &= 0x1F;  // 0x1F is 11111 in binary, which masks the value to 5 bits

    // Use random device and mt19937_64 for high-quality random number generation
    std::random_device rd;  // Seed for random number engine
    std::mt19937_64 generator(rd());  // 64-bit Mersenne Twister engine
    uint64_t guid = generator();

    // Force the first bit (MSB) to be 1
    guid |= (1ULL << 63);

    // Clear the next 5 bits after the MSB
    guid &= ~(0x1FULL << 58);  // 0x1F is 5 bits of 1's, shifted to the 58th position

    // Set the next 5 bits to the value of the parameter
    guid |= (static_cast<uint64_t>(valueFor5Bits) << 58);

    return guid;
}

 /**************************************************************************
  * @brief Loads textures from the specified directory.
  *
  * This function scans the specified directory for image files and loads them
  * as OpenGL textures. It supports common image formats such as PNG, JPEG, and
  * BMP. Each image file is loaded as a texture and stored for later use in the
  * application. The textures are associated with their respective filenames
  * (without file extensions) and stored in the 'textures' map for easy access.
  *
  * @param None.
  *
  * @return bool - Returns true if textures are loaded successfully, false if
  *               there is an error. It will return false if the specified
  *               directory doesn't exist or if there are any issues with
  *               loading the textures.
  *************************************************************************/
bool AssetManager::LoadTextures() {

    Currentlyloading = true;

    if (fs::is_directory(FILEPATH_TEXTURES)) {
        for (const auto& entry : fs::directory_iterator(FILEPATH_TEXTURES)) {
            std::string texFilePath = FILEPATH_TEXTURES + "\\" + entry.path().filename().string();
            //std::cout << "Texture file " << texFilePath << " Found." << std::endl;

            size_t pos = entry.path().filename().string().find_last_of('.');
            if (pos != std::string::npos) {
                std::string nameWithoutExtension = entry.path().filename().string().substr(0, pos);
                //std::cout << nameWithoutExtension << std::endl;

                std::string Extension = entry.path().filename().string().substr(pos);
                //std::cout << Extension;
                std::string allowedExtensions = ".jpg,.jpeg,.png,.gif";

                // Check if the substring exists in the full string
                size_t found = allowedExtensions.find(toLowerCase(Extension));

                if (found == std::string::npos) {
                    std::string file(entry.path().filename().string());
                    std::wstring widefile(file.begin(), file.end());
                    HWND hwnd = GetActiveWindow();
                    std::string filepath(FILEPATH_TEXTURES);
                    // Convert std::string to std::wstring
                    std::wstring widefilepath(filepath.begin(), filepath.end());

                    std::wstring message = L"Incompatible file \"" + widefile + L"\" detected in \"" + widefilepath + L"\" folder!\n\nFile moved to trash bin!";
                    LPCWSTR boxMessage = message.c_str();

                    MessageBox(hwnd, boxMessage, L"Load Failure", MB_OK | MB_ICONERROR);

                    // Construct the full destination path including the file name
                    fs::path destinationPath = FILEPATH_TRASHBIN / entry.path().filename();
                    fs::path trashbin = FILEPATH_TRASHBIN;

                    if (!fs::exists(trashbin))
                        fs::create_directory(trashbin);

                    if (fs::exists(destinationPath)) {
                        int counter = 1;

                        std::string addstr = nameWithoutExtension + "(" + std::to_string(counter) + ")" + Extension;

                        fs::path finalDestination = trashbin / addstr;

                        while (fs::exists(finalDestination)) {
                            counter++;
                            addstr = nameWithoutExtension + "(" + std::to_string(counter) + ")" + Extension;
                            finalDestination = trashbin / addstr;
                        }

                        fs::rename(entry.path(), finalDestination);
                    }
                    else {
                        fs::rename(entry.path(), destinationPath);
                    }

                    continue;
                }


#ifdef _DEBUG
                std::cout << "\n**************************************************************************************\n";
                std::cout << nameWithoutExtension << " detected successfully!\n";
#endif // DEBUG

                // Create an output file stream (ofstream) object
                std::string descriptorFilePath{ FILEPATH_DESCRIPTORS + "/" + nameWithoutExtension + ".txt" };
                std::ofstream outFile(descriptorFilePath);

                // Check if the file opened successfully
                if (outFile.is_open()) {

                    // Write "Hello World" to the file
                    outFile << "File Name : " << entry.path().filename().string() << std::endl;
                    outFile << "Compression Format : "<< "-fd BC3";

                    // Close the file
                    outFile.close();
                }

                // Process the descriptor file and print details
                std::vector<std::string> fileInfo = processDescriptorFile(descriptorFilePath);
                if (!fileInfo.empty()) {
                    // std::cout << fileInfo[0] << std::endl;
                    // std::cout << fileInfo[1] << std::endl;

                    // Run compression command
                    runCommand("..\\lib\\Compressonator\\compressonatorcli.exe " + fileInfo[0] + " " + FILEPATH_TEXTURES + "\\" + fileInfo[1] + " " + FILEPATH_DDS + "\\" + nameWithoutExtension + ".dds");
                    std::cout << "eleiggle\t " << nameWithoutExtension << '\n';
                    g_ResourceManager.AddTextureDDS(nameWithoutExtension);
                }


            }
            else
            {
#ifdef _DEBUG
                std::cout << "File " << entry.path().filename().string() << " is missing file extension.\n";
#endif // DEBUG
            }

        }
        Currentlyloading = false;
        return true;
    }
    else {
        // Print error
#ifdef _DEBUG
        std::cout << "The specified path is not a directory." << std::endl;
#endif // DEBUG
        Currentlyloading = false;
        return false;
    }
}


/**************************************************************************
 * @brief Frees all loaded textures.
 *
 * This function removes all textures that have been previously loaded using
 * the 'LoadTextures' function. It clears the 'textures' map, effectively
 * releasing the memory used for storing the textures. After calling this
 * function, the 'textures' map will be empty.
 *
 * @return bool - Returns true if textures are freed successfully, and the
 *               'textures' map is empty. Returns false if there is an error or
 *               if the map is not empty after the operation.
 *************************************************************************/
bool AssetManager::FreeTextures() {
    TextureDescriptionFiles.clear();
    // Return true if the container size is 0, false otherwise.
    return TextureDescriptionFiles.empty();
}

/**************************************************************************
 * @brief Reloads all textures.
 *
 * This function is used to reload all textures by first freeing the existing
 * textures and then loading them again using the 'LoadTextures' function. It
 * is a convenient way to refresh the textures in your application without
 * restarting the entire program.
 *
 * @return True if the scenes are both successfully freed and reloaded,
 *         false otherwise.
 *************************************************************************/
bool AssetManager::ReloadTextures() {
    // Return true if free and load successfully
    return (AssetManager::FreeTextures() && AssetManager::LoadTextures());
}



































/**************************************************************************
 * @brief Loads the scene filenames from the specified directory.
 *
 * This function scans the directory specified by FILEPATH_SCENES and retrieves
 * the filenames of all scenes found in that directory. It populates the 'scenes'
 * container with the filenames.
 *
 * @return True if the scenes were loaded successfully, false if there was an error.
 *************************************************************************/
bool AssetManager::LoadScenes() {
    Currentlyloading = true;
    bool result{ true };
    if (fs::is_directory(FILEPATH_SCENES)) {
        for (const auto& entry : fs::directory_iterator(FILEPATH_SCENES)) {

            // find the file extension 
            size_t extensionPos = entry.path().filename().string().find_last_of('.');

            std::string Extension = entry.path().filename().string().substr(extensionPos);
            //std::cout << Extension;
            std::string allowedExtensions = ".json";

            // Check if the substring exists in the full string
            size_t found = allowedExtensions.find(toLowerCase(Extension));

            if (found == std::string::npos) {
                std::string file(entry.path().filename().string());
                std::wstring widefile(file.begin(), file.end());
                HWND hwnd = GetActiveWindow();
                std::string filepath(FILEPATH_SCENES);
                // Convert std::string to std::wstring
                std::wstring widefilepath(filepath.begin(), filepath.end());

                std::wstring message = L"Incompatible file \"" + widefile + L"\" detected in \"" + widefilepath + L"\" folder!\n\nFile moved to trash bin!";
                LPCWSTR boxMessage = message.c_str();

                MessageBox(hwnd, boxMessage, L"Load Failure", MB_OK | MB_ICONERROR);

                // Construct the full destination path including the file name
                fs::path destinationPath = FILEPATH_TRASHBIN / entry.path().filename();
                fs::path trashbin = FILEPATH_TRASHBIN;

                if (!fs::exists(trashbin))
                    fs::create_directory(trashbin);

                if (fs::exists(destinationPath)) {
                    int counter = 1;
                    std::string nameWithoutExtension = entry.path().stem().string();

                    std::string addstr = nameWithoutExtension + "(" + std::to_string(counter) + ")" + Extension;

                    fs::path finalDestination = trashbin / addstr;

                    while (fs::exists(finalDestination)) {
                        counter++;
                        addstr = nameWithoutExtension + "(" + std::to_string(counter) + ")" + Extension;
                        finalDestination = trashbin / addstr;
                    }

                    fs::rename(entry.path(), finalDestination);
                }
                else {
                    fs::rename(entry.path(), destinationPath);
                }

                continue;
            }
        }
    }
    else {
        // Print error
#ifdef _DEBUG
        std::cout << "The specified scenes path is not a directory." << std::endl;
#endif // DEBUG
        result = false;
    }
    Currentlyloading = false;

    return result;
}

/**************************************************************************
 * @brief Reloads the available scene filenames.
 *
 * This function reloads the available scene filenames by first freeing the
 * current list of scenes and then loading them again from the specified
 * directory.
 *
 * @return True if the scenes are both successfully freed and reloaded,
 *         false otherwise.
 *************************************************************************/
bool AssetManager::ReloadScenes() {
    return AssetManager::LoadScenes();
}



















/**************************************************************************
 * @brief Load the Shaders for Graphics Pipeline for Object to Render
 *        and/or Translate their objects.
 *
 * @return True if prefabs are successfully loaded,
 *         false otherwise.
*************************************************************************/
bool AssetManager::LoadShaders() {

    FreeShaders();

    Currentlyloading = true;

    // Check if the Shaders filepath is an existing directory
    if (fs::is_directory(FILEPATH_SHADERS)) {

        // Create temporary containers for the shader files
        std::vector<std::string> fragfiles, vertfiles;

        // For every file within the directory
        for (const auto& entry : fs::directory_iterator(FILEPATH_SHADERS)) {
            std::string texFilePath = FILEPATH_SHADERS + "/" + entry.path().filename().string();
            //std::cout << "Texture file " << texFilePath << " Found." << std::endl;

            size_t pos = entry.path().filename().string().find_last_of('.');
            if (pos != std::string::npos) {
                std::string nameWithoutExtension = entry.path().filename().string().substr(0, pos);
                //std::cout << nameWithoutExtension << std::endl;

                std::string Extension = entry.path().filename().string().substr(pos);
                //std::cout << Extension;
                std::string allowedExtensions = ".frag,.vert";

                // Check if the substring exists in the full string
                size_t found = allowedExtensions.find(toLowerCase(Extension));

                if (found == std::string::npos) {
                    std::string file(entry.path().filename().string());
                    std::wstring widefile(file.begin(), file.end());
                    HWND hwnd = GetActiveWindow();
                    std::string filepath(FILEPATH_SHADERS);
                    // Convert std::string to std::wstring
                    std::wstring widefilepath(filepath.begin(), filepath.end());

                    std::wstring message = L"Incompatible file \"" + widefile + L"\" detected in \"" + widefilepath + L"\" folder!\n\nFile moved to trash bin!";
                    LPCWSTR boxMessage = message.c_str();

                    MessageBox(hwnd, boxMessage, L"Load Failure", MB_OK | MB_ICONERROR);

                    // Construct the full destination path including the file name
                    fs::path destinationPath = FILEPATH_TRASHBIN / entry.path().filename();
                    fs::path trashbin = FILEPATH_TRASHBIN;

                    // If the trashbin file doesn't exist, create it
                    if (!fs::exists(trashbin))
                        fs::create_directory(trashbin);

                    // If there exists a file in the trashbin with the same file name, add a counter to the name
                    if (fs::exists(destinationPath)) {
                        int counter = 1;

                        std::string addstr = nameWithoutExtension + "(" + std::to_string(counter) + ")" + Extension;

                        fs::path finalDestination = trashbin / addstr;

                        while (fs::exists(finalDestination)) {
                            counter++;
                            addstr = nameWithoutExtension + "(" + std::to_string(counter) + ")" + Extension;
                            finalDestination = trashbin / addstr;
                        }

                        // Move the file to the trashbin
                        fs::rename(entry.path(), finalDestination);
                    }
                    // If there isn't an existing file in the trashbin with the same file name, move the file to the trashbin
                    else {
                        fs::rename(entry.path(), destinationPath);
                    }

                    continue;
                }

                // If the file is detected to be a frag file
                if (found == 0)
                    fragfiles.push_back(nameWithoutExtension);
                // If the file is detected to be a vert file
                else if (found == 6)
                    vertfiles.push_back(nameWithoutExtension);
#ifdef _DEBUG
                std::cout << nameWithoutExtension << " success!\n";
#endif // DEBUG
            }
            else
            {
#ifdef _DEBUG
                std::cout << "File " << entry.path().filename().string() << " is missing file extension.\n";
#endif // DEBUG
            }

        }

        // sort the names in the vector alphabetically
        std::sort(fragfiles.begin(), fragfiles.end());
        std::sort(vertfiles.begin(), vertfiles.end());

        // Find differing strings
        std::vector<std::string> differingStrings;
        std::set_symmetric_difference(fragfiles.begin(), fragfiles.end(), vertfiles.begin(), vertfiles.end(), std::back_inserter(differingStrings));

        // Move files with missing partners to the trash bin
        fs::path trashbin = FILEPATH_TRASHBIN;

        // If trashbin doesn't exist
        if (!fs::exists(trashbin))
            fs::create_directory(trashbin);

        // For all files with missing pairing files
        for (const auto& file : differingStrings) {
            size_t fragIndex = std::find(fragfiles.begin(), fragfiles.end(), file) - fragfiles.begin(),
                vertIndex = std::find(vertfiles.begin(), vertfiles.end(), file) - vertfiles.begin();

            if (fragIndex != fragfiles.size() && vertIndex == vertfiles.size()) {
                std::string fileName = file + ".frag";  // Assuming .frag extension, modify based on your naming convention
                fs::path sourceFilePath = FILEPATH_SHADERS + "/" + fileName;
                fs::path destinationPath = trashbin / fileName;

                if (fs::exists(sourceFilePath)) {
                    if (fs::exists(destinationPath)) {
                        int counter = 1;
                        std::string addstr = file + "(" + std::to_string(counter) + ").frag";

                        fs::path finalDestination = trashbin / addstr;

                        while (fs::exists(finalDestination)) {
                            counter++;
                            addstr = file + "(" + std::to_string(counter) + ").frag";
                            finalDestination = trashbin / addstr;
                        }

                        fs::rename(sourceFilePath, finalDestination);
                        // Update fragfiles after moving the file
                        fragfiles.erase(std::remove(fragfiles.begin(), fragfiles.end(), file), fragfiles.end());
                    }
                    else {
                        fs::rename(sourceFilePath, destinationPath);
                        // Update fragfiles after moving the file
                        fragfiles.erase(std::remove(fragfiles.begin(), fragfiles.end(), file), fragfiles.end());
                    }

                    // Display a pop-up message
                    HWND hwnd = GetActiveWindow();
                    std::wstring wideFileName(fileName.begin(), fileName.end());
                    std::wstring message = L"File \"" + wideFileName + L"\" moved to trash bin!";
                    LPCWSTR boxMessage = message.c_str();
                    MessageBox(hwnd, boxMessage, L"Missing Corresponding Vert File.", MB_OK | MB_ICONINFORMATION);
                }
            }
            else if (fragIndex == fragfiles.size() && vertIndex != vertfiles.size()) {
                std::string fileName = file + ".vert";  // Assuming .vert extension, modify based on your naming convention
                fs::path sourceFilePath = FILEPATH_SHADERS + "/" + fileName;
                fs::path destinationPath = trashbin / fileName;

                if (fs::exists(sourceFilePath)) {
                    if (fs::exists(destinationPath)) {
                        int counter = 1;
                        std::string addstr = file + "(" + std::to_string(counter) + ").vert";

                        fs::path finalDestination = trashbin / addstr;

                        while (fs::exists(finalDestination)) {
                            counter++;
                            addstr = file + "(" + std::to_string(counter) + ").vert";
                            finalDestination = trashbin / addstr;
                        }

                        fs::rename(sourceFilePath, finalDestination);
                        // Update vertfiles after moving the file
                        vertfiles.erase(std::remove(vertfiles.begin(), vertfiles.end(), file), vertfiles.end());
                    }
                    else {
                        fs::rename(sourceFilePath, destinationPath);
                        // Update vertfiles after moving the file
                        vertfiles.erase(std::remove(vertfiles.begin(), vertfiles.end(), file), vertfiles.end());
                    }

                    // Display a pop-up message
                    HWND hwnd = GetActiveWindow();
                    std::wstring wideFileName(fileName.begin(), fileName.end());
                    std::wstring message = L"File \"" + wideFileName + L"\" moved to trash bin!";
                    LPCWSTR boxMessage = message.c_str();
                    MessageBox(hwnd, boxMessage, L"Missing Corresponding Frag File.", MB_OK | MB_ICONINFORMATION);
                }
            }
        }

        //// Print out fragfiles and vertfiles
        //std::cout << "Frag files: ";
        //for (const auto& fragFile : fragfiles) {
        //    std::cout << fragFile << "\n";
        //}
        //std::cout << "\n";

        /*std::cout << "Vert files: ";
        for (const auto& vertFile : vertfiles) {
            std::cout << vertFile << "\n";
        }
        std::cout << "\n";*/

        // For every fragfile, pair it with a vert file
        for (int i = 0; i < fragfiles.size(); i++) {
            VectorPairString pairing{ std::make_pair<std::string, std::string>(FILEPATH_SHADERS + "\\" + vertfiles[i] + ".vert", FILEPATH_SHADERS + "\\" + fragfiles[i] + ".frag") };
            InitShdrpgms(pairing);
        }


        std::cout << "\n\n\n\n";

        // Copy the names in order to the shdrpgmOrder container. Needed as the shader programs stored in shdrpgms is stored in the same sequence
        shdrpgmOrder = vertfiles;
#ifdef _DEBUG
        for (auto tmp : shdrpgmOrder)
            std::cout << "Shader Program Order: " << tmp << "\n";
#endif

        std::cout << "\n\n\n\n";
        Currentlyloading = false;
        return true;
    }
    else {
        // Print error
#ifdef _DEBUG
        std::cout << "The specified path is not a directory." << std::endl;
#endif // DEBUG
        Currentlyloading = false;
        return false;
    }
}


/**************************************************************************
 * @brief Initialize Shaders into the graphics pipeline
 * @param VPSS the vector paired <string,string>
 *************************************************************************/
void AssetManager::InitShdrpgms(VectorPairString const& vpss)
{


    for (auto const& x : vpss) {
        std::cout << "Files to be added: \t";
        std::cout << x.first << '\t';
        std::cout << x.second << '\t' << '\n';


        // Create Vector for pair of Enum and String
        std::vector<std::pair<GLenum, std::string>> shdr_files;
        // Emplace back into the shdr_files vector
        shdr_files.emplace_back(std::make_pair(GL_VERTEX_SHADER, x.first));
        shdr_files.emplace_back(std::make_pair(GL_FRAGMENT_SHADER, x.second));

        // Create Shader
        OpenGLShader shdr_pgm;
        // Validate the shader program (shdr_Files).
        shdr_pgm.CompileLinkValidate(shdr_files);
        // insert shader program into container
        shdrpgms.emplace_back(shdr_pgm);

    }

    std::cout << shdrpgms.size() << "\n";

}

/**************************************************************************
 * @brief  Access the shader by name
 *
 * @param  std::string      Name of the shader
 * @return OpenGLShader&    The named shader
*************************************************************************/
OpenGLShader& AssetManager::GetShader(std::string shaderName) {
    // Find the index of the shader in shdrpgmorder
    auto it = std::find(shdrpgmOrder.begin(), shdrpgmOrder.end(), shaderName);




    if (it != shdrpgmOrder.end()) {
        // Calculate the index in shdrpgm container
        size_t index = std::distance(shdrpgmOrder.begin(), it);

        // Check if the index is valid
        if (index < shdrpgms.size()) 
        {
            // std::cout << shdrpgms[index].Validate();
            
            // Return the shader program at the corresponding index
            return shdrpgms[index];
        }
        else {
            // Handle an invalid index (out of bounds)
            // You might want to throw an exception or handle it based on your requirements
            throw std::out_of_range("Invalid index when retrieving shader program.");
        }
    }
    else {
        // Handle the case where the shaderName is not found in shdrpgmorder
        // You might want to throw an exception or handle it based on your requirements
        throw std::runtime_error(shaderName + " Shader not found in shdrpgmorder.");
    }
}

/**************************************************************************
 * @brief Free Shaders (Unimplemented)
 *************************************************************************/
bool AssetManager::FreeShaders()
{
    return false;
}
