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
#include "Graphics/GraphicsSystem.h"
#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Graphics/Object.h"

 // Forward declarations




/**************************************************************************
* @brief AssetManager Class
*************************************************************************/
class AssetManager
{


public:

	AssetManager();																			// Default Constructor
	~AssetManager();																		// Default DeConstructor

	using VectorPairString = std::vector <std::pair<std::string, std::string>>;				// Using a vector-->pair string for shaders

	bool LoadShaders();																		// Load Shaders
	void InitShdrpgms(VectorPairString const& vpss);										//Function to initialize shaders
	bool FreeShaders();																		//Function to free shaders


	//static ObjData Objectdata;


	std::vector<OpenGLShader>shdrpgms;														//Container to store shader programs

	
	std::vector <Model>	Models;
	std::vector <Object> Objects;

	std::vector <Model2D> Model2D;
	std::vector <Object2D> Object2D;



private:

};




extern AssetManager g_AssetManager;		// Allow usage of class outside of assetManager.cpp

#endif