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
#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include "Graphics/GraphicsSystem.h"
#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Graphics/Object.h"
#include "Sprite.h"

 // Forward declarations




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
	bool ReloadScenes();																	//Function to reload scenes

	bool LoadObjects();																		//Function to load objects

	bool LoadFonts();																		//Function to load fonts


	using VectorPairString = std::vector <std::pair<std::string, std::string>>;				// Using a vector-->pair string for shaders

	bool LoadShaders();																		// Load Shaders
	void InitShdrpgms(VectorPairString const& vpss);										//Function to initialize shaders
	bool FreeShaders();																		//Function to free shaders


	//Getters
	Sprite GetSprite(std::string);															//Function to access sprite
	int GetSpriteTexture(std::string);														//Function to access sprite texture
	OpenGLShader& GetShader(std::string);													//Function to access shaders





//	std::map <std::string, Model> ModelMap;


	bool Currentlyloading{ false };

private:

	//Containers
	std::vector<std::string> TextureDescriptionFiles;										//Container to store texture description files
	std::map<std::string, Sprite> sprites{};												//Container to store sprites
	std::vector<OpenGLShader>shdrpgms;														//Container to store shader programs
	std::vector<std::string>shdrpgmOrder{};													//Container to store the order of shader programs


};




extern AssetManager g_AssetManager;		// Allow usage of class outside of assetManager.cpp

#endif