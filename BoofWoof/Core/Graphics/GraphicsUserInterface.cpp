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

#include "GraphicsUserInterface.h"
#include <iostream>
#include "../Graphics/BoundingVolume.h"

//Include IMGUI
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "../Headers/TestCases.h"

#include "Graphics.h"


// Set color to differentiate them
float color1[3] = { 65.0f / 255.0f,249.0f / 255.0f,237.0f / 255.0f };

float color2[3] = { 185 / 255.0f, 255 / 255.0f, 159 / 255.0f };


// Set Original Angles
// OBB not used.
int angle1[3] = { 0, 0, 0 };

int angle2[3] = { 0, 0, 0 };


// Set Original Position
int Position1[3] = { -40, 0, -10 };

int Position2[3] = {  40, 0, -10 };

float Scale = 0.01f;

int GraphicsUserInterface::options;

bool inputFieldFocused1 = false;




/**************************************************************************
* @brief Initialization function for ImGui
* @return void
*************************************************************************/
void GraphicsUserInterface::Initialize()
{

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(newWindow, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

/**************************************************************************
* @brief Update function for ImGui
* @return void
*************************************************************************/
void GraphicsUserInterface::Update()
{
	RenderLeftWindow();
	RenderRightWindow();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


/**************************************************************************
* @brief Rendering Left window with its various UI and testcases
* @return void
*************************************************************************/
void GraphicsUserInterface::RenderLeftWindow()
{
	ImGui::Begin("Left Window");
	ImGui::Text("Controls for Bounding Volume");
	ImGui::Checkbox("Toggle Render Mode", &togglePolygonMode);

	//ImGui::SliderFloat("Size", &graphicObject.size, 0.5f, 2.0f);
	ImGui::ColorEdit3("Color", color1);

//	ImGui::SliderInt3("Position", Position1, -100,100);

	if(TESTCASE::Point_vs_Triangle || TESTCASE::Ray_vs_Triangle)
	{ 
		ImGui::DragInt2("Position", Position1, 1.0f, -100, 100);
		Position1[2] = 0;
	}
	else
		ImGui::DragInt3("Position", Position1, 1.0f, -100, 100);



	if (ImGui::Button("Reset Pos"))
		for (int i = 0; i < 3; ++i)
			Position1[i] = 0;

//	ImGui::ColorEdit3("Position [X,Y,Z]", Position1);

	//ImGui::SliderFloat("Position X", Position1, -300, 300);


	ImGui::SliderInt3("Angles", angle1, 0, 360);


	if (ImGui::Button("Reset Angle"))
		for (int i = 0; i < 3; ++i)
			angle1[i] = 0;

	using namespace TESTCASE;

	//if (ImGui::RadioButton("Draw Rectangle", &options))
	//{
	//	drawRectangle = !drawRectangle;
	//}
	if (ImGui::RadioButton("Sphere vs Sphere", &options))
	{
		Sphere_vs_Sphere = !Sphere_vs_Sphere;
		turnOnOffTest(SPHERE_VS_SPHERE);
	}
	if (ImGui::RadioButton("AABB vs Sphere", &options))
	{
		AABB_vs_Sphere = !AABB_vs_Sphere;
		turnOnOffTest(AABB_VS_SPHERE);
	}
	if (ImGui::RadioButton("Sphere vs AABB", &options))
	{
		Sphere_vs_AABB = !Sphere_vs_AABB;
		turnOnOffTest(SPHERE_VS_AABB);
	}
	if (ImGui::RadioButton("AABB vs AABB", &options))
	{
		AABB_vs_AABB = !AABB_vs_AABB;
		turnOnOffTest(AABB_VS_AABB);
	}
	if (ImGui::RadioButton("Point vs Sphere", &options))
	{
		Point_vs_Sphere = !Point_vs_Sphere;
		turnOnOffTest(POINT_VS_SPHERE);
	}
	if (ImGui::RadioButton("Point vs AABB", &options))
	{
		Point_vs_AABB = !Point_vs_AABB;
		turnOnOffTest(POINT_VS_AABB);
	}
	if (ImGui::RadioButton("Point vs Plane", &options))
	{
		Point_vs_Plane = !Point_vs_Plane;
		turnOnOffTest(POINT_VS_PLANE);
	}
	if (ImGui::RadioButton("Point vs Triangle", &options))
	{
		Point_vs_Triangle = !Point_vs_Triangle;
		turnOnOffTest(POINT_VS_TRIANGLE);
	}
	if (ImGui::RadioButton("Ray vs Plane", &options))
	{
		Ray_vs_Plane = !Ray_vs_Plane;
		turnOnOffTest(RAY_VS_PLANE);
	}
	if (ImGui::RadioButton("Ray vs Triangle", &options))
	{
		Ray_vs_Triangle = !Ray_vs_Triangle;
		turnOnOffTest(RAY_VS_TRIANGLE);
	}
	if (ImGui::RadioButton("Ray vs AABB", &options))
	{
		Ray_vs_AABB = !Ray_vs_AABB;
		turnOnOffTest(RAY_VS_AABB);
	}
	if (ImGui::RadioButton("Ray vs Sphere", &options))
	{
		Ray_vs_Sphere = !Ray_vs_Sphere;
		turnOnOffTest(RAY_VS_SPHERE);
	}
	if (ImGui::RadioButton("Plane vs AABB", &options))
	{
		Plane_vs_AABB = !Plane_vs_AABB;
		turnOnOffTest(PLANE_VS_AABB);
	}
	if (ImGui::RadioButton("Plane vs Sphere", &options))
	{
		Plane_vs_Sphere = !Plane_vs_Sphere;
		turnOnOffTest(PLANE_VS_SPHERE);
	}

	graphicObject.PositionLeft = glm::vec3(Position1[0] * Scale, Position1[1] * Scale, Position1[2] * Scale);
	graphicObject.size = 0.5f;
	graphicObject.AngleLeft = glm::vec3(angle1[0], angle1[1], angle1[2]);
	graphicObject.setColorLeft = glm::vec3(color1[0], color1[1], color1[2]);

	ImGui::End();
}

/**************************************************************************
* @brief Rendering Right window with its various UI.
* @return void
*************************************************************************/
void GraphicsUserInterface::RenderRightWindow()
{
	ImGui::Begin("Right Window");
	ImGui::Text("Right Window Controls");
	
	
	//ImGui::SliderFloat("Size", &graphicObject.size, 0.5f, 2.0f);
	ImGui::ColorEdit3("Color", color2);
	if (TESTCASE::Point_vs_Triangle || TESTCASE::Ray_vs_Triangle)
	{
		ImGui::DragInt2("Position", Position2, 1.0f, -100, 100);
		Position2[2] = 0;
	}
	else
		ImGui::DragInt3("Position", Position2, 1.0f, -100, 100);

	if (ImGui::Button("Reset Pos"))
		for (int i = 0; i < 3; ++i)
			Position2[i] = 0;

	ImGui::SliderInt3("Angles", angle2, 0, 360);


	if (ImGui::Button("Reset Angles"))
		for (int i = 0; i < 3; ++i)
			angle2[i] = 0;
	
	

	graphicObject.PositionRight = glm::vec3(Position2[0] * Scale, Position2[1] * Scale, Position2[2] * Scale);
	graphicObject.size = 0.5f;
	graphicObject.AngleRight = glm::vec3(angle2[0], angle2[1], angle2[2]);
	graphicObject.setColorRight = glm::vec3(color2[0], color2[1], color2[2]);
	
	ImGui::End();
	
}

/**************************************************************************
* @brief Render GUI that has been created, but not rendered yet.
* @return void
*************************************************************************/
void GraphicsUserInterface::RenderGUI()
{
	// create new frame for ImGUI
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Enable Docking using main Viewports
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
}

/**************************************************************************
* @brief Cleanup used resource to prevent Memleaks
* @return void
*************************************************************************/
void GraphicsUserInterface::Cleanup()
{
	// cleanup after usage.
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
