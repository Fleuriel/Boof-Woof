/**************************************************************************
 * @file Graphics.h
 * @author 	TAN Angus Yit Hoe
 * @param DP email: tan.a@digipen.edu [0067684]
 * @param Course: CS 350
 * @param Course: Advanced Computer Graphics II
 * @date  06/14/2024 (14 JUNE 2024)
 * @brief
 *
 * This file allows the creation and initialization of a OpenGL Application
 * Window which then allows user to draw functions and/or change states
 * depending on the game state manager.
 *
 *
 *************************************************************************/

#pragma once
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <GL/glew.h> // for access to OpenGL API declarations 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Shader.h"
#include <utility>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <unordered_map>

#include "../Input/Input.h"
#include "../../Headers/TestCases.h"
#include "BoundingVolume.h"
#include "GraphicsUserInterface.h"
#include "../GSM/GameStateMachine.h"
#include "../AssetManager/AssetManager.h"

/**************************************************************************
* @brief Allow usage of the newWindow variable for the OpenGL Pipeline
*************************************************************************/
extern GLFWwindow* newWindow;


/**************************************************************************
* @brief Model Enum
*************************************************************************/
enum ModelType {
	BOX, 
	SPHERE,
	POINTT,
	RAY,
	PLANE,
	TRIANGLE,
	ARROW
}; 



/**************************************************************************
* @brief Graphics Class
*************************************************************************/


class Graphics {
public:


	/**************************************************************************
	* @brief Initialization for Window creation and Pipeline of Graphics 
	*		 Separated
	*************************************************************************/
	static void initWindow();
	static void initGraphicsPipeline();

	/**************************************************************************
	* @brief Window Resize Callback
	*************************************************************************/
	static void OpenGLWindowResizeCallback(GLFWwindow* window, int width, int height);


	/**************************************************************************
	* @brief Update Every Loop
	*************************************************************************/
	void UpdateLoop();


	/**************************************************************************
	* @brief Draw the Models (with shaders)
	*************************************************************************/
	void Draw();


	/**************************************************************************
	* @brief Draw the Objects Models (Iterate through the objects to draw)
	*************************************************************************/
	static void DrawObject();

	/**************************************************************************
	* @brief Collection of Test cases to test against assignment tasks.
	*************************************************************************/
	void CheckTestsCollisions();



	static GLuint vaoid;					// VAO ID

	glm::mat4 model_To_NDC_xform;			// Model to NDC (We declare here so that we can do calculation outside of Shaders) 
	glm::vec3 Position;						// Position
	glm::vec3 ScaleModel;					// Scale

	float angleX;							// Angle of the Model (X)
	float angleY;							// Angle of the Model (Y)
	float angleZ;							// Angle of the Model (Z)
	float angleSpeed;						// Speed of the Model Displacement

	int TagID;								// Tag ID in the future if required assets and/or textures and/or force data into this tag.
	
	/**************************************************************************
	* @brief Shaders Variables
	*************************************************************************/
	static GLuint mdl_ref;
	static GLuint shd_ref; // Model and Shader Reference

	/**************************************************************************
* @brief Set Color to primitives
*************************************************************************/
	glm::vec3 setColor;					// Parsing Value to the Shader							
	glm::vec3 setColorLeft;				// Will set the value to setColor. (When Pushing to Shader)
	glm::vec3 setColorRight;			// Will set the value to setColor. (When Pushing to Shader)


	/**************************************************************************
	* @brief Position and Angle (Separate to easier control)
	*************************************************************************/
	glm::vec3 PositionLeft;
	glm::vec3 PositionRight;
	glm::vec3 AngleLeft;
	glm::vec3 AngleRight;

	/**************************************************************************
* @brief Struct of OpenGLModels
*		-> Creation of Objects usage of this struct.
*************************************************************************/
	struct OpenGLModel {
		std::string Name;
		GLenum primitive_type;		// Primitive Type
		size_t primitive_cnt;		// Primitive Count
		GLuint vaoid;				// Vaoid of the Model
		GLsizei draw_cnt;			// Draw Count of the model
		size_t idx_elem_cnt;		// Index Element Count of the Model


		OpenGLModel() : primitive_type(0), primitive_cnt(0), vaoid(0), draw_cnt(0), idx_elem_cnt(0) {}

	};


	/**************************************************************************
	* @brief Update Object Orientation and/or Position
	*************************************************************************/
	void Update(glm::vec3 position, glm::vec3 scale, glm::vec3 angle);

	/**************************************************************************
	* @brief Create Object with model and Tag
	*************************************************************************/
	static void CreateObject(OpenGLModel model, int Tag);


	/**************************************************************************
	* @brief What the model to create is
	*************************************************************************/
	OpenGLModel model_to_create;


	/**************************************************************************
	* @brief Container for OpenGL Models
	*************************************************************************/
	static std::vector<OpenGLModel> models;

	/**************************************************************************
	* @brief OpenGL Models
	*************************************************************************/
	static OpenGLModel BoxModel();
	static OpenGLModel SphereModel();
	static OpenGLModel PointModel();
	static OpenGLModel RayModel();
	static OpenGLModel PlaneModel();
	static OpenGLModel TriangleModel();
	static OpenGLModel ArrowModel();



	/**************************************************************************
	* @brief size
	*************************************************************************/
	float size = 1.0f;





	// Default constructor
	Graphics()
		: model_To_NDC_xform(1.0f), Position(0.0f), ScaleModel(1.0f),
		setColor(1.0f), setColorLeft(1.0f), setColorRight(1.0f),
		PositionLeft(0.0f), PositionRight(0.0f),
		AngleLeft(0.0f), AngleRight(0.0f),
		angleX(0.0f), angleY(0.0f), angleZ(0.0f),
		angleSpeed(0.0f), TagID(0) {}

	// Constructor with parameters
	Graphics(const OpenGLModel model, int id = 0)
		: model_To_NDC_xform(1.0f), Position(0.0f), ScaleModel(0.5f),
		setColor(1.0f), setColorLeft(1.0f), setColorRight(1.0f),
		PositionLeft(0.0f), PositionRight(0.0f),
		AngleLeft(0.0f), AngleRight(0.0f),
		angleX(0.0f), angleY(0.0f), angleZ(0.0f),
		angleSpeed(0.0f), TagID(id), model_to_create(model) {}

	// Deconstructor 
	~Graphics() {}


private:

	/**************************************************************************
	* @brief  private variables for Graphic Pipelines model creation etc.
	*************************************************************************/
	static unsigned int VBO, VAO, EBO;
};


	/**************************************************************************
	* @brief  Changeing modes from GL_FILL to GL_LINE
	*************************************************************************/
	inline bool togglePolygonMode;

/**************************************************************************
* @brief  allow other files to access Graphics class.
*************************************************************************/
extern Graphics graphicObject;


#endif