#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H




#include <map>
#include <string>

class ResourceManager {

public:
	ResourceManager();
	~ResourceManager();

	bool LoadAll();
	bool LoadTexturesDDS();
	int GetTextureDDS(std::string);																		//Function to access textures

private:
	std::map<std::string, int> texturesDDS{};															//Container to store textures

};

extern ResourceManager g_ResourceManager;																// Allow usage of class outside of resourceManager.cpp

#endif // !RESOURCEMANAGER_H