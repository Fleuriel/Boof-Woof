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

//#include "../Core/Graphics/Model.h"
#include "../Core/Graphics/FontSystem.h"
//#include "../Core/Animation/Animator.h"
//#include "../Core/Animation/Animation.h"


struct DDSData {


	GLint ID;
	GLuint Width;
	GLuint Height;

	DDSData() :ID(0), Width(0), Height(0) {}

};

class Animator;
class AnimationT;
class Model;

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
	std::string GetTextureDDSFileName(int);



	std::map <std::string, Model> ModelMap;
	/// for loading font resources
	// read the .bin file and create the FontResources
	// add the FontResources to the fontResources map
	// similar to FontSystem::init() function
	void AddFont(std::string name);
	FontResources GetFont(std::string name) { return fontResources[name]; }
	std::vector<std::string> GetFontNames() const { return fontnames; }
	void ClearFont() { fontResources.clear(); fontnames.clear(); }
	

	void SetModel(Model* model) { m_Model = model; }

	Model* GModel() const;

	Model* m_Model{};


//	std::unordered_map<std::string, AnimationT*> AnimationMap;
//	std::unordered_map<std::string, Animator*> AnimatorMap; // Add this line


	std::vector<AnimationT*> animationVec;
	std::vector<Animator*> animatorVec;



private:
	


	std::vector<std::string> ModelNames;

	std::map <std::string, DDSData> textureDDS{};														//Container to store textures
	std::vector<std::string> textureDDSFileNames{};														//Container to store texture DDS file names

	std::vector<DDSData> textureDDS_WidthHeight;

	std::map<std::string, FontResources> fontResources{};												//Container to store font resources
	std::vector<std::string> fontnames{};

};

extern ResourceManager g_ResourceManager;																// Allow usage of class outside of resourceManager.cpp

#endif // !RESOURCEMANAGER_H