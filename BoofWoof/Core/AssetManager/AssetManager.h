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
#pragma once

#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include "../Graphics/Graphics.h"
#include "../Graphics/Shader.h"


#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))


#define FILEPATH_SHADERS "../Shaders"


#define FILEPATH_LOADING_MODELS "../Adv Computer Graphics/Model/PPlantModel/Section5.txt"
#define FILEPATH_LOADING_TEST_MODELS "../Adv Computer Graphics/Model/RenderThis.txt"


#define FILEPATH_PRE_ADDITION_MODEL "../Adv Computer Graphics/Model/PPlantModel/"
#define FILEPATH_PRE_ADDITION_MODEL2 "../Adv Computer Graphics/Model/TestingModel/"

#define FILEPATH_MODELS_A "../Adv Computer Graphics/Model/PPlantModel/ppsection5/part_a/"
#define FILEPATH_MODELS_B "../Adv Computer Graphics/Model/PPlantModel/ppsection5/part_b/"
#define FILEPATH_MODELS_C "../Adv Computer Graphics/Model/PPlantModel/ppsection5/part_c/"

#define FILEPATH_TESTINGMODELS "../Adv Computer Graphics/Model/TestingModel/"

#define FILEPATH_RABBITMODEL "../Adv Computer Graphics/Model/TestingModel/bunny_high_poly.obj"
#define FILEPATH_RABBITMODEL2 "../Adv Computer Graphics/Model/TestingModel/g5.obj"
//#define FILEPATH_RABBITMODEL2 "../Adv Computer Graphics/Model/TestingModel/g0.obj"

#define FILEPATH_OBJ1 "../Adv Computer Graphics/Model/PPlantModel/ppsection5/part_a/g5.obj"




enum Classification
{
	MODEL = 0,

	TESTING = 100

};




// Structure to hold material data
struct Material {
	std::string name;       // Material name
	glm::vec3 ambient;      // Ambient color
	glm::vec3 diffuse;      // Diffuse color
	glm::vec3 specular;     // Specular color
	float shininess;        // Shininess coefficient
	// Add more material properties as needed
};


struct ObjData {
	std::vector<glm::vec3> positions; // Vertex positions
	std::vector<glm::vec3> normals;   // Vertex normals
	std::vector<unsigned int> indices; // Indices for triangles
	std::vector<Material> materials;  // Materials in the OBJ file
};

extern ObjData Objectdata;

ObjData LoadOBJ(const std::vector<std::string>& filePath);

/**************************************************************************
* @brief AssetManager Class
*************************************************************************/
class AssetManager
{


public:

	AssetManager();																			// Default Constructor
	~AssetManager();																		// Default DeConstructor


	bool LoadAll();																			// Function to load all the assets that are required (NOT IMPLEMENTED)		
	bool Free();																			// Function to Free Assets

	using VectorPairString = std::vector <std::pair<std::string, std::string>>;				// Using a vector-->pair string for shaders


	bool LoadScene();
	bool LoadShaders();																		// Load Shaders
	void InitShdrpgms(VectorPairString const& vpss);										//Function to initialize shaders
	
	bool FreeShaders();																		//Function to free shaders

	//std::vector<std::string> LoadModelPaths(const std::vector<std::string>& filepath);

	bool FindModelFiles(int chooseMode = MODEL);
	void PrintModelFiles(const std::string& directory);



	std::vector<std::string> readFilePathsFromFile(const std::string& filename);
	std::vector<std::string> readFilePathsFromFileMaterial(const std::string& filename);

	//bool LoadModel(const std::string& path);
	//bool ProcessNode(aiNode* node, const aiScene* scene); 
	//
	//Graphics::Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);


	bool LoadMesh(const std::string& fileName);



	//static ObjData Objectdata;


	std::vector<OpenGLShader>shdrpgms;														//Container to store shader programs

	static std::vector<std::string> txtFile;
	static std::vector<std::string> mtlFile;
	static std::vector<glm::vec3> modelCenterCoord;
	static std::vector<glm::vec3> modelAABBHalfExtents;
	
	
	static std::vector<std::string> objFilesA;
	static std::vector<std::string> mtlFilesA;

	static std::vector<std::string> objFilesB;
	static std::vector<std::string> mtlFilesB;

	static std::vector<std::string> objFilesC;
	static std::vector<std::string> mtlFilesC;


	static std::vector<std::string> testingContainer;

	static std::vector <ObjData> Vector3_Objects;



private:

};




extern AssetManager assetManager;		// Allow usage of class outside of assetManager.cpp

#endif