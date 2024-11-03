// This person is not up to Pollos Standard - Gustavo Fring

#include "Header.h"
#include "Model.h"
#include <fstream>



GLFWwindow* createHiddenWindow();
bool fileExistsInDirectory(const std::string& directoryPath, const std::string& fileName);
std::vector<std::string> processDescriptorFile(const std::string& descriptorFilePath);
void saveMeshToBin(std::vector<Mesh> meshes, const std::string& binFilePath);
void parseOBJ(const std::string& filename, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);


namespace fs = std::filesystem;

int main(int argc, char** argv) {
    
    
    GLFWwindow* window = createHiddenWindow();
    if (!window) {
        std::cerr << "Failed to create a hidden window!" << std::endl;
        return -1;
    }




    std::cout << "##################################################################################################\n";
    std::cout << "##################################################################################################\n";
    std::cout << "##################################################################################################\n";
    std::cout << "                                  MeshCompiler executed\n";
    
    for (int i = 0; i < argc; ++i)
    {
        std::cout << argv[i] << '\n';
    }

    // [1] Descriptor File [TXT]
    // [2] Filepath of Descriptor
    // [3] [1][2] combined



    if (argc < 2) {
        std::cerr << "Usage: MeshCompiler <path_to_directory_with_descriptors>" << std::endl;
        return -1;
    }

    std::string directoryPath = argv[2];
    if (!fs::is_directory(directoryPath)) {
        std::cerr << "Provided path is not a directory: " << directoryPath << std::endl;
        return -1;
    }

    std::vector<std::string> descriptorFileInformation;



#ifdef _DEBUG
    std::cout << "Processing File: " << argv[3] << '\n';
#endif

    descriptorFileInformation = processDescriptorFile(argv[3]);

#ifdef _DEBUG
    
    std::cout << "#######################################################\n";
    std::cout << "#######################################################\n";
    std::cout << "Input Data:\n";

    for (int i = 0; i < descriptorFileInformation.size(); ++i)
    {
        std::cout << i << '\t' << descriptorFileInformation[i] << '\n';
    }
    std::cout << "#######################################################\n";
    std::cout << "#######################################################\n\n";
#endif


    std::string objFilePath, binFilePath;

    objFilePath = descriptorFileInformation[1] + "\\" + descriptorFileInformation[0];
    binFilePath = descriptorFileInformation[2];

    while (!glfwWindowShouldClose(window))
    {
#ifdef _DEBUG
        std::cout << "Loop has entered\n";
#endif;
        // Iterate over all descriptor files in the directory

        std::wcout << argv[2] << '\n';
        std::wcout << argv[1] << '\n';

        if (fileExistsInDirectory(argv[2], argv[1])) {

            // Load and convert the .obj file
            Model model;
            model.loadModel(objFilePath, GL_TRIANGLES);


            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;
            parseOBJ(objFilePath, vertices, indices);

#ifdef _DEBUG
            std::cout << vertices.size() << '\t' << indices.size() << "\t This Vertices ASize\n";
#endif

            // Create Mesh object and populate it with the vertices and indices
           // Mesh mesh;//(vertices, indices);

            //mesh.processMesh();

            // Now save the mesh to the .bin file
            saveMeshToBin(model.meshes, binFilePath);


            // Save the mesh data into a single .bin file
            if (!model.meshes.empty()) {
                saveMeshToBin(model.meshes, binFilePath);
                std::cout << "Converted " << objFilePath << " to " << binFilePath << std::endl;
            }
            else {
                std::cerr << "No mesh data found in " << objFilePath << std::endl;
            }
        }
        

        break;
    }

    std::cout << "##################################################################################################\n";
    std::cout << "##################################################################################################\n";
    std::cout << "##################################################################################################\n";


    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}


GLFWwindow* createHiddenWindow() {
    if (!glfwInit()) {
        return nullptr;
    }

    // Create a hidden window
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(1, 1, "Hidden Window", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    glewInit();

    return window;
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



std::vector<std::string> processDescriptorFile(const std::string& descriptorFilePath) {
    std::ifstream file(descriptorFilePath);

    // Check if the file opened successfully
    if (!file.is_open()) {
        std::cerr << "Unable to open descriptor file: " << descriptorFilePath << std::endl;
        return {};
    }

    std::string line;
    std::string fileName;
    std::string objFilePath;
    std::string binFilePath;

    // Read the file line by line
    while (std::getline(file, line)) {
        // Check for "File Name" and extract the value
        if (line.find("File Name") != std::string::npos) {
            fileName = line.substr(line.find(":") + 2); // Extract name after ": "
            fileName += ".obj"; // Append .obj extension
        }
        // Check for "Source File Path" and extract the value
        else if (line.find("Source File Path") != std::string::npos) {
            objFilePath = line.substr(line.find(":") + 2); // Extract path after ": "
            objFilePath = objFilePath.substr(0, objFilePath.find_last_of("\\/")); // Get directory path
        }
        // Check for "Output File Path" and extract the value
        else if (line.find("Output File Path") != std::string::npos) {
            binFilePath = line.substr(line.find(":") + 2); // Extract path after ": "
        }
    }

    file.close(); // Close the file
    return { fileName, objFilePath, binFilePath }; // Return the extracted information
}


bool fileExistsInDirectory(const std::string& directoryPath, const std::string& fileName) {
    fs::path dirPath(directoryPath);
    fs::path filePath(fileName);

    // Iterate over the directory to check for the exact file
    for (const auto& entry : fs::directory_iterator(dirPath)) {
        if (entry.path().filename() == filePath.filename()) {
            return true; // The file exists in the directory
        }
    }

    return false; // The file does not exist in the directory
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

    std::cout << "Object has been parsed\n";
    objFile.close();
}

