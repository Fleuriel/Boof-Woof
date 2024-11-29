#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
#define NOMINMAX             // Avoid macro conflict with min/max
#include <windows.h>

// Prevent GLFW from redefining APIENTRY
#ifdef APIENTRY
#undef APIENTRY
#endif
#include "ECS/pch.h"
#include <thread>

extern bool monitoringThread;

LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam);

bool CopyAndOverwriteDLL(const std::wstring& sourcePath, const std::wstring& destPath);

bool IsDLLInUse(const std::wstring& dllPath);

bool CompareFiles(const std::filesystem::path& main, const std::filesystem::path& copy);

void MonitorDirectory();

void ChangeDLL();

void SetCustomHook();

void DLL_Support_Init();

void DLL_Support_Update();

void DLL_Support_Unload();

//void StartFocusChecker();

// void FocusChecker(GLFWwindow* window, int focused);