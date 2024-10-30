/**************************************************************************
 * @file ResourceManager.cpp
 * @author 	Aaron Chan Jun Xiang
 * @param DP email: aaronjunxiang.chan@digipen.edu [2200880]
 * @param Course: CS 3401
 * @param Course: Game Project 3
 * @date  10/06/2024 (06 OCTOBER 2024)
 * @brief
 *
 * This file loads resources to be used
 *
 *************************************************************************/
 
#include "pch.h"
#pragma warning(push)
#pragma warning(disable : 4244 4458 4100 5054)

#include "ResourceManager.h"
#include "AssetManager/FilePaths.h"
#include <gli/gli.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STBI_ONLY_DDS // Enable support for DDS only, if you don't want other formats

#pragma warning(pop)


ResourceManager g_ResourceManager;

// Constructor
ResourceManager::ResourceManager() {
    // Initialization if needed
}

// Destructor
ResourceManager::~ResourceManager() {
    // Cleanup resources if necessary
}


void AddModelFromOwnCreation()
{
    Model model;

    model = SquareModel(glm::vec3(0.0f));
    g_ResourceManager.SetModelMap(model.name, model);
    std::cout << "Loaded: " << model.name << " [Models Reference: "
        << g_ResourceManager.GetModelMap().size() - 1 << "]" << '\n';

    g_ResourceManager.addModelNames(model.name);


    // Create CubeModel and add it to ModelMap
    model = CubeModel(glm::vec3(1.0f));
    g_ResourceManager.SetModelMap(model.name, model);
    std::cout << "Loaded: " << model.name << " [Models Reference: "
        << g_ResourceManager.GetModelMap().size() - 1 << "]" << '\n';

    g_ResourceManager.addModelNames(model.name);
}



bool ResourceManager::LoadAll() {
    LoadTexturesDDS();
    AddModelFromOwnCreation();
    LoadModelBinary();
    //std::cout << "load 1 : " << GetTextureDDS("texture1.dds");
    //std::cout << "load 2 : " << GetTextureDDS("texture2.dds");
    //std::cout << "load 3 : " << GetTextureDDS("texture3.dds");

    return true;
}


GLuint LoadDDSTexture(const char* filePath) {
    // Load the DDS file
    gli::texture Texture = gli::load(filePath);
    if (Texture.empty()) {
        std::cerr << "Failed to load DDS file: " << filePath << std::endl;
        return 0; // Return 0 on failure
    }

    // Check if the format is compressed
    if (!gli::is_compressed(Texture.format())) {
        std::cerr << "Loaded DDS is not a compressed format!" << std::endl;
        return 0;
    }

    // Get OpenGL texture parameters
    gli::gl GL(gli::gl::PROFILE_GL33);
    gli::gl::format const Format = GL.translate(Texture.format(), Texture.swizzles());
    GLenum Target = GL.translate(Texture.target());
    //assert(Target == gli::TARGET_2D); // Ensure the target is 2D

    GLuint TextureName = 0;
    glGenTextures(1, &TextureName);
    glBindTexture(Target, TextureName);

    // Set texture parameters
    glTexParameteri(Target, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(Target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(Texture.levels() - 1));
    glTexParameteriv(Target, GL_TEXTURE_SWIZZLE_RGBA, &Format.Swizzles[0]);

    // Allocate storage for the texture
    glTexStorage2D(Target, static_cast<GLint>(Texture.levels()), Format.Internal,
        Texture.extent(0).x, Texture.extent(0).y);

    // Upload the texture data for each mipmap level
    for (std::size_t Level = 0; Level < Texture.levels(); ++Level) {
        glm::tvec3<GLsizei> Extent(Texture.extent(Level)); // Get the extent for the current level
        glCompressedTexSubImage2D(
            Target, static_cast<GLint>(Level), 0, 0, Extent.x, Extent.y,
            Format.Internal, static_cast<GLsizei>(Texture.size(Level)), Texture.data(0, 0, Level)
        );
    }

    // Optionally set additional texture parameters
    glTexParameteri(Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(Target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(Target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(Target, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Unbind the texture
    glBindTexture(Target, 0);

    return TextureName; // Return the OpenGL texture ID
}



bool LoadBinFile(std::string filePath)
{
    std::ifstream binFile(filePath, std::ios::binary);
    if (!binFile.is_open()) {
        std::cerr << "Failed to open binary file: " << filePath << std::endl;
        return {};
    }

    // Step 1: Read the number of meshes
    size_t meshCount;
    binFile.read(reinterpret_cast<char*>(&meshCount), sizeof(size_t));

    std::vector<Mesh> meshes;
    meshes.reserve(meshCount);

    // Step 2: For each mesh, read vertex and index data
    for (size_t i = 0; i < meshCount; ++i) {
        // Read vertex data
        size_t vertexCount;
        binFile.read(reinterpret_cast<char*>(&vertexCount), sizeof(size_t));
        std::vector<Vertex> vertices(vertexCount);
        binFile.read(reinterpret_cast<char*>(vertices.data()), vertexCount * sizeof(Vertex));

        // Read index data
        size_t indexCount;
        binFile.read(reinterpret_cast<char*>(&indexCount), sizeof(size_t));
        std::vector<unsigned int> indices(indexCount);
        binFile.read(reinterpret_cast<char*>(indices.data()), indexCount * sizeof(unsigned int));

        // Create the Mesh object and add it to the vector
        meshes.emplace_back(vertices, indices);
    }

    binFile.close();

    std::cout << "Loaded mesh from " << filePath << " successfully!" << std::endl;

    
    std::cout << "********************************************\n";
    std::cout << "Mesh contains: " << meshes.size() << '\t' << meshes.size() << '\n';
    
    std::cout << "Attempting to load into Model Map\n";
    std::cout << "********************************************\n";



    std::filesystem::path p(filePath);

    std::string fileName = p.stem().string();
    
    Model model;
    
    model.name = fileName;
    model.meshes = meshes;
//    meshes[0].setupMesh();

    g_ResourceManager.SetModelMap(model.name, model);


    //g_ResourceManager.addModelNames(model.name);


    return true;
}



std::vector<std::string> ResourceManager::getModelNames() const {
    return ModelNames; // Return a copy of the vector
}

void ResourceManager::addModelNames(std::string modelName)
{
    ModelNames.push_back(modelName);
}

const std::map<std::string, Model>& ResourceManager::GetModelMap() const {
    return ModelMap;
}

bool ResourceManager::SetModelMap(const std::string& name, const Model& model) {
    if (name.size() == 0)
        return false;


    
    ModelMap.insert(std::make_pair(name, model));

//    ModelNames.push_back(name);
}

Model* ResourceManager::getModel(const std::string& modelName) {
    auto it = ModelMap.find(modelName);
    if (it != ModelMap.end()) {
        return &(it->second);  // Return pointer to the model
    }
    return nullptr;  // Return nullptr if not found
}

// Setter for ModelMap (add a new model or update existing one)
void ResourceManager::setModel(const std::string& modelName, const Model& model) {
    ModelMap[modelName] = model;  // Insert or update the model
}

// Optional: Check if a model exists in the map
bool ResourceManager::hasModel(const std::string& modelName) {
    return ModelMap.find(modelName) != ModelMap.end();
}






/* Resource Manager Functions */


bool ResourceManager::LoadModelBinary()
{
    for (int i = 0; i < ModelNames.size(); ++i)
    {
        if (ModelNames[i] == "Square" || ModelNames[i] == "cubeModel")
            continue;

        std::cout << ModelNames[i] << '\n';
        LoadBinFile(FILEPATH_OBJECTS_RESOURCE + "\\" + ModelNames[i] + ".bin");
    }

    return true;
}


bool ResourceManager::AddModelBinary(std::string fileName)
{
    if (fileName.size() == 0)
    {
        std::cerr << "Filename does not contain anything " << fileName << '\n';
        return false;
    }
 

    ModelNames.push_back(fileName);

    return true;
}



// Function to load textures (simulating DDS texture loading here)
bool ResourceManager::LoadTexturesDDS() {

    for (int i = 0; i < textureDDSFileNames.size(); i++) {
        //
        // std::cout << "names:" << textureDDSFileNames[i].c_str() << "\n";

        //add DDS processing here
        int result = LoadDDSTexture((FILEPATH_TEXTURES_RESOURCE + "\\" + textureDDSFileNames[i] + ".dds").c_str());



        if (result != -1) {
            texturesDDS[textureDDSFileNames[i]] = result;
            std::cout << "Texture DDS File Added : " << textureDDSFileNames[i] << std::endl;
        }
        else
        {
            std::cout << "ji pai siao liao\n";
        }

    }
    // For now, returning true to indicate successful loading
    return true;
}

bool ResourceManager::AddTextureDDS(std::string textureName) {
    textureDDSFileNames.push_back(textureName);



    for (int i = 0; i < textureDDSFileNames.size(); ++i)
    {
        std::cout << "Contains: " << i << '\t' << textureDDSFileNames[i].c_str() << '\n';
    }

    return true;
}

// Function to access textures
int ResourceManager::GetTextureDDS(std::string textureName) {
    // Check if the texture exists in the map
    if (texturesDDS.find(textureName) != texturesDDS.end()) {
        //std::cout << "Texture is Found!" << textureName << '\n';
        // Return the texture ID if found
        return texturesDDS[textureName];
    }
    else {
        // Return -1 or some error value if texture not found
        //std::cerr << "Texture not found: " << textureName << std::endl;
        return -1;
    }
}


