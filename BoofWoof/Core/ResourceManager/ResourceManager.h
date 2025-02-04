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

#include "../Core/Graphics/Model.h"

struct DDSData {


	GLint ID;
	GLuint Width;
	GLuint Height;

	DDSData() :ID(0), Width(0), Height(0) {}

};

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
	bool FreeTexturesDDS();
	bool ReloadTexturesDDS();

	bool AddTextureDDS(std::string);
	int GetTextureDDS(std::string);																		//Function to access textures
	int GetTextureDDSWidth(std::string);
	int GetTextureDDSHeight(std::string);


	bool LoadFontsDDS();
	bool FreeFontsDDS();
	bool ReloadFontsDDS();

	bool AddFontDDS(std::string);
	int GetFontDDS(std::string);


	
private:
	

	std::map <std::string, Model> ModelMap;

	std::vector<std::string> ModelNames;

	std::map <std::string, DDSData> textureDDS{};														//Container to store textures
	std::vector<std::string> textureDDSFileNames{};														//Container to store texture DDS file names

	std::map<std::string, int> fontDDS{};																//Container to store textures
	std::vector<std::string> fontDDSFileNames{};														//Container to store texture DDS file names


	std::vector<DDSData> textureDDS_WidthHeight;

	std::map<std::string, std::map<std::string, Model>> AnimationContainer;
	// AnimationContainer["Rex"]["walk"];

};

extern ResourceManager g_ResourceManager;																// Allow usage of class outside of resourceManager.cpp

#endif // !RESOURCEMANAGER_H