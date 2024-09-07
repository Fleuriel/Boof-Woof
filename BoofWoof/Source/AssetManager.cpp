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

#include "AssetManager.h"
#include <filesystem>
#include "windows.h"
#include <iostream>
#include "Graphics.h"
#include "Shader.h"
#include <fstream>


extern AssetManager assetManager;



std::vector<std::string> AssetManager::txtFile;
std::vector<std::string> AssetManager::mtlFile;


std::vector<glm::vec3> AssetManager::modelCenterCoord;
std::vector<glm::vec3> AssetManager::modelAABBHalfExtents;

std::vector<std::string> AssetManager::objFilesA;
std::vector<std::string> AssetManager::mtlFilesA;

std::vector<std::string> AssetManager::objFilesB;
std::vector<std::string> AssetManager::mtlFilesB;

std::vector<std::string> AssetManager::objFilesC;
std::vector<std::string> AssetManager::mtlFilesC;

std::vector<std::string> AssetManager::testingContainer;


std::vector <ObjData> AssetManager::Vector3_Objects;


ObjData Objectdata;

namespace fs = std::filesystem;



/**************************************************************************
 * @brief Default Constructor
 *************************************************************************/
AssetManager::AssetManager()
{

}
/**************************************************************************
 * @brief Deconstructor
 *************************************************************************/
AssetManager::~AssetManager()
{

}

/**************************************************************************
 * @brief Load All Assets (Unimplemented)
 *************************************************************************/
bool AssetManager::LoadAll()
{

#ifdef _DEBUG
	std::cout << "Current working directory: " << fs::current_path() << std::endl;
	//PrintModelFiles(FILEPATH_MODELS_A);
	//PrintModelFiles(FILEPATH_MODELS_B);
	//PrintModelFiles(FILEPATH_MODELS_C);
#endif



	//if (!FindModelFiles(TESTING))
	//{
	//	std::cout << "Model not Found in PATH:\n";
	//	return false;
	//}


#ifdef _DEBUG

	std::cout << "ObjectA and mtlA size: " << objFilesA.size() << '\t' << mtlFilesA.size() << '\n';

	std::cout << "ObjectB and mtlB size: " << objFilesB.size() << '\t' << mtlFilesB.size() << '\n';

	std::cout << "ObjectC and mtlC size: " << objFilesC.size() << '\t' << mtlFilesC.size() << '\n';
#endif
	std::cout << "Found Model Paths\n";



	if (!LoadShaders())
	{
		std::cout << "Shaders not loaded \n";
		return false;
	}

	std::cout << "Load Shaders Completed.\n";

	if (!LoadScene())
	{

		std::cout << "Scene(s) not Found in PATH:\n";
		return false;
	}

	std::cout << "Load Scene Completed.\n";


	std::cout << "Loading Assets Completed.\n";
	return true;
}
/**************************************************************************
 * @brief Free Assets (Unimplemented)
 *************************************************************************/
bool AssetManager::Free()
{

	return true;
}


/**************************************************************************
 * @brief Load Shaders
 * @param bool true or false depending on failure or success
 *************************************************************************/
bool AssetManager::LoadShaders() {
	
	std::cout << "Load Shaders\n";

	VectorPairString SHADER
	{
		std::make_pair<std::string, std::string>
		("Shaders\\Shader.vert", "Shaders\\Shader.frag")
	};

	InitShdrpgms(SHADER);

#ifdef _DEBUG
	std::cout << "ShdrPGM Size : " << shdrpgms.size() << '\n';
#endif

	if (shdrpgms.size() == 0)
		return false;

	return true;
}
/**************************************************************************
 * @brief Initialize Shaders into the graphics pipeline
 * @param VPSS the vector paired <string,string>
 *************************************************************************/
void AssetManager::InitShdrpgms(VectorPairString const& vpss)
{
	for (auto const& x : vpss) {
		std::cout << "Files to be added: \t";
		std::cout << x.first << '\t';
		std::cout << x.second << '\t' << '\n';


		// Create Vector for pair of Enum and String
		std::vector<std::pair<GLenum, std::string>> shdr_files;
		// Emplace back into the shdr_files vector
		shdr_files.emplace_back(std::make_pair(GL_VERTEX_SHADER, x.first));
		shdr_files.emplace_back(std::make_pair(GL_FRAGMENT_SHADER, x.second));

		// Create Shader
		OpenGLShader shdr_pgm;
		// Validate the shader program (shdr_Files).
		shdr_pgm.CompileLinkValidate(shdr_files);
		// insert shader program into container
		shdrpgms.emplace_back(shdr_pgm);
	}
}
/**************************************************************************
 * @brief Free Shaders (Unimplemented)
 *************************************************************************/
bool AssetManager::FreeShaders()
{
	return false;
}



bool AssetManager::LoadScene()
{

	bool success = true;



	//if (objFilesA.empty() || objFilesB.empty() || objFilesC.empty())
	//	return success = false;
	//
	//
	txtFile = readFilePathsFromFile(FILEPATH_LOADING_MODELS);
	//
	//
	for (int i = 0; i < txtFile.size(); ++i)
	{
	//	std::cout << txtFile[i] << '\n';;
	}


	//std::cout << "AssetManager Size: " << assetManager.txtFile.size() << '\n';

#ifdef _DEBUG



#endif

	//std::cout << "here already\n";
	
	



//	LoadOBJ(txtFile);




	//std::cout << Objectdata.indices.size() << '\n';
	//std::cout << Objectdata.materials.size() << '\n';
	//std::cout << Objectdata.positions.size() << '\n';
	//std::cout << Objectdata.normals.size() << '\n';





	//for (int i = 0; i < Objectdata.positions.size(); ++i)
	//{
	//	std::cout << "Positions: " << Objectdata.positions[i].x << '\t';
	//	std::cout << Objectdata.positions[i].y << '\t';
	//	std::cout << Objectdata.positions[i].z << '\n';
	//
	//}





	return true;
}

std::vector<std::string> AssetManager::readFilePathsFromFileMaterial(const std::string& filename) {
	std::vector<std::string> filePaths;
	std::ifstream file(filename);
	std::string line;

	while (std::getline(file, line)) {
		// Remove any leading/trailing whitespace
		line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());

		// Check if the line is not empty and ends with ".obj"
		if (!line.empty() && line.size() > 4) {
			std::string extension = line.substr(line.size() - 4);
			if (extension == ".obj") {
				// Remove the ".obj" extension
				std::string baseName = line.substr(0, line.size() - 4);
				// Add ".mtl" extension
				std::string mtlFile = baseName + ".mtl";
				filePaths.push_back(FILEPATH_PRE_ADDITION_MODEL + mtlFile);
			}
		}
	}


	return filePaths;
}

std::vector<std::string> AssetManager::readFilePathsFromFile(const std::string& filename) {
	std::vector<std::string> filePaths;
	std::ifstream file(filename);
	std::string line;


	//while (std::getline(file, line)) {
	//	// Remove any leading/trailing whitespace
	//	line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
	//
	//	if (!line.empty()) {
	//		filePaths.push_back(FILEPATH_PRE_ADDITION_MODEL + line);
	//	}
	//
	//
	//
	//
	//}

	return filePaths;
}


bool AssetManager::FindModelFiles(int chooseMode) {


	bool secured = true;
	std::string directory;
	if (chooseMode == MODEL)
	{
		for (int i = 0; i < 3; ++i)
		{
			if (i == 0)
				directory = FILEPATH_MODELS_A;
			if (i == 1)
				directory = FILEPATH_MODELS_B;
			if (i == 2)
				directory = FILEPATH_MODELS_C;
			try {
				for (const auto& entry : fs::directory_iterator(directory)) {
					if (entry.is_regular_file()) {
						std::string path = entry.path().string();
						std::string extension = entry.path().extension().string();
						if (extension == ".obj") {
							if (i == 0)
								objFilesA.push_back(path);
							else if (i == 1)
								objFilesB.push_back(path);
							else if (i == 2)
								objFilesC.push_back(path);
						}
						else if (extension == ".mtl") {
							if (i == 0)
								mtlFilesA.push_back(path);
							else if (i == 1)
								mtlFilesB.push_back(path);
							else if (i == 2)
								mtlFilesC.push_back(path);
						}
					}
				}
			}
			catch (const fs::filesystem_error& e) {
				std::cerr << "Filesystem error: " << e.what() << std::endl;
				return false;
			}
			catch (const std::exception& e) {
				std::cerr << "General error: " << e.what() << std::endl;
				return false;
			}
		}



		if (objFilesA.empty() || mtlFilesA.empty())
		{
			std::cout << "Obj Files A Or MTL Files A is empty. \n";
			secured = false;
		}

		if (objFilesB.empty() || mtlFilesB.empty())
		{
			std::cout << "Obj Files B Or MTL Files B is empty. \n";
			secured = false;
		}

		if (objFilesC.empty() || mtlFilesC.empty())
		{
			std::cout << "Obj Files C Or MTL Files C is empty. \n";
			secured = false;
		}



	}
	else if (chooseMode == TESTING)
	{
		directory = FILEPATH_TESTINGMODELS;
		try {	
			for (const auto& entry : fs::directory_iterator(directory)) {
				if (entry.is_regular_file()) {
					std::string path = entry.path().string();
					std::cout << path << '\n';
					std::string extension = entry.path().extension().string();
					if (extension == ".obj") {
						testingContainer.push_back(path);
					}
				}
			}
		}
		catch (const fs::filesystem_error& e) {
			std::cerr << "Filesystem error: " << e.what() << std::endl;
			return false;
		}
		catch (const std::exception& e) {
			std::cerr << "General error: " << e.what() << std::endl;
			return false;
		}

	}


	return secured;
}

void AssetManager::PrintModelFiles(const std::string& directory) {

	try {
		for (const auto& entry : fs::directory_iterator(directory)) {
			if (entry.is_regular_file()) {
				std::string path = entry.path().string();
				std::cout << "Found file: " << path << std::endl;
			}
		}
	}
	catch (const fs::filesystem_error& e) {
		std::cerr << "Filesystem error: " << e.what() << std::endl;
	}
	catch (const std::exception& e) {
		std::cerr << "General error: " << e.what() << std::endl;
	}
}







