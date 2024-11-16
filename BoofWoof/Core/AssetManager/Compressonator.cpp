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
#include "pch.h"
#include "Compressonator.h"
#include "FilePaths.h"

// Function to trim leading and trailing whitespaces from a string
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    size_t last = str.find_last_not_of(" \t\n\r");
    return (first == std::string::npos || last == std::string::npos) ? "" : str.substr(first, last - first + 1);
}

int CompressTextureWithDescriptor(const TextureDescriptor& descriptor, const std::string& inputTexturePath, const std::string& outputTexturePath) {
    std::stringstream commandStream;

    // Base CompressonatorCLI command
    commandStream << "..\\lib\\Compressonator\\compressonatorcli.exe -fd ";

    // Set texture format based on descriptor
    if (descriptor.format == 0) {
        commandStream << "BC3";  // For example, default format is BC7
    }
    else {
        commandStream << descriptor.format;
    }

    // Set compression quality
    //commandStream << " -quality " << descriptor.compressionQuality;

    // Set other texture options based on descriptor fields (e.g., mipmaps, wrap mode, etc.)
    //if (descriptor.generateMipMaps) {
    //    commandStream << " -mipmaps";
    //}

    //if (descriptor.useCrunchCompression) {
    //    commandStream << " -crunch";
    //}

    //// Wrap mode
    //if (descriptor.wrapMode != "default_wrap") {
    //    commandStream << " -wrapmode " << descriptor.wrapMode;
    //}

    //// Filter mode
    //if (descriptor.filterMode != "default_filter") {
    //    commandStream << " -filtermode " << descriptor.filterMode;
    //}

    // Specify the input and output files
    commandStream << " " << inputTexturePath << " " << outputTexturePath;

    // Execute the command
    std::string command = commandStream.str();
    return runCommand(command);  // Call your existing runCommand function
}

// Function to run a command line command
int runCommand(const std::string& command) {

#ifdef _DEBUG
    std::cout << "\n**************************************************************************************\n";
    std::cout << "Executing command: " << command << "\n";
    std::cout << "****************************************************************************************\n";
#endif
    // Execute the command and return the result (exit status)
    int result = std::system(command.c_str());

    // Check for error in execution
    if (result != 0) {
        std::cerr << "Command execution failed with exit code: " << result << std::endl;
    }
    else {
#ifdef _DEBUG
        std::cout << "Command executed successfully." << std::endl;
#endif
    }

    return result;  // Return the command's exit status
}

