#include "Compressonator.h"
#include <iostream>
#include <fstream>
#include "FilePaths.h"


// Function to trim leading and trailing whitespaces from a string
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    size_t last = str.find_last_not_of(" \t\n\r");
    return (first == std::string::npos || last == std::string::npos) ? "" : str.substr(first, last - first + 1);
}

// Function to process the descriptor file
std::vector<std::string> processDescriptorFile(const std::string& descriptorFilePath) {
    std::ifstream file(descriptorFilePath);

    if (!file.is_open()) {
        std::cerr << "Unable to open descriptor file: " << descriptorFilePath << std::endl;
        return {};
    }

    std::string line;
    std::string fileName;
    std::string filePath;
    std::string compressionFormat;

    while (std::getline(file, line)) {
        // Find the "File Name" line
        if (line.find("File Name") != std::string::npos) {
            fileName = trim(line.substr(line.find(":") + 1));
        }
        // Find the "Compression Format" line
        else if (line.find("Compression Format") != std::string::npos) {
            compressionFormat = trim(line.substr(line.find(":") + 1));
        }
    }

    std::cout << "\n**************************************************************************************\n";
    // Print out the details from the descriptor file
    std::cout << "File Name: " << fileName << std::endl;
    std::cout << "Compression Format: " << compressionFormat << std::endl;
    std::cout << "\n**************************************************************************************\n";

    file.close();

    return { compressionFormat , fileName };
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

int test() {
    // Example path to the descriptor file
    std::string descriptorFilePath = "..\\BoofWoof\\Assets\\Descriptors\\Sadge.txt";

    // Process the descriptor file and print details
    std::vector<std::string> fileInfo = processDescriptorFile(descriptorFilePath);
    if (!fileInfo.empty()) {
        std::cout << fileInfo[0] << std::endl;
        std::cout << fileInfo[1] << std::endl;

        // Run compression command
        runCommand("..\\lib\\Compressonator\\compressonatorcli.exe " + fileInfo[0] + " ..\\" + fileInfo[1] + " " + FILEPATH_DESCRIPTORS);

        // Open the folder containing the file in Windows Explorer
        std::string folderPath = fileInfo[1].substr(0, fileInfo[1].find_last_of("\\/")); // Extract folder path from file path
        std::string openExplorerCommand = "explorer \"" + folderPath + "\"";
        runCommand(openExplorerCommand);  // Open the folder in Windows Explorer
    }

    return 0;
}
