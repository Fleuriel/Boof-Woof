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
    commandStream << "..\\lib\\Compressonator\\compressonatorcli.exe ";

    if (descriptor.generateMipMaps == false) {
        commandStream << "-nomipmap ";
    }

    if (descriptor.sRGB == true) {
        commandStream << "-UseSRGBFrames ";
    }

    commandStream << "-fd ";

    // Set texture format based on descriptor
    switch (descriptor.format) {
    case 0: // RGB Uncompressed
        commandStream << "NONE";
        break;

    case 1: // ATC_RGB
        commandStream << "ATC_RGB";
        break;

    case 2: // ATC_RGBA_Explicit
        commandStream << "ATC_RGBA_Explicit";
        break;

    case 3: // ATC_RGBA_Interpolated
        commandStream << "ATC_RGBA_Interpolated";
        break;

    case 4: // ATI1N
        commandStream << "ATI1N";
        break;

    case 5: // ATI2N
        commandStream << "ATI2N";
        break;

    case 6: // ATI2N_XY
        commandStream << "ATI2N_XY";
        break;

    case 7: // ATI2N_DXT5
        commandStream << "ATI2N_DXT5";
        break;

    case 8: // BC1
        commandStream << "BC1";
        break;

    case 9: // BC2
        commandStream << "BC2";
        break;

    case 10: // BC3
        commandStream << "BC3";
        break;

    case 11: // BC4
        commandStream << "BC4";
        break;

    case 12: // BC4_S
        commandStream << "BC4_S";
        break;

    case 13: // BC5
        commandStream << "BC5";
        break;

    case 14: // BC5_S
        commandStream << "BC5_S";
        break;

    case 15: // BC6H
        commandStream << "BC6H";
        break;

    case 16: // BC7
        commandStream << "BC7";
        break;

    case 17: // DXT1
        commandStream << "DXT1";
        break;

    case 18: // DXT3
        commandStream << "DXT3";
        break;

    case 19: // DXT5
        commandStream << "DXT5";
        break;

    case 20: // DXT5_xGBR
        commandStream << "DXT5_xGBR";
        break;

    case 21: // DXT5_RxBG
        commandStream << "DXT5_RxBG";
        break;

    case 22: // DXT5_RBxG
        commandStream << "DXT5_RBxG";
        break;

    case 23: // DXT5_xRBG
        commandStream << "DXT5_xRBG";
        break;

    case 24: // DXT5_RGxB
        commandStream << "DXT5_RGxB";
        break;

    case 25: // DXT5_xGxR
        commandStream << "DXT5_xGxR";
        break;

    default:
        std::cerr << "Unsupported format in descriptor!" << std::endl;
        return -1;
    }

    

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

