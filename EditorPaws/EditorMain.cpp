#define _CRTDBG_MAP_ALLOC
#include "ECS/pch.h"
#include <stdlib.h>
#include <crtdbg.h>
#define NOMINMAX
#include <windows.h>
#include <thread>
#include "ImGuiEditor.h"
#include "../BoofWoof/Core/EngineCore.h"

EngineCore* g_Core = nullptr;

// Global Variables
HHOOK hHook = NULL;
UINT WM_FILE_CHANGED;

// Directory to watch
const std::wstring WATCHED_DIRECTORY = L"..\\ScriptWoof\\x64\\Debug";

//DLL Directory
const std::wstring DLL_MAIN_DIRECTORY = L"..\\ScriptWoof\\x64\\Debug\\ScriptWoof.dll";
const std::wstring DLL_COPY_DIRECTORY = L"..\\ScriptWoof.dll";



LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam);
bool CopyAndOverwriteDLL(const std::wstring& sourcePath, const std::wstring& destPath);
void MonitorDirectory();
void SetCustomHook();


int main()
{

#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
#endif

	//for use with debugging, change the value to the location of the mem leak per the crt debug info from the console
	// refer to stack frame and see where it all went wrong
	//_crtBreakAlloc = 20649858;

    
    // Register a custom message
    WM_FILE_CHANGED = RegisterWindowMessageW(L"WM_FILE_CHANGED");

    // Set the hook
    SetCustomHook();

    // Start the directory monitoring in a separate thread
    std::thread monitorThread(MonitorDirectory);

    MSG msg;
    
	g_Core = new EngineCore();
	g_Core->OnInit();
	g_ImGuiEditor.ImGuiInit(g_Window);

	while (!glfwWindowShouldClose(g_Window->GetGLFWWindow()))
	{
        
		if (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		g_Core->OnUpdate();
		g_ImGuiEditor.ImGuiUpdate();
		g_ImGuiEditor.ImGuiRender();
	}

	g_ImGuiEditor.ImGuiEnd();
	g_Core->OnShutdown();

	delete g_Core;

    
	// Clean up
	monitorThread.detach();
	UnhookWindowsHookEx(hHook);
    
	return 0;
}


// Custom hook procedure
LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        MSG* msg = (MSG*)lParam;

        if (msg->message == WM_FILE_CHANGED) {
            MessageBoxW(NULL, L"File change detected!", L"Notification", MB_OK);
			std::cout << "File change detected!" << std::endl;
        }
    }
    return CallNextHookEx(hHook, nCode, wParam, lParam);
}

bool CopyAndOverwriteDLL(const std::wstring& sourcePath, const std::wstring& destPath) {
    // Attempt to copy the DLL file and overwrite the existing one
    if (CopyFileW(sourcePath.c_str(), destPath.c_str(), FALSE)) {
        std::wcout << L"Successfully copied and overwritten the DLL!" << std::endl;
        return true;
    }
    else {
        std::wcerr << L"Failed to copy the DLL. Error: " << GetLastError() << std::endl;
        return false;
    }
}

// Function to monitor directory changes
void MonitorDirectory() {
    HANDLE hDir = CreateFileW(
        WATCHED_DIRECTORY.c_str(),
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
        NULL
    );

    if (hDir == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to open directory for monitoring." << std::endl;
        return;
    }

    char buffer[1024];
    DWORD bytesReturned;
    OVERLAPPED overlapped = { 0 };

    while (true) {
        if (ReadDirectoryChangesW(
            hDir,
            buffer,
            sizeof(buffer),
            TRUE, // Watch subdirectories
            FILE_NOTIFY_CHANGE_LAST_WRITE | // Detect file modifications
            FILE_NOTIFY_CHANGE_FILE_NAME,   // Detect file name changes
            &bytesReturned,
            NULL,
            NULL)) {

			//std::cout << "Directory change detected!" << std::endl;

			// Check if the dll file has been modified
            
            FILE_NOTIFY_INFORMATION* fni = (FILE_NOTIFY_INFORMATION*)buffer;
            do {
                // Convert the file name to a wide string
                std::wstring fileName(fni->FileName, fni->FileNameLength / sizeof(WCHAR));
                if (fileName == L"ScriptWoof.dll") { // Replace with your DLL name
                    PostThreadMessage(GetCurrentThreadId(), WM_FILE_CHANGED, 0, 0);
					std::cout << "File change detected!" << std::endl;


					// Check if the DLL file is being used by another process
					if (GetFileAttributesW(DLL_COPY_DIRECTORY.c_str()) == INVALID_FILE_ATTRIBUTES) {
						// if used by another process, unload the DLL
						std::cout << "DLL is being used by another process. Unloading the DLL..." << std::endl;
						FreeLibrary(GetModuleHandleW(DLL_COPY_DIRECTORY.c_str()));
						break;
					}


					// Copy and overwrite the DLL
					if (CopyAndOverwriteDLL(DLL_MAIN_DIRECTORY, DLL_COPY_DIRECTORY)) {
						std::cout << "DLL copied and overwritten successfully!" << std::endl;
					}
					else {
						std::cerr << "Failed to copy and overwrite the DLL." << std::endl;
					}
                    break;
                }

                // Move to the next record if it exists
                fni = fni->NextEntryOffset ? (FILE_NOTIFY_INFORMATION*)((BYTE*)fni + fni->NextEntryOffset) : NULL;
            } while (fni);
        }
        else {
            std::cerr << "Failed to read directory changes." << std::endl;
        }
    }

    CloseHandle(hDir);
}

// Set up the hook
void SetCustomHook() {
    hHook = SetWindowsHookEx(WH_GETMESSAGE, HookProc, NULL, GetCurrentThreadId());
    if (!hHook) {
        MessageBoxW(NULL, L"Failed to set hook.", L"Error", MB_OK);
    }
}
