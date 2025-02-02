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
#include <gli/load_dds.hpp>


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
    //LoadTexturesDDS();
    AddModelFromOwnCreation();
    LoadModelBinary();
    //std::cout << "load 1 : " << GetTextureDDS("texture1.dds");
    //std::cout << "load 2 : " << GetTextureDDS("texture2.dds");
    //std::cout << "load 3 : " << GetTextureDDS("texture3.dds");

    return true;
}


DDSData LoadDDS(const char* filePath, GLuint existingTexture = 0) {
    
    DDSData ddsData;



    // Load the DDS file
    gli::texture Texture = gli::load(filePath);
    if (Texture.empty()) {
        std::cerr << "Failed to load DDS file: " << filePath << std::endl;
        return ddsData; // Return 0 on failure
    }

    // Check if the format is compressed
    if (!gli::is_compressed(Texture.format())) {
        std::cerr << "Loaded DDS is not a compressed format!" << std::endl;
        return ddsData;
    }


  
    // Get OpenGL texture parameters
    gli::gl GL(gli::gl::PROFILE_GL33);
    gli::gl::format const Format = GL.translate(Texture.format(), Texture.swizzles());
    GLenum Target = GL.translate(Texture.target());
    glm::tvec3<GLsizei> extent(Texture.extent());
    //assert(Target == gli::TARGET_2D); // Ensure the target is 2D


    if (Target != GL_TEXTURE_2D) {
        std::cerr << "Error: DDS texture is not 2D!" << std::endl;
        return ddsData;
    }

    GLuint TextureName = existingTexture;
    if (TextureName == 0)
    {
        glGenTextures(1, &TextureName);
    }
    else
    {
        glDeleteTextures(1, &TextureName);
    }

    if (Texture.levels() <= 0) {
        std::cerr << "Error: DDS texture does not contain mipmaps!" << std::endl;
        return ddsData;
    }

    std::cout << "Internal format: " << Format.Internal << std::endl;
    if (Format.Internal != GL_COMPRESSED_RGBA_S3TC_DXT5_EXT) {
        std::cerr << "Warning: Internal format mismatch!" << std::endl;
    }
    
    
    glBindTexture(Target, TextureName);


    std::cout << "Format Type External Internal: " << Format.Type << '\t' << Format.External << '\t' << Format.Internal << '\n';
    


    // Set texture parameters
    glTexParameteri(Target, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(Target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(Texture.levels() - 1));
//    glTexParameteriv(Target, GL_TEXTURE_SWIZZLE_RGBA, &Format.Swizzles[0]);
    //glTexParameteri(Target, GL_TEXTURE_RB, GL_TRUE);

    // Allocate storage for the texture
    glTexStorage2D(Target, static_cast<GLint>(Texture.levels()), Format.Internal,
        Texture.extent(0).x, Texture.extent(0).y);


    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // BC3/DXT5 has 1-byte alignment

    // Upload the texture data for each mipmap level
    for (std::size_t Level = 0; Level < Texture.levels(); ++Level) {
        glm::tvec3<GLsizei> Extent(Texture.extent(Level)); // Get the extent for the current level
        glCompressedTexSubImage2D(
            Target, static_cast<GLint>(Level), 0, 0, Extent.x, Extent.y,
            Format.Internal, static_cast<GLsizei>(Texture.size(Level)), Texture.data(0, 0, Level)
        );

        ddsData.Width = Extent.x;
        ddsData.Height = Extent.y;

    }

    // Optionally set additional texture parameters
    glTexParameteri(Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(Target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(Target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(Target, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Unbind the texture
    glBindTexture(Target, 0);


    ddsData.ID = TextureName;

    return ddsData; // Return the OpenGL texture ID
}



bool LoadBinFile(const std::string& filePath) {
    std::ifstream binFile(filePath, std::ios::binary);
    if (!binFile.is_open()) {
        std::cerr << "Failed to open binary file: " << filePath << std::endl;
        return false;
    }

    try {
        // Step 1: Read the number of meshes
        size_t meshCount;
        if (!binFile.read(reinterpret_cast<char*>(&meshCount), sizeof(size_t))) {
            throw std::runtime_error("Failed to read mesh count.");
        }

        std::vector<Mesh> meshes;
        meshes.reserve(meshCount);

        // Step 2: For each mesh, read vertex, index, and texture data
        for (size_t i = 0; i < meshCount; ++i) {
            // Read vertices
            size_t vertexCount;
            if (!binFile.read(reinterpret_cast<char*>(&vertexCount), sizeof(size_t))) {
                throw std::runtime_error("Failed to read vertex count for mesh.");
            }
            std::vector<Vertex> vertices(vertexCount);
            if (!binFile.read(reinterpret_cast<char*>(vertices.data()), vertexCount * sizeof(Vertex))) {
                throw std::runtime_error("Failed to read vertex data for mesh.");
            }

            // Read indices
            size_t indexCount;
            if (!binFile.read(reinterpret_cast<char*>(&indexCount), sizeof(size_t))) {
                throw std::runtime_error("Failed to read index count for mesh.");
            }
            std::vector<unsigned int> indices(indexCount);
            if (!binFile.read(reinterpret_cast<char*>(indices.data()), indexCount * sizeof(unsigned int))) {
                throw std::runtime_error("Failed to read index data for mesh.");
            }

            // Read textures
            size_t textureCount;
            if (!binFile.read(reinterpret_cast<char*>(&textureCount), sizeof(size_t))) {
                throw std::runtime_error("Failed to read texture count for mesh.");
            }
            std::vector<Texture> textures;
#ifdef _DEBUG
            std::cout << textureCount << '\n';
#endif
            textures.reserve(textureCount);
            for (size_t j = 0; j < textureCount; ++j) {
                Texture texture;

                // Read the path length
                size_t pathLength;
                if (!binFile.read(reinterpret_cast<char*>(&pathLength), sizeof(size_t))) {
                    throw std::runtime_error("Failed to read texture path length.");
                }

                // Read the path
                texture.path.resize(pathLength);
                if (!binFile.read(&texture.path[0], pathLength)) {
                    throw std::runtime_error("Failed to read texture path.");
                }
#ifdef _DEBUG
                std::cout << texture.path << '\n';
#endif
                size_t dotPos = texture.path.find_last_of(".");
                if (dotPos != std::string::npos) {
                    texture.path = texture.path.substr(0, dotPos);
                }
#ifdef _DEBUG
                std::cout << texture.path << '\n';
#endif
                 //texture.id = 0;
                // texture.id = g_ResourceManager.GetTextureDDS(texture.path);

#ifdef _DEBUG
                std::cout << texture.id << '\n';
#endif
              // texture.type = "texture_diffuse"; // Assign type if known
                textures.push_back(texture);

                std::cout << textures.size() << '\n';
            }

#ifdef _DEBUG
            std::cout  << "tEX SIZE Inside Loadin \t" << textures.size() << '\n';
#endif


            // Create the Mesh object and add it to the vector
            meshes.emplace_back(vertices, indices, textures);
        }

        binFile.close();
        std::cout << "Loaded mesh from " << filePath << " successfully!" << std::endl;

        std::filesystem::path p(filePath);
        std::string fileName = p.stem().string();

        Model model;
        model.name = fileName;
        model.meshes = meshes;
        g_ResourceManager.SetModelMap(model.name, model);
    }
    catch (const std::exception& e) {
        std::cerr << "Error occurred: " << e.what() << std::endl;
        return false;
    }

    return true;
}


//bool LoadBinFile(std::string filePath)
//{
//    std::ifstream binFile(filePath, std::ios::binary);
//    if (!binFile.is_open()) {
//        std::cerr << "Failed to open binary file: " << filePath << std::endl;
//        return {};
//    }
//
//    // Step 1: Read the number of meshes
//    size_t meshCount;
//    binFile.read(reinterpret_cast<char*>(&meshCount), sizeof(size_t));
//
//    std::vector<Mesh> meshes;
//    meshes.reserve(meshCount);
//
//    // Step 2: For each mesh, read vertex and index data
//    for (size_t i = 0; i < meshCount; ++i) {
//        // Read vertex data
//        size_t vertexCount;
//        binFile.read(reinterpret_cast<char*>(&vertexCount), sizeof(size_t));
//        std::vector<Vertex> vertices(vertexCount);
//        binFile.read(reinterpret_cast<char*>(vertices.data()), vertexCount * sizeof(Vertex));
//
//        // Read index data
//        size_t indexCount;
//        binFile.read(reinterpret_cast<char*>(&indexCount), sizeof(size_t));
//        std::vector<unsigned int> indices(indexCount);
//        binFile.read(reinterpret_cast<char*>(indices.data()), indexCount * sizeof(unsigned int));
//
//
//        size_t texCount;
//        std::vector<Texture> texture;
//        binFile.read(reinterpret_cast<char*>(&texCount), sizeof(size_t));
//
//        std::cout << "textrure count: \t\t\t" << texCount << "\n\n\n\n\n";
//
//        // Create the Mesh object and add it to the vector
//        meshes.emplace_back(vertices, indices, texture);
//    }
//
//    binFile.close();
//
//    std::cout << "Loaded mesh from " << filePath << " successfully!" << std::endl;
//
//
//    std::cout << "********************************************\n";
//    std::cout << "Mesh contains: " << meshes.size() << '\t' << meshes.size() << '\n';
//
//    std::cout << "Attempting to load into Model Map\n";
//    std::cout << "********************************************\n";
//
//
//
//    std::filesystem::path p(filePath);
//
//    std::string fileName = p.stem().string();
//
//    Model model;
//
//    model.name = fileName;
//    model.meshes = meshes;
//    //    meshes[0].setupMesh();
//
//    g_ResourceManager.SetModelMap(model.name, model);
//
//
//    //g_ResourceManager.addModelNames(model.name);
//
//
//    return true;
//}



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


    
    ModelMap.insert(std::pair<std::string, Model>(name, model));

#ifdef _DEBUG

    std::cout << "Loaded name: " << name << " [Models Reference: " << ModelMap.size() - 1 << "]" << '\n';

#endif
	return true;

//    ModelNames.push_back(name);
}

Model* ResourceManager::getModel(const std::string& modelName) {
    auto it = ModelMap.find(modelName);
    if (it != ModelMap.end()) {
        return &(it->second);  // Return pointer to the model
    }
    return nullptr;  // Return nullptr if not found
}


Model* ResourceManager::GModel() const {
    return m_Model;

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

#ifdef _DEBUG
        std::cout << ModelNames[i] << '\n';
#endif
        LoadBinFile(FILEPATH_RESOURCE_OBJECTS + "\\" + ModelNames[i] + ".bin");
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





bool ResourceManager::LoadTexturesDDS() {

    for (int i = 0; i < textureDDSFileNames.size(); i++) {
        
        // std::cout << "names:" << textureDDSFileNames[i].c_str() << "\n";
    
        //add DDS processing here
        DDSData result = LoadDDS((FILEPATH_RESOURCE_TEXTURES + "\\" + textureDDSFileNames[i] + ".dds").c_str());
    
  //      result = LoadDDS((FILEPATH_RESOURCE_TEXTURES + "\\" + textureDDSFileNames[i] + ".dds").c_str())
    
        std::cout << result.ID <<std::endl;
    
    
        if (result.ID != -1) {
            textureDDS[textureDDSFileNames[i]] = result;
            std::cout << "Texture DDS File Added : " << textureDDSFileNames[i] << std::endl;
        }
        else
        {
            std::cout << "Failed to load Textures DDS\n";
        }
    
    }
    // For now, returning true to indicate successful loading
    return true;
}

bool ResourceManager::FreeTexturesDDS() {
    // Iterate over each texture in the map and delete it
    for (const auto& texturePair : textureDDS) {
        GLuint textureID = texturePair.second.ID;
        glDeleteTextures(1, &textureID); // Delete the OpenGL texture
    }

    // Clear only the map of loaded textures, keep textureDDSFileNames intact
    textureDDS.clear();
    return true;
}


bool ResourceManager::ReloadTexturesDDS() {
    // Free existing textures
    FreeTexturesDDS();

    // Clear the existing texture names list
    //textureDDSFileNames.clear();

    // Re-populate the list by scanning the directory
    for (const auto& entry : std::filesystem::directory_iterator(FILEPATH_RESOURCE_TEXTURES)) {
        if (entry.path().extension() == ".dds") {
            std::string textureName = entry.path().stem().string();
 //           textureDDSFileNames.push_back(textureName);
        }
    }

    // Load textures based on the updated list
    return LoadTexturesDDS();
}


bool ResourceManager::AddTextureDDS(std::string textureName) {
   // // Add texture name to the list
    textureDDSFileNames.push_back(textureName);
   
    // Attempt to load the DDS texture immediately
    DDSData result = LoadDDS((FILEPATH_RESOURCE_TEXTURES + "\\" + textureName + ".dds").c_str());
   
    if (result.ID != 0) { // If loading is successful, result will be a valid texture ID
        textureDDS[textureName] = result; // Store the loaded texture ID in the map
        std::cout << "Texture DDS File Added and Loaded: " << textureName << " with ID " << result.ID << std::endl;
    }
    else {
        std::cerr << "Failed to load Texture DDS: " << textureName << std::endl;
    }
   
    return result.ID != 0; // Return true if texture was loaded successfully, false otherwise

    return 0;
}


int ResourceManager::GetTextureDDS(std::string textureName) {
    // Check if the texture exists in the map
    auto it = textureDDS.find(textureName);
    if (it != textureDDS.end()) {
        // Return the texture ID if found
        return it->second.ID;
    }
    else {
        // Return -1 or some error value if texture not found
        // std::cerr << "Texture not found: " << textureName << std::endl;
        return -1;
    }
}


int ResourceManager::GetTextureDDSWidth(std::string textureName)
{
    auto it = textureDDS.find(textureName);
    if (it != textureDDS.end()) {
        // Return the DDS data if found
        return it->second.Width;  // This will return the DDSData associated with the texture
    }
    else {
        // Return a default or error DDSData if texture not found
        // You can either create a default DDSData object with error values or handle it as needed
        std::cerr << "Texture not found: " << textureName << std::endl;
        return -1; // Default-constructed DDSData (all members initialized to 0)
    }
}


int ResourceManager::GetTextureDDSHeight(std::string textureName)
{
    auto it = textureDDS.find(textureName);
    if (it != textureDDS.end()) {
        // Return the DDS data if found
        return it->second.Height;  // This will return the DDSData associated with the texture
    }
    else {
        // Return a default or error DDSData if texture not found
        // You can either create a default DDSData object with error values or handle it as needed
        std::cerr << "Texture not found: " << textureName << std::endl;
        return -1; // Default-constructed DDSData (all members initialized to 0)
    }
}

std::string ResourceManager::GetTextureDDSFileName(int textureID)
{
	for (auto& it : textureDDS)
	{
		if (it.second.ID == textureID)
		{
			return it.first;
		}
	}
	return "";
}






void ResourceManager::AddFont(std::string name) {
    fontResources[name] = fontSystem.readFromBin(name);
    fontnames.push_back(name);
}
