
#include "Header.h"

#include "Model.h"
#include <fstream>





void processDescriptorFile(const std::string& descriptorFilePath, std::string& objFilePath, std::string& binFilePath);
void saveMeshToBin(std::vector<Mesh> meshes, const std::string& binFilePath);

namespace fs = std::filesystem;

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: MeshCompiler <path_to_directory_with_descriptors>" << std::endl;
        return -1;
    }

    std::string directoryPath = argv[1];
    if (!fs::is_directory(directoryPath)) {
        std::cerr << "Provided path is not a directory: " << directoryPath << std::endl;
        return -1;
    }

    // Iterate over all descriptor files in the directory
    for (const auto& entry : fs::directory_iterator(directoryPath)) {
        if (entry.path().extension() == ".txt") {
            std::string descriptorFilePath = entry.path().string();
            std::string objFilePath, binFilePath;

            // Read paths from descriptor file
            processDescriptorFile(descriptorFilePath, objFilePath, binFilePath);

            if (objFilePath.empty() || binFilePath.empty()) {
                std::cerr << "Error: Missing file paths in descriptor " << descriptorFilePath << std::endl;
                continue;
            }

            // Load and convert the .obj file
            Model model;
            model.loadModel(objFilePath, GL_TRIANGLES);

            // Save the mesh data into a single .bin file
            if (!model.meshes.empty()) {
                saveMeshToBin(model.meshes, binFilePath);
                std::cout << "Converted " << objFilePath << " to " << binFilePath << std::endl;
            }
            else {
                std::cerr << "No mesh data found in " << objFilePath << std::endl;
            }
        }
    }
    return 0;
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



void processDescriptorFile(const std::string& descriptorFilePath, std::string& objFilePath, std::string& binFilePath) {
    std::ifstream descriptorFile(descriptorFilePath);
    std::string line;
    if (!descriptorFile.is_open()) {
        std::cerr << "Error opening descriptor file: " << descriptorFilePath << std::endl;
        return;
    }

    while (getline(descriptorFile, line)) {
        std::istringstream iss(line);
        std::string key;
        if (line.find("Source File Path:") != std::string::npos) {
            objFilePath = line.substr(line.find(":") + 2);  // Extract path after ": "
        }
        else if (line.find("Output File Path:") != std::string::npos) {
            binFilePath = line.substr(line.find(":") + 2);
        }
    }
    descriptorFile.close();
}