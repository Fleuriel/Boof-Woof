/**************************************************************************
 * @file AssetManager.h
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

#include "Shader.h"
#include <vector>
#include <string>

/**************************************************************************
* @brief AssetManager Class
*************************************************************************/
class AssetManager
{


public:

	AssetManager();																			// Default Constructor
	~AssetManager();																		// Default DeConstructor


	void LoadAll();																			// Function to load all the assets that are required (NOT IMPLEMENTED)																
	void Free();																			// Function to Free Assets

	using VectorPairString = std::vector <std::pair<std::string, std::string>>;				// Using a vector-->pair string for shaders

	bool LoadShaders();																		// Load Shaders
	void InitShdrpgms(VectorPairString const& vpss);										//Function to initialize shaders
	
	bool FreeShaders();																		//Function to free shaders


	std::vector<OpenGLShader>shdrpgms;														//Container to store shader programs

private:
};

extern AssetManager assetManager;		// Allow usage of class outside of assetManager.cpp

#endif