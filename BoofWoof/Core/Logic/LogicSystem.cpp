#include "pch.h"

#pragma warning(push)
#pragma warning(disable: 4005)

#include "LogicSystem.h"
#include "../ECS/Coordinator.hpp"
#include "../Logic/BehaviourInterface.h"
#include "../Utilities/Components/BehaviourComponent.hpp"
#include "../../../ScriptWoof/Compile.cpp"	
#include "Script_to_Engine.h"


#pragma warning(pop)

Entity g_Player = NULL;
//DLL_MAIN_DIRECTORY = L"..\\ScriptWoof\\x64\\Debug\\ScriptWoof.dll";
//DLL_COPY_DIRECTORY = L"..\\ScriptWoof.dll";
//const std::wstring DLL_PATH = L"..\\ScriptWoof.dll";
std::string dllpath = "ScriptWoof\\x64\\Debug";
std::string dllname = "ScriptWoof.dll";
std::string dllname2 = "CopyScriptWoof.dll";
HINSTANCE hGetProcIDDLL = nullptr;

std::filesystem::path FindDllPath(std::string targetPath, std::string targetName) {
	std::filesystem::path currentPath = std::filesystem::current_path();
	//std::cout << "Current Path: " << currentPath << '\n';

	// Specify the name of the file or directory you're looking for
	bool found = false;
	int counter = 0;

	// Iterate through the parent directory to find the file or directory
	while (!found) {
		//Go to Parent Directory
		std::filesystem::path parentPath = currentPath.parent_path();
		//std::cout << "Parent Path: " << parentPath << '\n';

		// Add the target name to the parent path
		std::filesystem::path tempPath = parentPath / targetPath;
		//std::cout << "Target Path: " << tempPath << '\n';

		// Check if the target exists
		if (std::filesystem::exists(tempPath)) {
			//std::cout << targetName << " found in the parent directory.\n";
			//Check if dll exists
			if (std::filesystem::exists(tempPath / dllname)) {
				//std::cout << dllname << " found in the parent directory.\n";
				currentPath = tempPath / dllname;
				found = true;
				break;
			}
		}


		currentPath = parentPath;
		if (counter > 5) {
			break;
		}
		counter++;
	}

	if (!found) {
		std::cout << targetName << " not found in the parent directory.\n";
		return "";
	}
	else {
		return currentPath;
	}
}

void LogicSystem::Init()
{
	std::cout << std::endl << "Logic System Initialized" << std::endl;
#ifdef DEBUG
	
#else
	std::string copydirectory = "CopyScriptWoof.dll";
	DLL_COPY_DIRECTORY = std::filesystem::current_path() / copydirectory;
	std::wcout << "DLL Copy Directory: " << DLL_COPY_DIRECTORY.c_str() << std::endl;
#endif

#ifdef DEBUG
	// Find the path of the DLL
	DLL_MAIN_DIRECTORY = FindDllPath(dllpath, dllname);
	//DLL_COPY_DIRECTORY = std::filesystem::current_path() / dllname;
	DLL_COPY_DIRECTORY = L"..\\CopyScriptWoof.dll";
	std::wcout << "DLL Main Directory: " << DLL_MAIN_DIRECTORY.c_str() << std::endl;
	std::wcout << "DLL Copy Directory: " << DLL_COPY_DIRECTORY.c_str() << std::endl;

	// Check if the DLL exists
	if (!std::filesystem::exists(DLL_MAIN_DIRECTORY)) {
		std::cerr << "DLL not found" << std::endl;
		throw std::runtime_error("DLL not found");
		return;
	}
	else
	{
		bool bSuccess = CopyFileW(DLL_MAIN_DIRECTORY.c_str(), DLL_COPY_DIRECTORY.c_str(), FALSE);
		if (bSuccess)
			std::cout << "Updated DLL" << std::endl;
		else {
			DWORD dwError = GetLastError();
			std::cerr << "Failed to copy DLL. Error code: " << dwError << std::endl;
		}
	}
#endif // DEBUG


	//HINSTANCE hGetProcIDDLL = LoadLibrary(L"..\\ScriptWoof\\x64\\Debug\\ScriptWoof.dll");
	hGetProcIDDLL = LoadLibraryW(DLL_COPY_DIRECTORY.c_str());

	if (hGetProcIDDLL == NULL)
	{
		std::cerr << "Could not load the dynamic library" << std::endl;
		throw std::runtime_error("Could not load the dynamic library");
		return;
	}

	if (hGetProcIDDLL)
	{
		auto pGetScripts = (GetScripts_cpp_t)GetProcAddress(hGetProcIDDLL, "GetScripts");
		if (!pGetScripts)
		{
			std::cout << "Its not working" << std::endl;
		}
		else
		{
			// Create Script_to_Engine object dynamically
			mScriptEngine = new Script_to_Engine();

			// Get the scripts from the script engine
			AddBehaviours(GetScripts(mScriptEngine));
		}
	}

	for (auto const& entity : mEntities)
	{
		// Get the logic component of the entity
		BehaviourComponent behaviourComponent = g_Coordinator.GetComponent<BehaviourComponent>(entity);
		std::string behaviourName = behaviourComponent.GetBehaviourName();

		// Check if behaviour exists
		if (mBehaviours.find(behaviourName) == mBehaviours.end())
		{
			std::cout << "Behaviour not found" << std::endl;
			continue;
		}
		else if (behaviourName == "Player" && g_Player == NULL)
		{
			g_Player = entity;
			std::cout << "Player entity found" << std::endl;
		}
		else if (behaviourName == "Player" && g_Player != NULL)
		{
			std::cerr << "Multiple Player entities found!" << std::endl;
			continue;
		}
		else {
			std::cout << "Behaviour Name: " << behaviourName << "exist" << std::endl;
		}
		//print all the behaviours
		std::cout << "All Behaviours: " << std::endl;
		for (auto const& behaviour : mBehaviours)
		{
			std::cout << behaviour.first << std::endl;
		}
		std::cout << std::endl;

		//Init the behaviour
		mBehaviours[behaviourComponent.GetBehaviourName()]->Init(entity);

	}
}

void LogicSystem::Update()
{
	//std::cout << "Logic System Updated" << std::endl;
	for (auto const& entity : mEntities)
	{
		if (g_Coordinator.HaveComponent<BehaviourComponent>(entity)) {
			// Get the logic component of the entity
			BehaviourComponent behaviourComponent = g_Coordinator.GetComponent<BehaviourComponent>(entity);
			if (mBehaviours.find(behaviourComponent.GetBehaviourName()) == mBehaviours.end())
			{
				//std::cout << behaviourComponent.GetBehaviourName() << std::endl;
				continue;
			}
			// Find which behaviour the entity has and run
			mBehaviours[behaviourComponent.GetBehaviourName()]->Update(entity);
		}
	}
}

void LogicSystem::Shutdown()
{
	// Unload the dynamic library
	FreeLibrary(hGetProcIDDLL);

	// Clean up Script_to_Engine object to prevent memory leak
	delete mScriptEngine;  // Ensure that the dynamically allocated object is deleted

	// Clear the map to remove all entries
	mBehaviours.clear();
}

void LogicSystem::AddBehaviours(void* scriptBehavioursPtr)
{
	// Take in the void* from GetScripts and cast back to the correct type
	auto* B_Vec = static_cast<std::vector<std::unique_ptr<Behaviour_i>>*>(scriptBehavioursPtr);

	if (B_Vec == nullptr || B_Vec->empty())
	{
		std::cerr << "B_Vec is null or empty!" << std::endl;
		return;
	}

	// Move behaviours to mBehaviours
	for (auto& behaviourPtr : *B_Vec)
	{
		if (behaviourPtr)
		{
			// ensuring unique pointers are properly moved so no mem leaks
			std::cout << "Behaviour Added : " << behaviourPtr->getBehaviourName() << std::endl;
			mBehaviours[behaviourPtr->getBehaviourName()] = std::move(behaviourPtr);
		}
	}

	// Clean up B_Vec to free the allocated memory
	delete B_Vec;
}