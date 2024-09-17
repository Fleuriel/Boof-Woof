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

extern AssetManager assetManager;

#define FILEPATH_SHADERS "../Shaders"

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
void AssetManager::LoadAll()
{

}
/**************************************************************************
 * @brief Free Assets (Unimplemented)
 *************************************************************************/
void AssetManager::Free()
{
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
