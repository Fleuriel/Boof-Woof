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
#include "pch.h"

#pragma warning(push)
#pragma warning(disable: 4244 4005)

#include <thread>
#include "AssetManager.h"
#include "windows.h"
#include "FilePaths.h"
#include "Compressonator.h"
#include "ResourceManager/ResourceManager.h"

#include "Graphics/GraphicsSystem.h" //temporary

#include "Descriptor.h"

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


void AssetManager::DiscardToTrashBin(const std::string& filepath, const std::string& folderName, bool msg) {

    if (msg) {
        // Convert the file path from std::string to std::wstring
        std::wstring widefile(filepath.begin(), filepath.end());
        HWND hwnd = GetActiveWindow();

        // Convert paths to wstring for MessageBox
        std::wstring widefolder(folderName.begin(), folderName.end());
        std::wstring message = L"Incompatible file \"" + widefile + L"\" detected in \"" + widefolder + L"\" folder!\n\nFile moved to trash bin!";
        LPCWSTR boxMessage = message.c_str();

        // Show message box to inform the user
        MessageBox(hwnd, boxMessage, L"Load Failure", MB_OK | MB_ICONERROR);
    }

    // Define the paths using std::filesystem
    fs::path entryPath(filepath);
    fs::path trashbin = FILEPATH_ASSET_TRASHBIN;
    fs::path destinationPath = trashbin / entryPath.filename();

    // Ensure the trash bin directory exists
    try {
        if (!fs::exists(trashbin)) {
            fs::create_directory(trashbin);
        }

        // If the destination file already exists, rename it with a counter
        if (fs::exists(destinationPath)) {
            int counter = 1;
            std::string nameWithoutExtension = entryPath.stem().string();  // File name without extension
            std::string extension = entryPath.extension().string();        // File extension

            std::string newFileName = nameWithoutExtension + "(" + std::to_string(counter) + ")" + extension;
            fs::path finalDestination = trashbin / newFileName;

            // Find an available name
            while (fs::exists(finalDestination)) {
                counter++;
                newFileName = nameWithoutExtension + "(" + std::to_string(counter) + ")" + extension;
                finalDestination = trashbin / newFileName;
            }

            // Move the file to the final destination
            fs::rename(entryPath, finalDestination);
        }
        else {
            // If no file with the same name exists, move it directly
            fs::rename(entryPath, destinationPath);
        }

        std::cout << "File moved to trash bin: " << destinationPath << std::endl;

    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "General error: " << e.what() << std::endl;
    }
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


    if (!fs::exists(FILEPATH_DESCRIPTORS))
        fs::create_directory(FILEPATH_DESCRIPTORS);

    if (!fs::exists(FILEPATH_RESOURCES))
        fs::create_directory(FILEPATH_RESOURCES);

#ifdef _DEBUG
    bool loadTextures   = AssetManager::LoadTextures(),
        loadObjects     = LoadObjects(),
        //loadSprites   = AssetManager::LoadSprites(),
        //loadSounds    = AssetManager::LoadSounds(),
        loadFonts     = AssetManager::LoadFonts(),
        loadScenes      = AssetManager::LoadScenes(),
        //loadPrefabs   = AssetManager::LoadPrefabs(),
        loadShaders     = AssetManager::LoadShaders();
       // LoadObjects();

    std::cout
        << ((loadTextures) ? "Textures loaded successfully" : "Failed to load textures") << std::endl
        //<< ((loadSprites) ? "Sprites loaded successfully" : "Failed to load sprites") << std::endl
        //<< ((loadSounds) ? "Sounds loaded successfully" : "Failed to load sounds") << std::endl
        << ((loadFonts) ? "Fonts loaded successfully" : "Failed to load fonts") << std::endl
        << ((loadScenes) ? "Scenes loaded successfully" : "Failed to load scenes") << std::endl
        //<< ((loadPrefabs) ? "Prefabs loaded successfully" : "Failed to load prefabs") << std::endl
        << ((loadShaders) ? "Shaders loaded successfully" : "Failed to load shaders") << std::endl 
        << ((loadObjects) ? "Object loaded Successfully" : "failed to load object") << '\n';
#else
        AssetManager::LoadTextures(),
        AssetManager::LoadObjects(),
        //AssetManager::LoadSprites(),
        //AssetManager::LoadSounds(),
        AssetManager::LoadFonts(),
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
    std::string filepath(FILEPATH_ASSET_TEXTURES);

    if (fs::is_directory(filepath)) {
        for (const auto& entry : fs::directory_iterator(filepath)) {
            std::string texFilePath = filepath + "\\" + entry.path().filename().string();
            //std::cout << "Texture file " << texFilePath << " Found." << std::endl;

            size_t pos = entry.path().filename().string().find_last_of('.');
            if (pos != std::string::npos) {
                std::string nameWithoutExtension = entry.path().filename().string().substr(0, pos);
                //std::cout << nameWithoutExtension << std::endl;

                std::string Extension = entry.path().filename().string().substr(pos);
                //std::cout << Extension;
                std::string allowedExtensions = ".png";

                // Check if the substring exists in the full string
                size_t found = allowedExtensions.find(toLowerCase(Extension));

                if (found == std::string::npos) {
                    DiscardToTrashBin(entry.path().string(), FILEPATH_ASSET_TEXTURES);
                    continue;
                }

                TextureFiles.insert(entry.path().wstring());

#ifdef _DEBUG
                std::cout << "\n**************************************************************************************\n";
                std::cout << nameWithoutExtension << " detected successfully!\n";
#endif // DEBUG

                if (!fs::exists(FILEPATH_DESCRIPTOR_TEXTURES))
                    fs::create_directory(FILEPATH_DESCRIPTOR_TEXTURES);

                // Create an output file stream (ofstream) object
                std::string descriptorFilePath{ FILEPATH_DESCRIPTOR_TEXTURES + "/" + nameWithoutExtension + ".json"};

                // Check if the file exists
                if (std::filesystem::exists(descriptorFilePath)) {
                    //std::cout << "Descriptor file already exists: " << descriptorFilePath << std::endl;
                    // Optional: Handle what to do if the file exists (e.g., overwrite, prompt user, etc.)
                }
                else {
                    TextureDescriptor desc;
                    if (desc.SaveTextureDescriptor(descriptorFilePath)) {
                        std::cout << "Descriptor file saved successfully: " << descriptorFilePath << std::endl;
                    }
                    else {
                        std::cerr << "Failed to save descriptor file: " << descriptorFilePath << std::endl;
                    }
                }



                // Ensure the directory exists
                if (!fs::exists(FILEPATH_RESOURCE_TEXTURES)) {
                    fs::create_directory(FILEPATH_RESOURCE_TEXTURES);
                }

                // Ensure texFilePath is valid
                fs::path outputPath = fs::path(FILEPATH_RESOURCE_TEXTURES) / (nameWithoutExtension + ".dds");

                // Run the compression command
                textureInfo.LoadTextureDescriptor(descriptorFilePath);
                CompressTextureWithDescriptor(textureInfo, texFilePath, outputPath.string());
                g_ResourceManager.AddTextureDDS(nameWithoutExtension);


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
    DeleteAllFilesInDirectory(FILEPATH_RESOURCE_TEXTURES);
    DeleteAllFilesInDirectory(FILEPATH_DESCRIPTOR_TEXTURES);
    TextureFiles.clear();
    TextureDescriptionFiles.clear();
    // Return true if the container size is 0, false otherwise.
    return (TextureDescriptionFiles.empty() && TextureFiles.empty());
}

/**************************************************************************
 * @brief Reloads all textures.
 *
 * This function is used to reload all textures by first freeing the existing
 * textures and then loading them again using the 'LoadTextures' function. It
 * is a convenient way to refresh the textures in your application without
 * restarting the entire program.
 *
 * @return True if the textures are both successfully freed and reloaded,
 *         false otherwise.
 *************************************************************************/
bool AssetManager::ReloadTextures() {
    FreeTextures();
    LoadTextures();
    return g_ResourceManager.ReloadTexturesDDS();
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
    if (fs::is_directory(FILEPATH_ASSET_SCENES)) {
        for (const auto& entry : fs::directory_iterator(FILEPATH_ASSET_SCENES)) {

            // find the file extension 
            size_t extensionPos = entry.path().filename().string().find_last_of('.');

            std::string Extension = entry.path().filename().string().substr(extensionPos);
            //std::cout << Extension;
            std::string allowedExtensions = ".json";

            // Check if the substring exists in the full string
            size_t found = allowedExtensions.find(toLowerCase(Extension));

            if (found == std::string::npos) {
                DiscardToTrashBin(entry.path().string(), FILEPATH_ASSET_SCENES);
                continue;
            }

            SceneFiles.insert(entry.path().wstring());
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

bool AssetManager::FreeScenes() {
    SceneFiles.clear();
    return SceneFiles.empty();
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
    return (FreeScenes() && LoadScenes());
}









void parseOBJ(const std::string& filename, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;

    std::ifstream objFile(filename);
    if (!objFile.is_open()) {
        std::cerr << "Could not open OBJ file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(objFile, line)) {
        std::stringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "v") {
            glm::vec3 position;
            ss >> position.x >> position.y >> position.z;
            positions.push_back(position);
        }
        else if (type == "vt") {
            glm::vec2 texCoord;
            ss >> texCoord.x >> texCoord.y;
            texCoords.push_back(texCoord);
        }
        else if (type == "vn") {
            glm::vec3 normal;
            ss >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        }
        else if (type == "f") {
            unsigned int vertexIndex[3], texCoordIndex[3], normalIndex[3];
            char slash;
            for (int i = 0; i < 3; i++) {
                ss >> vertexIndex[i] >> slash >> texCoordIndex[i] >> slash >> normalIndex[i];
                vertexIndex[i]--; texCoordIndex[i]--; normalIndex[i]--;

                Vertex vertex;
                vertex.Position = positions[vertexIndex[i]];
                vertex.TexCoords = texCoords[texCoordIndex[i]];
                vertex.Normal = normals[normalIndex[i]];
                vertices.push_back(vertex);

                indices.push_back(static_cast<unsigned int>(vertices.size() - 1));
            }
        }
    }
    objFile.close();
}


void saveMeshToBin(std::vector<Mesh> meshes, const std::string& binFilePath) {
    std::ofstream binFile(binFilePath, std::ios::binary);
    if (!binFile.is_open()) {
        std::cerr << "Failed to create binary file at saveAllMeshesToBin: " << binFilePath << std::endl;
        return;
    }

    // Step 1: Write the number of meshes
    size_t meshCount = meshes.size();
    binFile.write(reinterpret_cast<const char*>(&meshCount), sizeof(size_t));

    // Step 2: For each mesh, write vertex and index data
    for (const auto& mesh : meshes) {
        // Write vertex data
        size_t vertexCount = mesh.vertices.size();
        binFile.write(reinterpret_cast<const char*>(&vertexCount), sizeof(size_t));
        binFile.write(reinterpret_cast<const char*>(mesh.vertices.data()), vertexCount * sizeof(Vertex));

        // Write index data
        size_t indexCount = mesh.indices.size();
        binFile.write(reinterpret_cast<const char*>(&indexCount), sizeof(size_t));
        binFile.write(reinterpret_cast<const char*>(mesh.indices.data()), indexCount * sizeof(unsigned int));
    }

    binFile.close();
}


static bool MTLCheck = false;
static bool PNGCheck = false;
static bool JPGCheck = false;

bool AssetManager::LoadObjects() {
    Currentlyloading = true;
    std::string filepath(FILEPATH_ASSET_OBJECTS);

    if (fs::is_directory(filepath)) {
        for (const auto& entry : fs::directory_iterator(filepath)) {

            std::string texFilePath = filepath + "\\" + entry.path().filename().string();

            size_t pos = entry.path().filename().string().find_last_of('.');
            if (pos != std::string::npos) {

                std::string nameWithoutExtension = entry.path().filename().string().substr(0, pos);
                std::string Extension = entry.path().filename().string().substr(pos);
                std::string allowedExtensions = ".obj";
                std::string ignExt = ".mtl, .jpg, .png";

                std::vector<std::string> ignoredExtensions = { ".mtl", ".png", ".jpg" };


                std::string mtlFileName = nameWithoutExtension + ".mtl";
                std::string pngFileName = nameWithoutExtension + ".png";
                std::string jpgFileName = nameWithoutExtension + ".jpg";
#ifdef _DEBUG
                std::cout << mtlFileName << '\t' << pngFileName << '\t' << jpgFileName << '\t';
#endif

                // Check if the substring exists in the full string
                size_t found = allowedExtensions.find(toLowerCase(Extension));
                
                size_t found2 = ignExt.find(toLowerCase(Extension));

                if (!(found == std::string::npos && found2 == std::string::npos)) {
                    ObjectFiles.insert(entry.path().wstring());
                }


                // Only proceed if the extension matches the allowed extension
                if (found == std::string::npos) {
                    // Process the .obj file
#ifdef _DEBUG
                    std::cout << "Processing: " << texFilePath << std::endl;
#endif
                }
                else {
                    if (found2 == std::string::npos)
                    {
                        continue;
                    }
                    // Discard unrecognized files
                    DiscardToTrashBin(entry.path().string(), "FILEPATH_OBJECTS");
                }

                

                for (int i = 0; i < ignoredExtensions.size(); ++i)
                {
                    if (Extension == ignoredExtensions[0])
                    {
                        MTLCheck = true;
                    }
                    if (Extension == ignoredExtensions[1])
                    {
                        PNGCheck = true;
                    }
                    if (Extension == ignoredExtensions[2])
                    {
                        JPGCheck = true;
                    }
                }

                

#ifdef _DEBUG
                std::cout << MTLCheck << '\t' << PNGCheck << '\t' << JPGCheck << '\n';
#endif

#ifdef _DEBUG
                std::cout << "\n**************************************************************************************\n";

                std::cout << "\n**************************************************************************************\n";
                std::cout << "ASDF: " << nameWithoutExtension << " detected successfully!\n";
                std::cout << "\n**************************************************************************************\n";
#endif

                if (!fs::exists(FILEPATH_RESOURCE_OBJECTS))
                    fs::create_directory(FILEPATH_RESOURCE_OBJECTS);

                std::string objFilePath = FILEPATH_ASSET_OBJECTS + "\\" + nameWithoutExtension + ".obj";
                std::string binFilePath = FILEPATH_RESOURCE_OBJECTS + "\\" + nameWithoutExtension + ".bin";


                if (!fs::exists(FILEPATH_DESCRIPTOR_OBJECTS))
                    fs::create_directory(FILEPATH_DESCRIPTOR_OBJECTS);


                std::string descriptorFilePath{ FILEPATH_DESCRIPTOR_OBJECTS + "/Model_" + nameWithoutExtension + ".txt" };
                std::ofstream outFile(descriptorFilePath);
                
               
 
                if (outFile.is_open())
                {
                    outFile << "File Name: " << nameWithoutExtension << '\n';
                    outFile << "Source File Path: " << objFilePath << '\n';
                    outFile << "Output File Path: " << binFilePath << '\n';
                    outFile << "MTL File Exist: " << MTLCheck << '\n';
                    outFile << "PNG File Exist: " << PNGCheck << '\n';
                    outFile << "JPG File Exist: " << JPGCheck << '\n';
                    outFile << "Expected Attributes: \n";
                    outFile << " -  Vertices\n";
                    outFile << " -  Indices\n";
                    outFile << " -  Normals\n";
                    outFile << "Transform Defauls: \n";
                    outFile << " -  Scale: 1.0\n";
                    outFile.close();
                }
 

                // bool firstCheck = false;

                if (MTLCheck && JPGCheck || PNGCheck)
                {

                    MTLCheck = JPGCheck = PNGCheck = false;
                    continue;

                }


#ifdef _DEBUG
                runCommand("..\\bin\\Debug-x64\\MeshCompiler\\MeshCompiler.exe " + descriptorFilePath);
#else
                runCommand("..\\bin\\Release-x64\\MeshCompiler\\MeshCompiler.exe " + descriptorFilePath);
#endif
                g_ResourceManager.AddModelBinary(nameWithoutExtension);

#ifdef _DEBUG
                std::cout << "Binary file created: " << binFilePath << std::endl;
#endif

            }
            else {
#ifdef _DEBUG
                std::cout << "File " << entry.path().filename().string() << " is missing file extension.\n";
#endif
            }
        }
        Currentlyloading = false;
        return true;
    }
    else {
#ifdef _DEBUG
        std::cout << "The specified path is not a directory." << std::endl;
#endif
        Currentlyloading = false;
        return false;
    }
}

bool AssetManager::FreeObjects() {
    ObjectFiles.clear();
    DeleteAllFilesInDirectory(FILEPATH_RESOURCE_OBJECTS);
    return ObjectFiles.empty();
}

bool AssetManager::ReloadObjects() {
    return (FreeObjects() && LoadObjects());
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
    if (fs::is_directory(FILEPATH_ASSET_SHADERS)) {

        // Create temporary containers for the shader files
        std::vector<std::string> fragfiles, vertfiles;

        // For every file within the directory
        for (const auto& entry : fs::directory_iterator(FILEPATH_ASSET_SHADERS)) {
            std::string texFilePath = FILEPATH_ASSET_SHADERS + "/" + entry.path().filename().string();
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
                    std::string filepath(FILEPATH_ASSET_SHADERS);
                    // Convert std::string to std::wstring
                    std::wstring widefilepath(filepath.begin(), filepath.end());

                    std::wstring message = L"Incompatible file \"" + widefile + L"\" detected in \"" + widefilepath + L"\" folder!\n\nFile moved to trash bin!";
                    LPCWSTR boxMessage = message.c_str();

                    MessageBox(hwnd, boxMessage, L"Load Failure", MB_OK | MB_ICONERROR);

                    // Construct the full destination path including the file name
                    fs::path destinationPath = FILEPATH_ASSET_TRASHBIN / entry.path().filename();
                    fs::path trashbin = FILEPATH_ASSET_TRASHBIN;

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
                if (found == 0) {
                    fragfiles.push_back(nameWithoutExtension);
                    ShaderFiles.insert(entry.path().wstring());
                }
                // If the file is detected to be a vert file
                else if (found == 6) {
                    vertfiles.push_back(nameWithoutExtension);
                    ShaderFiles.insert(entry.path().wstring());
                }
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
        fs::path trashbin = FILEPATH_ASSET_TRASHBIN;

        // If trashbin doesn't exist
        if (!fs::exists(trashbin))
            fs::create_directory(trashbin);

        // For all files with missing pairing files
        for (const auto& file : differingStrings) {
            size_t fragIndex = std::find(fragfiles.begin(), fragfiles.end(), file) - fragfiles.begin(),
                vertIndex = std::find(vertfiles.begin(), vertfiles.end(), file) - vertfiles.begin();

            if (fragIndex != fragfiles.size() && vertIndex == vertfiles.size()) {
                std::string fileName = file + ".frag";  // Assuming .frag extension, modify based on your naming convention
                fs::path sourceFilePath = FILEPATH_ASSET_SHADERS + "/" + fileName;
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
                fs::path sourceFilePath = FILEPATH_ASSET_SHADERS + "/" + fileName;
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
            VectorPairString pairing{ std::make_pair<std::string, std::string>(FILEPATH_ASSET_SHADERS + "\\" + vertfiles[i] + ".vert", FILEPATH_ASSET_SHADERS + "\\" + fragfiles[i] + ".frag") };
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
#ifdef _DEBUG
        std::cout << "Files to be added: \t";
        std::cout << x.first << '\t';
        std::cout << x.second << '\t' << '\n';
#endif

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
 * @brief Free Shaders 
 *************************************************************************/
bool AssetManager::FreeShaders()
{
    shdrpgms.clear();
    shdrpgmOrder.clear();
    ShaderFiles.clear();
    return (shdrpgms.empty() && shdrpgmOrder.empty() && ShaderFiles.empty());
}

bool AssetManager::ReloadShaders()
{
    return (FreeShaders() && LoadShaders());
}
























bool AssetManager::LoadFonts() {

    Currentlyloading = true;
    std::string filepath(FILEPATH_ASSET_FONTS);

    if (fs::is_directory(filepath)) {
        for (const auto& entry : fs::directory_iterator(filepath)) {


            std::string FilePath = filepath + "\\" + entry.path().filename().string();
            //std::cout << "Font file " << FilePath << " Found." << std::endl;

            size_t pos = entry.path().filename().string().find_last_of('.');
            if (pos != std::string::npos) {

                std::string nameWithoutExtension = entry.path().filename().string().substr(0, pos);
                //std::cout << nameWithoutExtension << std::endl;

                std::string Extension = entry.path().filename().string().substr(pos);
                //std::cout << Extension;
                std::string allowedExtensions = ".ttf";

                // Check if the substring exists in the full string
                size_t found = allowedExtensions.find(toLowerCase(Extension));

                if (found == std::string::npos) {
                    DiscardToTrashBin(entry.path().string(), FILEPATH_ASSET_FONTS);
                    continue;
                }

                FontFiles.insert(entry.path().wstring());

#ifdef _DEBUG
                std::cout << "\n**************************************************************************************\n";
                std::cout << nameWithoutExtension << " detected successfully!\n";
#endif // DEBUG

                if (!fs::exists(FILEPATH_DESCRIPTOR_FONTS))
                    fs::create_directory(FILEPATH_DESCRIPTOR_FONTS);

                // Create an output file stream (ofstream) object
                std::string descriptorFilePath{ FILEPATH_DESCRIPTOR_FONTS+ "/" + nameWithoutExtension + ".json" };

                TextureDescriptor desc;
                desc.SaveTextureDescriptor(descriptorFilePath);  // Correctly passing as const reference

                // Ensure the directory exists
                if (!fs::exists(FILEPATH_RESOURCE_FONTS)) {
                    fs::create_directory(FILEPATH_RESOURCE_FONTS);
                }

                // Ensure texFilePath is valid
                fs::path outputPath = fs::path(FILEPATH_RESOURCE_FONTS) / (nameWithoutExtension + ".dds");

                // Run the compression command
                runCommand("..\\lib\\msdf-atlas-gen\\msdf-atlas-gen.exe -font " + FilePath + " -allglyphs -size 32 -imageout " + FILEPATH_RESOURCE_FONTS + "\\" + nameWithoutExtension + ".png" + " -json " + FILEPATH_RESOURCE_FONTS + "\\" + nameWithoutExtension + ".json");
                CompressTextureWithDescriptor(desc, FILEPATH_RESOURCE_FONTS + "\\" + nameWithoutExtension + ".png", outputPath.string());
                if (std::remove((FILEPATH_RESOURCE_FONTS + "\\" + nameWithoutExtension + ".png").c_str()) == 0) {
                    std::cout << "File deleted successfully.\n";
                }
                else {
                    std::perror("Error deleting file");
                }
                g_ResourceManager.AddFontDDS(nameWithoutExtension);

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

bool AssetManager::FreeFonts() {
    FontFiles.clear();
    DeleteAllFilesInDirectory(FILEPATH_RESOURCE_FONTS);
    DeleteAllFilesInDirectory(FILEPATH_DESCRIPTOR_FONTS);
    return FontFiles.empty();
}

bool AssetManager::ReloadFonts() {
    FreeFonts();
    LoadFonts();
    return g_ResourceManager.ReloadFontsDDS();
}










// Check for added and deleted texture files
bool AssetManager::CheckFiles(const std::wstring& path) {
    // Create a set to hold the current detected files
    std::set<std::wstring> currentFiles;

    // Iterate through the directory and add the file names to the set
    for (const auto& entry : fs::directory_iterator(path)) {
        if (entry.is_regular_file()) {
            currentFiles.insert(entry.path().wstring());
        }
    }


    bool hasChanges = false;

    if (path == L"..\\BoofWoof\\Assets\\Art\\Textures") {
        // Compare currentFiles with the existing TextureFiles
        hasChanges = (currentFiles != TextureFiles);

        // Update TextureFiles with currentFiles
        TextureFiles = std::move(currentFiles); // Efficiently swap the content
    }
    //else if (path == L"..\\BoofWoof\\Assets\\Art\\Sprites") {
    //    // Compare currentFiles with the existing SpriteFiles
    //    hasChanges = (currentFiles != SpriteFiles);

    //    // Update SpriteFiles with currentFiles
    //    SpriteFiles = std::move(currentFiles); // Efficiently swap the content
    //}
    else if (path == L"..\\BoofWoof\\Assets\\Scenes") {
        // Compare currentFiles with the existing SceneFiles
        hasChanges = (currentFiles != SceneFiles);

        // Update SceneFiles with currentFiles
        SceneFiles = std::move(currentFiles); // Efficiently swap the content
    }
    else if (path == L"..\\BoofWoof\\Assets\\Objects") {
        // Compare currentFiles with the existing ObjectFiles
        hasChanges = (currentFiles != ObjectFiles);

        if (hasChanges) {
            std::wcout << L"Change detected in directory: " << path << L"\n";
            std::wcout << L"New state of files:\n";
            for (const auto& file : currentFiles) {
                std::wcout << file << L"\n";
            }
            std::wcout << L"Previous state of files:\n";
            for (const auto& file : ObjectFiles) {
                std::wcout << file << L"\n";
            }
        }


        // Update ObjectFiles with currentFiles
        ObjectFiles = std::move(currentFiles); // Efficiently swap the content
    }
    else if (path == L"..\\BoofWoof\\Assets\\Shaders") {
        // Compare currentFiles with the existing ShaderFiles
        hasChanges = (currentFiles != ShaderFiles);

        // Update ShaderFiles with currentFiles
        ShaderFiles = std::move(currentFiles); // Efficiently swap the content
    }
    else if (path == L"..\\BoofWoof\\Assets\\Fonts") {
        // Compare currentFiles with the existing FontFiles
        hasChanges = (currentFiles != FontFiles);

        // Update FontFiles with currentFiles
        FontFiles = std::move(currentFiles); // Efficiently swap the content
    }

    // Return true if changes were detected, otherwise false
    return hasChanges;
}

// Function to monitor texture files in a polling loop
void AssetManager::MonitorFiles(const std::wstring& path) {
    if (CheckFiles(path) && !Currentlyloading) {
        if (path == L"..\\BoofWoof\\Assets\\Art\\Textures") {
            //std::wcout << L"Changes detected." << std::endl;
            ReloadTextures();
        }
        /*else if (path == L"..\\BoofWoof\\Assets\\Art\\Sprites") {
            
        }*/
        else if (path == L"..\\BoofWoof\\Assets\\Scenes") {
            //std::wcout << L"Changes detected." << std::endl;
            ReloadScenes();
        }
        else if (path == L"..\\BoofWoof\\Assets\\Objects") {
            //std::wcout << L"Changes detected." << std::endl;
            ReloadObjects();
        }
        else if (path == L"..\\BoofWoof\\Assets\\Shaders") {
            //std::wcout << L"Changes detected." << std::endl;
            ReloadShaders();
        }
        else if (path == L"..\\BoofWoof\\Assets\\Fonts") {
            //std::wcout << L"Changes detected." << std::endl;
            ReloadFonts();
        }
    }
    else {
        //std::wcout << L"No changes detected." << std::endl;
    }
}

void AssetManager::DeleteAllFilesInDirectory(const std::string& directoryPath) {
    try {
        // Iterate over each item in the directory
        for (const auto& entry : fs::directory_iterator(directoryPath)) {
            if (fs::is_regular_file(entry.path())) {
                fs::remove(entry.path()); // Delete the file
            }
            // If you want to delete directories as well, you can use:
            // fs::remove_all(entry.path());
        }
        std::cout << "All files in the directory have been deleted." << std::endl;
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}





#pragma warning(pop)