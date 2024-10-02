
/**************************************************************************
 * @file AssetManager.cpp
 * @author 	TAN Angus Yit Hoe
 * @param DP email: tan.a@digipen.edu [0067684]
 * @param Course: CS 350
 * @param Course: Advanced Computer Graphics II
 * @date  06/14/2024 (14 JUNE 2024)
 * @brief
 *
 * This file allows assets to be loaded into containers to be used by
 * the application.
 *
 *
 *************************************************************************/
#define MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS 0

#include <filesystem>
#include <iostream>
#include "AssetManager.h"
#include "windows.h"
#include "FilePaths.h"
//#include <stb_image.h>

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

///**************************************************************************
// * @brief Load All Assets (Unimplemented)
// *************************************************************************/
//void AssetManager::LoadAll()
//{
//
//}
///**************************************************************************
// * @brief Free Assets (Unimplemented)
// *************************************************************************/
//void AssetManager::Free()
//{
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

    if (fs::is_directory(FILEPATH_TEXTURES)) {
        for (const auto& entry : fs::directory_iterator(FILEPATH_TEXTURES)) {
            std::string texFilePath = FILEPATH_TEXTURES + "/" + entry.path().filename().string();
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


                textures[nameWithoutExtension] = AssetManager::SetUpTexture(texFilePath);
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
 * @brief Sets up a texture and returns its handler.
 *
 * This function takes a file path to an image, loads it as an OpenGL texture,
 * and returns the texture handler. It's used internally for loading textures.
 *
 * @param std::string filePath - The path to the texture image file.
 * @return int - The OpenGL texture handler.
 *************************************************************************/
int AssetManager::SetUpTexture(std::string filePath) {
    GLuint textureObj_Handler; // OpenGL texture object handler

    int width, height, channels;

    // Load the image from the file using stb_image
    unsigned char* image = stbi_load(filePath.c_str(), &width, &height, &channels, STBI_rgb_alpha);

    if (!image) {
        // If the image loading fails, print an error message and return 0 (failure)
#ifdef _DEBUG
        std::cout << "Failed to load texture: " << filePath << std::endl;
#endif // DEBUG
        return 0; // Return 0 to indicate failure
    }

    // Create an OpenGL texture object
    glCreateTextures(GL_TEXTURE_2D, 1, &textureObj_Handler);

    // Allocate storage for the texture with RGBA8 format
    glTextureStorage2D(textureObj_Handler, 1, GL_RGBA8, width, height);

    // Upload the image data to the texture object
    glTextureSubImage2D(textureObj_Handler, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, image);

    // Free the image data after it's uploaded to OpenGL
    stbi_image_free(image);

    // Return the OpenGL texture object handler
    return textureObj_Handler;
}

/**************************************************************************
 * @brief Loads sprite data from the specified directory and populates the 'sprites' container.
 *
 * This method scans the directory specified by FILEPATH_SPRITES for sprite files, creates Sprite
 * instances for each sprite file found, and stores them in the 'sprites' container.
 * Each sprite file is expected to follow the naming convention, including the number of rows
 * and columns in the sprite sheet, e.g., "example_sprite(2x3).png".
 *
 * @return True if the sprite data is loaded successfully, false if there was an error or the directory does not exist.
 *************************************************************************/
bool AssetManager::LoadSprites() {

    Currentlyloading = true;

    // if file path for sprites exist
    if (fs::is_directory(FILEPATH_SPRITES)) {
        // for every sprite in the file path
        for (const auto& entry : fs::directory_iterator(FILEPATH_SPRITES)) {
            // get the file path for the sprite
            std::string spriteFilePath = FILEPATH_SPRITES + "/" + entry.path().filename().string();
            //std::cout << "Sprite file " << spriteFilePath << " Found." << std::endl;

            // find the file extension 
            size_t extensionPos = entry.path().filename().string().find_last_of('.');
            // if file extension found
            if (extensionPos != std::string::npos) {

                std::string Extension = entry.path().filename().string().substr(extensionPos);
                //std::cout << Extension;
                std::string allowedExtensions = ".png";

                // Check if the substring exists in the full string
                size_t found = allowedExtensions.find(toLowerCase(Extension));

                if (found == std::string::npos) {
                    std::string file(entry.path().filename().string());
                    std::wstring widefile(file.begin(), file.end());
                    HWND hwnd = GetActiveWindow();
                    std::string filepath(FILEPATH_SPRITES);
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

                bool correctnamingconvention{ true };

                // find '(' in the name
                size_t lBracketPos = entry.path().filename().string().find_last_of('(');

                // find ')' in the name
                size_t rBracketPos = entry.path().filename().string().find_last_of(')');

                int rows{}, columns{};

                // Check if both '(' and ')' are present and in the correct order
                if (lBracketPos != std::string::npos && rBracketPos != std::string::npos && lBracketPos < rBracketPos) {
                    // Get the string containing rows and columns from the name
                    std::string spriteRowsAndColumns = entry.path().filename().string().substr(lBracketPos + 1, rBracketPos - lBracketPos - 1);

                    // Find the 'x' in the string that separates the rows and columns
                    size_t xPos = spriteRowsAndColumns.find_last_of('x');

                    // Check if 'x' is present
                    if (xPos != std::string::npos) {
                        try {
                            // Get the rows
                            rows = std::stoi(spriteRowsAndColumns.substr(0, xPos));

                            // Get the columns
                            columns = std::stoi(spriteRowsAndColumns.substr(xPos + 1));
                        }
                        catch (const std::invalid_argument& e) {
                            e;
                            correctnamingconvention = false;
                        }
                        catch (const std::out_of_range& e) {
                            e;
                            correctnamingconvention = false;
                        }
                    }
                    else
                        correctnamingconvention = false;
                }
                else {
                    correctnamingconvention = false;
                }

                if (!correctnamingconvention) {
                    std::string file(entry.path().filename().string());
                    std::wstring widefile(file.begin(), file.end());
                    HWND hwnd = GetActiveWindow();
                    std::string filepath(FILEPATH_SPRITES);
                    // Convert std::string to std::wstring
                    std::wstring widefilepath(filepath.begin(), filepath.end());

                    std::wstring message = L"File with incompatible naming convention (\"" + widefile + L"\") detected in \"" + widefilepath + L"\" folder!\n\nFile not loaded!";
                    LPCWSTR boxMessage = message.c_str();

                    MessageBox(hwnd, boxMessage, L"Load Failure", MB_OK | MB_ICONERROR);
                    continue;
                }

                // create new sprite class
                Sprite newsprite;

                // set the texture
                newsprite.SetTexture(AssetManager::SetUpTexture(spriteFilePath));
                //std::cout << textures[nameWithoutExtension] << " success!\n";

                // set the rows and columns of the sprite
                newsprite.SetRowsAndColumns(rows, columns);

                // Bind the texture
                glBindTexture(GL_TEXTURE_2D, newsprite.GetTexture());

                // Get the width and height of the texture
                int texWidth, texHeight;
                glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texWidth);
                glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texHeight);

                // Unbind the texture
                glBindTexture(GL_TEXTURE_2D, 0);

                // Store width and height of texture
                newsprite.SetTextureWidthAndHeight(texWidth, texHeight);
                //std::cout << "Texture Width : " << texWidth << std::endl << "Texture Height : " << texHeight << std::endl;

                // Store width and height of each "image" in texture
                newsprite.SetWidthAndHeight(newsprite.GetTextureWidth() / newsprite.GetColumns(), newsprite.GetTextureHeight() / newsprite.GetRows());
                //std::cout << "Width : " << newsprite.GetWidth() << std::endl << " Height : " << newsprite.GetHeight() << std::endl;

                // get the name of the sprite sheet
                std::string nameWithoutExtension = entry.path().filename().string().substr(0, lBracketPos);
                //std::cout << nameWithoutExtension << std::endl;

                // store the sprite in the sprite container
                sprites[nameWithoutExtension] = newsprite;
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
 * @brief Retrieves the texture ID associated with a sprite by its name.
 *
 * This function searches for a sprite with the specified name and returns
 * the associated texture ID. If the sprite does not exist, it returns 0.
 *
 * @param name The name of the sprite to retrieve the texture for.
 * @return The texture ID of the sprite or 0 if the sprite is not found.
 *************************************************************************/
Sprite AssetManager::GetSprite(std::string name) {
    return sprites[name];
}

int AssetManager::GetSpriteTexture(std::string name) {
    return GetSprite(name).GetTexture();
}

/**************************************************************************
 * @brief Clears the sprite container and frees associated resources.
 *
 * This function clears the sprite container, releasing any associated
 * resources, and returns true if the container is empty afterward.
 *
 * @return True if the sprite container is empty after clearing, false otherwise.
 *************************************************************************/
bool AssetManager::FreeSprites() {
    sprites.clear();
    return sprites.empty();
}

/**************************************************************************
 * @brief Reloads sprites by clearing the container and loading new ones.
 *
 * This function first clears the sprite container, releasing any associated
 * resources. Then, it loads new sprite assets. It returns true if both
 * clearing and loading are successful.
 *
 * @return True if sprites are successfully reloaded, false otherwise.
 *************************************************************************/
bool AssetManager::ReloadSprites() {
    return (AssetManager::FreeSprites() && AssetManager::LoadSprites());
}



/**************************************************************************
 * @brief Load Shaders
 * @param bool true or false depending on failure or success
 *************************************************************************/
bool AssetManager::LoadShaders() {
	
	std::cout << "Load Shaders\n";

	std::cout << "We are currently working on : " << std::filesystem::current_path().string() << '\n';

	//VectorPairString SHADER3D
	//{
	//	std::make_pair<std::string, std::string>
	//	("Assets\\Shaders\\Shader3D.vert", "Assets\\Shaders\\Shader3D.frag")
	//};
	VectorPairString SHADER3D
	{
		std::make_pair<std::string, std::string>
		("../BoofWoof/Assets/Shaders/Shader3D.vert", "../BoofWoof/Assets/Shaders/Shader3D.frag")
	};


	VectorPairString SHADER2D
	{
		std::make_pair<std::string, std::string>
		("../BoofWoof/Assets/Shaders/Shader2D.vert", "../BoofWoof/Assets/Shaders/Shader2D.frag")
	};

	InitShdrpgms(SHADER3D);
	InitShdrpgms(SHADER2D);

	if (shdrpgms.size() == 0)
	{
		std::cout << "shader program size is 0\n";
		return false;
	}
	return true;
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
 * @brief Free Shaders (Unimplemented)
 *************************************************************************/
bool AssetManager::FreeShaders()
{
	return false;
}
