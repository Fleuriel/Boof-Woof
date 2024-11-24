#pragma once
#include "ECS/pch.h"
#define NOMINMAX
#include <windows.h>
#include <thread>

extern bool monitoringThread;

LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam);

bool CopyAndOverwriteDLL(const std::wstring& sourcePath, const std::wstring& destPath);

bool IsDLLInUse(const std::wstring& dllPath);

void MonitorDirectory();

void ChangeDLL();

void SetCustomHook();

void DLL_Support_Init();

void DLL_Support_Update();

void DLL_Support_Unload();

//void StartFocusChecker();

void FocusChecker(GLFWwindow* window, int focused);