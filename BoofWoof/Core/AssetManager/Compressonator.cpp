/**************************************************************************
 * @file Compressonator.cpp
 * @author 	Aaron Chan Jun Xiang
 * @param DP email: aaronjunxiang.chan@digipen.edu [2200880]
 * @param Course: CS 3401
 * @param Course: Game Project 3
 * @date  10/06/2024 (06 OCTOBER 2024)
 * @brief
 *
 * This file defines functions that uses compressonator
 *
 *************************************************************************/

#include "Compressonator.h"
#include "FilePaths.h"
#include "pch.h"


// Function to trim leading and trailing whitespaces from a string
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    size_t last = str.find_last_not_of(" \t\n\r");
    return (first == std::string::npos || last == std::string::npos) ? "" : str.substr(first, last - first + 1);
}

// Function to process the descriptor file
std::vector<std::string> processTextureDescriptorFile(const std::string& descriptorFilePath) {
    std::ifstream file(descriptorFilePath);

    if (!file.is_open()) {
        std::cerr << "Unable to open descriptor file: " << descriptorFilePath << std::endl;
        return {};
    }

    std::string line;
    std::string textureName;
    std::string textureFilePath;
    std::string resourceGuid;
    std::string resourceFilePath;
    std::string compressionFormat;

    std::vector<std::string> fileInfo{};

    while (std::getline(file, line)) {
        // Find the "Texture Name" line
        if (line.find("Texture Name") != std::string::npos) {
            textureName = trim(line.substr(line.find(":") + 1));
            fileInfo.push_back(textureName);
        }
        // Find the "Texture File Path" line
        if (line.find("Texture File Path") != std::string::npos) {
            textureFilePath = trim(line.substr(line.find(":") + 1));
            fileInfo.push_back(textureFilePath);
        }
        // Find the "Resource GUID" line
        if (line.find("Resource GUID") != std::string::npos) {
            resourceGuid = trim(line.substr(line.find(":") + 1));
            fileInfo.push_back(resourceGuid);
        }
        // Find the "Resource File Path" line
        else if (line.find("Resource File Path") != std::string::npos) {
            resourceFilePath = trim(line.substr(line.find(":") + 1));
            fileInfo.push_back(resourceFilePath);
        }
        // Find the "Compression Format" line
        else if (line.find("Compression Format") != std::string::npos) {
            compressionFormat = trim(line.substr(line.find(":") + 1));
            fileInfo.push_back(compressionFormat);
        }
    }


    /*
    outFile << "Texture Name : " << nameWithoutExtension << std::endl;
                    outFile << "Texture File Path : " << FILEPATH_TEXTURES + "\\" + entry.path().filename().string() << std::endl;
                    outFile << "Resource GUID : " << std::endl;
                    outFile << "Resource File Path : " << FILEPATH_TEXTURES_RESOURCE + "\\" + nameWithoutExtension + ".dds" << std::endl;
                    outFile << "Compression Format : "<< "-fd BC3";

    */

    std::cout << "\n**************************************************************************************\nCompressonator Print Out\n";
    // Print out the details from the descriptor file
    std::cout << "Texture Name: " << fileInfo[0] << std::endl;
    std::cout << "Texture File Path: " << fileInfo[1] << std::endl;
    std::cout << "Resource GUID: " << fileInfo[2] << std::endl;
    std::cout << "Resource File Path: " << fileInfo[3] << std::endl;
    std::cout << "Compression Format: " << fileInfo[4] << std::endl;
    std::cout << "\n**************************************************************************************\n";

    file.close();

    return fileInfo;
}

// Function to run a command line command
int runCommand(const std::string& command) {

    std::cout << "\n**************************************************************************************\n";
    std::cout << "Executing command: " << command << "\n";
    std::cout << "****************************************************************************************\n";
    // Execute the command and return the result (exit status)
    int result = std::system(command.c_str());

    // Check for error in execution
    if (result != 0) {
        std::cerr << "Command execution failed with exit code: " << result << std::endl;
    }
    else {
        std::cout << "Command executed successfully." << std::endl;
    }

    return result;  // Return the command's exit status
}

