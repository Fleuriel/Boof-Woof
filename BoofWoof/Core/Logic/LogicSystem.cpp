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

std::string dllpath = "ScriptWoof\\x64\\Debug";
std::string dllname = "ScriptWoof.dll";
std::string dllname2 = "CopyScriptWoof.dll";
HINSTANCE hGetProcIDDLL = nullptr;

std::filesystem::path FindDllPath(const std::string& targetPath, const std::string& targetName) {
    // Current directory
    std::filesystem::path currentPath = std::filesystem::current_path();

    // Check if the DLL is already in the current directory
    std::filesystem::path workingDirPath = currentPath / targetName;
    if (std::filesystem::exists(workingDirPath)) {
        std::cout << targetName << " found in the working directory: " << workingDirPath << std::endl;
        return workingDirPath; // Use the DLL from the current directory
    }

    // If not found, traverse parent directories
    for (int counter = 0; counter <= 5; ++counter) {
        std::filesystem::path tempPath = currentPath / targetPath / targetName;
        std::cout << "Checking: " << tempPath << std::endl;

        if (std::filesystem::exists(tempPath)) {
            std::cout << targetName << " found in parent directory: " << tempPath << std::endl;
            return tempPath;
        }

        // Move up to the parent directory
        currentPath = currentPath.parent_path();
    }

    // DLL not found after 5 levels of traversal
    std::cerr << targetName << " not found within 5 parent directories or in the working directory." << std::endl;
    return "";
}

void LogicSystem::Init()
{
    std::cout << std::endl << "Logic System Initialized" << std::endl;

    // Define paths for DLL_MAIN_DIRECTORY and DLL_COPY_DIRECTORY
    DLL_MAIN_DIRECTORY = FindDllPath(dllpath, dllname);

    std::filesystem::path executableDirDll = std::filesystem::current_path() / L"CopyScriptWoof.dll";
    std::filesystem::path fallbackDll = std::filesystem::current_path().parent_path() / L"CopyScriptWoof.dll";

    if (std::filesystem::exists(executableDirDll)) {
        DLL_COPY_DIRECTORY = executableDirDll;
        std::wcout << L"Using DLL from Executable Directory: " << DLL_COPY_DIRECTORY << std::endl;
    }
    else {
        DLL_COPY_DIRECTORY = fallbackDll;
        std::wcout << L"Using DLL from Fallback Directory: " << DLL_COPY_DIRECTORY << std::endl;
    }

    // Check if the DLL exists
    if (!std::filesystem::exists(DLL_MAIN_DIRECTORY)) {
        std::cerr << "DLL not found" << std::endl;
        throw std::runtime_error("DLL not found");
        return;
    }

    // Copy the DLL if necessary
    bool bSuccess = CopyFileW(DLL_MAIN_DIRECTORY.c_str(), DLL_COPY_DIRECTORY.c_str(), FALSE);
    if (bSuccess) {
        std::cout << "Updated DLL" << std::endl;
    }
    else {
        DWORD dwError = GetLastError();
        std::cerr << "Failed to copy DLL. Error code: " << dwError << std::endl;
    }

    // Load the DLL
    hGetProcIDDLL = LoadLibraryW(DLL_COPY_DIRECTORY.c_str());
    if (hGetProcIDDLL == NULL) {
        std::cerr << "Could not load the dynamic library" << std::endl;
        throw std::runtime_error("Could not load the dynamic library");
        return;
    }

    // Retrieve functions from the DLL
    auto pGetScripts = (GetScripts_cpp_t)GetProcAddress(hGetProcIDDLL, "GetScripts");
    if (!pGetScripts) {
        std::cout << "Its not working" << std::endl;
    }
    else {
        // Create Script_to_Engine object dynamically
        mScriptEngine = new Script_to_Engine();

        // Get the scripts from the script engine
        AddBehaviours(GetScripts(mScriptEngine));
    }

    // Initialize entities and behaviours
    for (auto const& entity : mEntities) 
    {
        // Get the logic component of the entity
        if (g_Coordinator.HaveComponent<BehaviourComponent>(entity))
        {
            BehaviourComponent behaviourComponent = g_Coordinator.GetComponent<BehaviourComponent>(entity);
            std::string behaviourName = behaviourComponent.GetBehaviourName();

            // Check if behaviour exists
            if (mBehaviours.find(behaviourName) == mBehaviours.end()) {
                std::cout << "Behaviour not found" << std::endl;
                continue;
            }
            else if (behaviourName == "Player" && g_Player == NULL) {
                g_Player = entity;
                std::cout << "Player entity found" << std::endl;
            }
            else if (behaviourName == "Player" && g_Player != NULL) {
                std::cerr << "Multiple Player entities found!" << std::endl;
                continue;
            }
            else {
                std::cout << "Behaviour Name: " << behaviourName << " exists" << std::endl;
            }

            // Print all the behaviours
            std::cout << "All Behaviours: " << std::endl;
            for (auto const& behaviour : mBehaviours) {
                std::cout << behaviour.first << std::endl;
            }
            std::cout << std::endl;

            // Initialize the behaviour
            mBehaviours[behaviourComponent.GetBehaviourName()]->Init(entity);
        }
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

void LogicSystem::ReInit()
{
	g_Player = NULL;

    // Initialize entities and behaviours
    for (auto const& entity : mEntities)
    {
        // Get the logic component of the entity
        if (g_Coordinator.HaveComponent<BehaviourComponent>(entity))
        {
            BehaviourComponent behaviourComponent = g_Coordinator.GetComponent<BehaviourComponent>(entity);
            std::string behaviourName = behaviourComponent.GetBehaviourName();

            // Check if behaviour exists
            if (mBehaviours.find(behaviourName) == mBehaviours.end()) {
                std::cout << "Behaviour not found" << std::endl;
                continue;
            }
            else if (behaviourName == "Player" && g_Player == NULL) {
                g_Player = entity;
                std::cout << "Player entity found" << std::endl;
                std::cout << "Behaviour Name: " << behaviourName << " exists" << std::endl;
            }
            else if (behaviourName == "Player" && g_Player != NULL) {
                std::cerr << "Multiple Player entities found!" << std::endl;
                continue;
            }
            else {
                std::cout << "Behaviour Name: " << behaviourName << " exists" << std::endl;
            }

            // Initialize the behaviour
            mBehaviours[behaviourComponent.GetBehaviourName()]->Init(entity);

            std::cout << std::endl;
        }
    }
}

bool LogicSystem::ApprovingScriptChange(Entity entity) {
    if (g_Coordinator.HaveComponent<BehaviourComponent>(entity))
    {
        BehaviourComponent behaviourComponent = g_Coordinator.GetComponent<BehaviourComponent>(entity);
        std::string behaviourName = behaviourComponent.GetBehaviourName();

        // Check if behaviour exists
        if (mBehaviours.find(behaviourName) == mBehaviours.end()) {
            std::cout << "Behaviour not found" << std::endl;
        }
        else if (behaviourName == "Player" && g_Player == NULL) {
            g_Player = entity;
            std::cout << "Player entity found" << std::endl;
        }
        else if (behaviourName == "Player" && g_Player != NULL) {
            std::cerr << "Multiple Player entities found!" << std::endl;
        }
        else {
            std::cout << "Behaviour Name: " << behaviourName << " exists" << std::endl;
        }
    }

    return true;
}

void LogicSystem::InitScript(Entity entity) {
         BehaviourComponent behaviourComponent = g_Coordinator.GetComponent<BehaviourComponent>(entity);
        // Initialize the behaviour
        mBehaviours[behaviourComponent.GetBehaviourName()]->Init(entity);
}