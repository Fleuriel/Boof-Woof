

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include "ImGuiEditor.h"
#include <glfw3.h>
#include <iostream>

GLFWwindow* win;

int main()
{

#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
#endif

	// Check glfwInit
	if (!glfwInit())
	{
		return -1;
	}

	//for use with debugging, change the value to the location of the mem leak per the crt debug info from the console
	// refer to stack frame and see where it all went wrong
	//_crtBreakAlloc = 372;

	g_ImGuiEditor.ImGuiInit();

	while (!glfwWindowShouldClose(win))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		g_ImGuiEditor.ImGuiUpdate();
		g_ImGuiEditor.ImGuiRender();




		//these two needs to be at the end.
		glfwSwapBuffers(win);
		glfwPollEvents();

	}

	g_ImGuiEditor.ImGuiEnd();

	return 0;
}