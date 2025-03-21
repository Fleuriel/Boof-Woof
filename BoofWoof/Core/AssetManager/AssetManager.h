/**************************************************************************
 * @file AssetManager.h
 * @author 	Aaron Chan Jun Xiang
 * @param DP email: aaronjunxiang.chan@digipen.edu [2200880]
 * @param Course: CS 3401
 * @param Course: Game Project 3
 * @date  10/06/2024 (06 OCTOBER 2024)
 * @brief
 *
 * This file loads assets to be used
 *
 *************************************************************************/

#pragma once

#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H


#include "../assimp2016/assimp/Importer.hpp"
#include "../assimp2016/assimp/scene.h"
#include "../assimp2016/assimp/postprocess.h"
#include "Graphics/GraphicsSystem.h"
#include "Graphics/Shader.h"
#include "Graphics/Object.h"
#include "Sprite.h"
#include "Descriptor.h"

 // Forward declarations




class MaterialComponent;

/**************************************************************************
* @brief AssetManager Class
*************************************************************************/
class AssetManager
{


public:

	// General functions
	AssetManager();																			//Constructor for AssetManager class
	~AssetManager();																		//Destructor for AssetManager class

	void LoadAll();																			//Function to load all assets
	void FreeAll();																			//Function to free all assets
	//void ReloadAll();																		//Function to reload all assets

	// Texture functions
	bool LoadTextures();																	//Function to load textures
	bool FreeTextures();																	//Function to free textures
	bool ReloadTextures();																	//Function to reload textures


	// Scene functions
	bool LoadScenes();																		//Function to load scenes
	bool FreeScenes();																		//Function to free scenes
	bool ReloadScenes();																	//Function to reload scenes

	bool LoadObjects();																		//Function to load objects
	bool FreeObjects();																		//Function to free objects
	bool ReloadObjects();																	//Function to reload objects

	bool LoadFonts();																		//Function to load fonts
	bool FreeFonts();																		//Function to free fonts
	bool ReloadFonts();																		//Function to reload fonts

	using VectorPairString = std::vector <std::pair<std::string, std::string>>;				// Using a vector-->pair string for shaders

	bool LoadShaders();																		//Function to load shaders
	void InitShdrpgms(VectorPairString const& vpss);										//Function to initialize shaders
	bool FreeShaders();																		//Function to free shaders
	bool ReloadShaders();																	//Function to reload shaders

	bool LoadAnimations();																	//Function to load animations
	bool FreeAnimations();																	//Function to free animations
	bool ReloadAnimations();																//Function to reload animations

	bool LoadMaterials();
	bool ReloadMaterials();
	bool FreeMaterials();


	

	//Getters
	OpenGLShader& GetShader(std::string);													//Function to access shaders


	bool CheckFiles(const std::wstring& path); // Function to check for added and deleted texture files
	void MonitorFiles(const std::wstring& path);
	void DeleteAllFilesInDirectory(const std::string& directoryPath);

	void DiscardToTrashBin(const std::string& filepath, const std::string& folderName, bool message = true);

	bool Currentlyloading{ false };

	MaterialComponent LoadMaterialFromFile(const std::string& filePath);

	std::vector<std::string> AnimationFiles;												//Container to store animation file names
//	std::vector<std::string> MaterialFiles;													//Container to store animation file names
	std::vector<std::string> MaterialFiles;

	std::vector<std::string>shdrpgmOrder{};													//Container to store the order of shader programs


private:

	//Monitoring purposes
	std::set<std::wstring> TextureFiles;													//Container to store texture file names
	std::set<std::wstring> SpriteFiles;														//Container to store sprite file names
	std::set<std::wstring> SceneFiles;														//Container to store scene file names
	std::set<std::wstring> ObjectFiles;														//Container to store object file names
	std::set<std::wstring> ShaderFiles;														//Container to store shader file names
	std::set<std::wstring> FontFiles;														//Container to store font file names
	//std::set<std::wstring> MaterialFiles;




	//Containers
	std::unordered_map<std::string, MaterialComponent> materials;
	std::vector<std::string> TextureDescriptionFiles;										//Container to store texture description files
	std::map<std::string, Sprite> sprites{};												//Container to store sprites
	std::vector<OpenGLShader>shdrpgms;														//Container to store shader programs

	TextureDescriptor textureInfo;
	MaterialDescriptor materialInfo;
};




extern AssetManager g_AssetManager;		// Allow usage of class outside of assetManager.cpp

#endif