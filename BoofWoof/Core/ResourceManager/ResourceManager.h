#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H




#include <map>
#include <string>
#include <vector>

class ResourceManager {

public:
	ResourceManager();
	~ResourceManager();

	bool LoadAll();
	bool LoadTexturesDDS();
	bool AddTextureDDS(std::string);
	int GetTextureDDS(std::string);																		//Function to access textures




private:
	std::map<std::string, int> texturesDDS{};															//Container to store textures
	std::vector<std::string> textureDDSFileNames{};														//Container to store texture DDS file names
	std::vector<std::string> textureFile{};
};

extern ResourceManager g_ResourceManager;																// Allow usage of class outside of resourceManager.cpp

#endif // !RESOURCEMANAGER_H