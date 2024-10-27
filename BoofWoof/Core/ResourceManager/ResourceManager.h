/**************************************************************************
 * @file ResourceManager.h
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


#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H


#include <map>
#include <string>
#include <vector>
#include <filesystem>
#include "../Core/Graphics/Model.h"

class ResourceManager {

public:
	ResourceManager();
	~ResourceManager();

	bool LoadAll();

	const std::map<std::string, Model>& GetModelMap() const;

	bool SetModelMap(const std::string& name, const Model& model);

	Model* getModel(const std::string& modelName);

	// Setter for ModelMap (add a new model or update existing one)
	void setModel(const std::string& modelName, const Model& model);

	std::vector<std::string> getModelNames() const;

	void addModelNames(std::string);

	// Optional: Check if a model exists in the map
	bool hasModel(const std::string& modelName);


	bool LoadModelBinary();
	bool AddModelBinary(std::string);

	bool LoadTexturesDDS();
	bool AddTextureDDS(std::string);
	int GetTextureDDS(std::string);																		//Function to access textures





	
private:
	

	std::map <std::string, Model> ModelMap;

	std::vector<std::string> ModelNames;

	std::map<std::string, int> texturesDDS{};															//Container to store textures
	std::vector<std::string> textureDDSFileNames{};														//Container to store texture DDS file names
	std::vector<std::string> textureFile{};
};

extern ResourceManager g_ResourceManager;																// Allow usage of class outside of resourceManager.cpp

#endif // !RESOURCEMANAGER_H