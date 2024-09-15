/**************************************************************************
 * @file GraphicsUserInterface.cpp
 * @author 	TAN Angus Yit Hoe
 * @param DP email: tan.a@digipen.edu [0067684]
 * @param Course: CS 350
 * @param Course: Advanced Computer Graphics II
 * @date  06/14/2024 (14 JUNE 2024)
 * @brief
 *
 * This file allows holds the ImGui mainframe to render UI Elements onto
 * the OpenGL Application.
 *
 *************************************************************************/

#pragma once
#include "BoundingVolume.h"

 //Include IMGUI
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "TestCases.h"

#include "Graphics.h"

#include <assimp/ai_assert.h>

#ifndef GRAPHICSUSERINTERFACE_H
#define GRAPHICSUSERINTERFACE_H

/**************************************************************************
* @brief  GraphicsUserInterface Class
*************************************************************************/
class GraphicsUserInterface
{
public:
	static void Initialize();
	static void Update();
	static void RenderGUI();
	static void Cleanup();


	static void RenderLeftWindow();
	static void RenderRightWindow();

	static int options;



private:




};

extern int Position1[3];
extern int LookAt[3];

#endif