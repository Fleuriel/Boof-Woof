// This person is not up to Pollos Standard - Gustavo Fring

#include "Header.h"
#include "Model.h"
#include <fstream>



GLFWwindow* createHiddenWindow();
bool fileExistsInDirectory(const std::string& directoryPath, const std::string& fileName);
std::vector<std::string> processDescriptorFile(const std::string& descriptorFilePath);
void saveMeshToBin(Model model, const std::string& binFilePath);
void parseOBJ(const std::string& filename, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture>& textureFiles);

std::string GetMtlFileName(const std::string& objFilePath);
std::vector<std::string> GetTextureFiles(const std::string& mtlFilePath);

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
    std::cout << "                                      MeshCompiler executed\n";
    

#ifdef _DEBUG
    for (int i = 0; i < argc; ++i)
    {
        std::cout << i << '\t' << argv[i] << '\n';
    }

    // [1] Descriptor File [TXT]
    // [2] Filepath of Descriptor
#endif


    if (argc < 2) {
        std::cerr << "Usage: MeshCompiler <path_to_directory_with_descriptors>" << std::endl;
        std::cout << "##################################################################################################\n";
        std::cout << "##################################################################################################\n";
        std::cout << "##################################################################################################\n";
        return -1;
    }

    
    std::string directoryPath = argv[1];
    size_t pos = directoryPath.find_last_of("\\/");

    // Extract the path up to that position
    std::string pathUpToObjects = (pos != std::string::npos) ? directoryPath.substr(0, pos) : directoryPath;



    if (!fs::is_directory(pathUpToObjects)) {
        std::cerr << "Provided path is not a directory: " << directoryPath << std::endl;

        std::cout << "##################################################################################################\n";
        std::cout << "##################################################################################################\n";
        std::cout << "##################################################################################################\n";
        return -1;
    }

    std::vector<std::string> descriptorFileInformation;


    std::string argumentFilepath;
    argumentFilepath = argv[1];


#ifdef _DEBUG
    std::cout << "Processing File: " << argumentFilepath << '\n';
#endif

    descriptorFileInformation = processDescriptorFile(argumentFilepath);

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
        std::cout << pathUpToObjects << '\n';
#endif;
        // Iterate over all descriptor files in the directory



        if (fileExistsInDirectory(descriptorFileInformation[1], descriptorFileInformation[0])) {

            // Load and convert the .obj file
            Model model;
            model.loadModel(objFilePath, GL_TRIANGLES);

#ifdef _DEBUG
            std::cout << model.textures_loaded.size() << '\n';
#endif
            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;
            std::vector<Texture> textures;
            parseOBJ(objFilePath, vertices, indices, textures);

#ifdef _DEBUG
            std::cout << vertices.size() << '\t' << indices.size() << "\t" <<  textures.size() << '\t' << "This Vertices ASize\n";
            std::cout << model.textures_loaded.size() << '\n';
#endif

            

            // Save the mesh data into a single .bin file
            if (!model.meshes.empty()) {
                saveMeshToBin(model, binFilePath);
                std::cout << "Converted " << objFilePath << " to " << binFilePath << std::endl;
            }
            else {
                std::cerr << "No mesh data found in " << objFilePath << std::endl;
            }
        }
        else
        {
            std::cout << "filepath does not exist: " << pathUpToObjects << '\t' << " is: " << fileExistsInDirectory(descriptorFileInformation[1], descriptorFileInformation[0]) << '\n';

        }
        break;
    }
    std::cout << "##################################### < EXECUTING FINISH > #######################################\n";
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


void saveMeshToBin(Model model, const std::string& binFilePath) {
    std::ofstream binFile(binFilePath, std::ios::binary);
    if (!binFile.is_open()) {
        std::cerr << "Failed to create binary file at saveAllMeshesToBin: " << binFilePath << std::endl;
        return;
    }

    std::vector<Mesh> meshes = model.meshes;
    
    

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

#ifdef _DEBUG
        std::cout << "texture size : mesh compiler: " << mesh.textures.size() << '\n';
#endif
        // Write textures
        size_t textureCount = model.textures_loaded.size();
        binFile.write(reinterpret_cast<const char*>(&textureCount), sizeof(size_t));
        for (int i = 0; i < model.textures_loaded.size(); ++i)
        {
#ifdef _DEBUG
            std::cout << "final\n";
            std::cout << model.textures_loaded[i].id << '\t' << model.textures_loaded[i].path << '\t' << model.textures_loaded[i].type << '\n';
#endif
            size_t pathLength = model.textures_loaded[i].path.size();
      
#ifdef _DEBUG
            std::cout << "path " << pathLength << '\n';
#endif

            binFile.write(reinterpret_cast<const char*>(&pathLength), sizeof(size_t));
            binFile.write(model.textures_loaded[i].path.c_str(), pathLength); // Texture path as a string
        }

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

    std::vector<std::string> outputVector;

    std::string line;
    //std::string line;
    //std::string fileName;
    //std::string objFilePath;
    //std::string binFilePath;
    //std::string MTLFile;
    //std::string PNGFile;
    //std::string JPGFile;

    // Read the file line by line
    //while (std::getline(file, line)) {
    //    // Check for "File Name" and extract the value
    //    if (line.find("File Name") != std::string::npos) {
    //        fileName = line.substr(line.find(":") + 2); // Extract name after ": "
    //        fileName += ".obj"; // Append .obj extension
    //    }
    //    // Check for "Source File Path" and extract the value
    //    else if (line.find("Source File Path") != std::string::npos) {
    //        objFilePath = line.substr(line.find(":") + 2); // Extract path after ": "
    //        objFilePath = objFilePath.substr(0, objFilePath.find_last_of("\\/")); // Get directory path
    //    }
    //    // Check for "Output File Path" and extract the value
    //    else if (line.find("Output File Path") != std::string::npos) {
    //        binFilePath = line.substr(line.find(":") + 2); // Extract path after ": "
    //    }
    //    else if (line.find("MTL File Exist") != std::string::npos)
    //    {
    //        MTLFile = line.substr(line.find(":") + 2);
    //    }
    //    else if (line.find("PNG File Exist") != std::string::npos)
    //    {
    //        PNGFile = line.substr(line.find(":") + 2);
    //    }
    //    else if (line.find("JPG File Exist") != std::string::npos)
    //    {
    //        JPGFile = line.substr(line.find(":") + 2);
    //    }
    //
    //
    //}


    // Read the file line by line
    while (std::getline(file, line)) {
        // Check for "File Name" and extract the value
        if (line.find("File Name") != std::string::npos) {
            std::string fileName = line.substr(line.find(":") + 2); // Extract name after ": "
            fileName += ".obj"; // Append .obj extension
            outputVector.push_back(fileName); // Add to vector
        }
        // Check for "Source File Path" and extract the value
        else if (line.find("Source File Path") != std::string::npos) {
            std::string objFilePath = line.substr(line.find(":") + 2); // Extract path after ": "
            objFilePath = objFilePath.substr(0, objFilePath.find_last_of("\\/")); // Get directory path
            outputVector.push_back(objFilePath); // Add to vector
        }
        // Check for "Output File Path" and extract the value
        else if (line.find("Output File Path") != std::string::npos) {
            std::string binFilePath = line.substr(line.find(":") + 2); // Extract path after ": "
            outputVector.push_back(binFilePath); // Add to vector
        }
        // Check for "MTL File Exist" and extract the value
        else if (line.find("MTL File Exist") != std::string::npos) {
            std::string MTLFile = line.substr(line.find(":") + 2); // Extract MTL file existence
            outputVector.push_back(MTLFile); // Add to vector
        }
        // Check for "PNG File Exist" and extract the value
        else if (line.find("PNG File Exist") != std::string::npos) {
            std::string PNGFile = line.substr(line.find(":") + 2); // Extract PNG file existence
            outputVector.push_back(PNGFile); // Add to vector
        }
        // Check for "JPG File Exist" and extract the value
        else if (line.find("JPG File Exist") != std::string::npos) {
            std::string JPGFile = line.substr(line.find(":") + 2); // Extract JPG file existence
            outputVector.push_back(JPGFile); // Add to vector
        }
    }

    file.close(); // Close the file
    return outputVector; // Return the vector with all information

   //file.close(); // Close the file
   //return { fileName, objFilePath, binFilePath }; // Return the extracted information
}


bool fileExistsInDirectory(const std::string& directoryPath, const std::string& fileName) {
    fs::path dirPath(directoryPath);
    fs::path filePath(fileName);

#ifdef _DEBUG
    std::cout << "checking " << dirPath << " against " << fileName << '\n';
#endif
    // Iterate over the directory to check for the exact file
    for (const auto& entry : fs::directory_iterator(dirPath)) {
        if (entry.path().filename() == filePath.filename()) {
            return true; // The file exists in the directory
        }
    }

    return false; // The file does not exist in the directory
}


void parseOBJ(const std::string& filename, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture>& textures) {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::string mtlFileName;

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
        else if (type == "mtllib") {
            ss >> mtlFileName; // Get the name of the material file
        }
        else if (type == "usemtl") {
            std::string materialName;
            ss >> materialName; // Get the name of the material being used
        }
        else if (type == "f") {
            unsigned int vertexIndex[3], texCoordIndex[3], normalIndex[3];
            char slash;

            for (int i = 0; i < 3; i++) {
                ss >> vertexIndex[i] >> slash >> texCoordIndex[i] >> slash >> normalIndex[i];
                vertexIndex[i]--; texCoordIndex[i]--; normalIndex[i]--;

                /*Vertex vertex;
                vertex.Position = positions[vertexIndex[i]];
                vertex.TexCoords = texCoords[texCoordIndex[i]];
                vertex.Normal = normals[normalIndex[i]];

                vertices.push_back(vertex);

                indices.push_back(static_cast<unsigned int>(vertices.size() - 1));*/
            }
            // calculate tangent and bitangent
            glm::vec3 edge1 = positions[vertexIndex[1]] - positions[vertexIndex[0]];
            glm::vec3 edge2 = positions[vertexIndex[2]] - positions[vertexIndex[0]];
            glm::vec2 deltaUV1 = texCoords[texCoordIndex[1]] - texCoords[texCoordIndex[0]];
            glm::vec2 deltaUV2 = texCoords[texCoordIndex[2]] - texCoords[texCoordIndex[0]];

            float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

            glm::vec3 tangent;
            tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
            tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
            tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
            tangent = glm::normalize(tangent);

            glm::vec3 bitangent;
            bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
            bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
            bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
            bitangent = glm::normalize(bitangent);

            for (int i = 0; i < 3; i++) {
                Vertex vertex;
                vertex.Position = positions[vertexIndex[i]];
                vertex.TexCoords = texCoords[texCoordIndex[i]];
                vertex.Normal = normals[normalIndex[i]];
                vertex.Tangent = tangent;
                vertex.Bitangent = bitangent;
                vertices.push_back(vertex);
                indices.push_back(static_cast<unsigned int>(vertices.size() - 1));
            }
        }
    }

    objFile.close();
#ifdef _DEBUG
    std::cout << "\n\n Opening Mtl File Now\n\n";
    std::filesystem::path currentPath = std::filesystem::current_path();

    std::cout << "Current directory: " << currentPath << std::endl;

#endif
    // Now read the MTL file to extract texture files
    if (!mtlFileName.empty()) {
        // Get the directory of the .obj file
        std::filesystem::path objPath(filename);
        std::filesystem::path mtlPath = objPath.parent_path() / mtlFileName; // Append mtl file name to directory

        std::ifstream mtlFile(mtlPath); // Use the full path to the MTL file
        if (!mtlFile.is_open()) {
            std::cerr << "Could not open MTL file: " << mtlPath << std::endl;
            return;
        }

        while (std::getline(mtlFile, line)) {
            std::stringstream ss(line);
            std::string materialType;
            ss >> materialType;

            // If this line specifies a texture (diffuse)
            if (materialType == "map_Kd") {
                std::string textureFile;
                ss >> textureFile;

                // Change the extension to .dds
                if (textureFile.find_last_of(".") != std::string::npos) {
                    textureFile.replace(textureFile.find_last_of("."), std::string::npos, ".dds");
                }


               // std::string prefix = "..\\BoofWoof\\Resources\\Textures\\";
                // Create a Texture object for each texture file
                Texture texture;
               // texture.id = model.textures_loaded[0].id;
               // texture.type = model.textures_loaded[0].type;
               //     // Assuming map_Kd is diffuse
               // //texture.path = prefix + textureFile;
               // texture.path = model.textures_loaded[0].path;
#ifdef _DEBUG
                std::cout << "path to texture: inside diffision mesh: " << texture.path << '\n';
#endif
                // You would typically load the texture here and set the `texture.id`
                // For example, texture.id = loadTextureFromFile(textureFile);

                textures.push_back(texture);
            }
        }

        mtlFile.close();
    }

#ifdef _DEBUG
    std::cout << "Object has been parsed with " << vertices.size() << " vertices and " << indices.size() << " indices.\n";
    std::cout << "Textures found: " << textures.size() << std::endl;
#else
 //   std::cout << "Object has been compiled\n";
#endif

}

std::string GetMtlFileName(const std::string& objFilePath) {
    std::ifstream objFile(objFilePath);
    std::string line;
    std::string mtlFileName;

    while (std::getline(objFile, line)) {
        // Look for the mtllib line
        if (line.substr(0, 7) == "mtllib ") {
            mtlFileName = line.substr(7); // Get the filename after "mtllib "
            break;
        }
    }

    objFile.close();
    return mtlFileName;
}

std::vector<std::string> GetTextureFiles(const std::string& mtlFilePath) {
    std::ifstream mtlFile(mtlFilePath);
    std::string line;
    std::vector<std::string> textureFiles;

    while (std::getline(mtlFile, line)) {
        // Look for the map_Kd line for diffuse textures
        if (line.substr(0, 6) == "map_Kd") {
            // Get the texture file path after "map_Kd "
            std::string textureFile = line.substr(7);
            // Change the extension to .dds
            if (textureFile.find_last_of(".") != std::string::npos) {
                textureFile.replace(textureFile.find_last_of("."), std::string::npos, ".dds");
            }
            textureFiles.push_back(textureFile);
        }
    }

    mtlFile.close();
    return textureFiles;
}