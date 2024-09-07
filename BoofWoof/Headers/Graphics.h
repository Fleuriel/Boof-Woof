/**************************************************************************
 * @file Graphics.cpp
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
#include <assimp/scene.h>  // Assimp header for aiNode and aiMesh
#include <utility>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <unordered_map>


#define RITTER_MODEL "Ritter"
#define LARSSON_MODEL "Larsson"
#define PCA_MODEL	"PCA"

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
	ARROW,
	RITTER,
	LARSSON,
	PCA,
	POWERPLANT,
	TESTING_MODEL
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

	void DrawModel();

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
	int count;								// Number Count of the object.


	/**************************************************************************
	* @brief Update Object Orientation and/or Position
	*************************************************************************/
	void Update(glm::vec3 position, glm::vec3 scale, glm::vec3 angle);


	/**************************************************************************
	* @brief Shaders Variables
	*************************************************************************/
	static GLuint mdl_ref;
	static GLuint shd_ref; // Model and Shader Reference



	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords; 
		glm::vec3 Tangent;
		// bitangent
		glm::vec3 Bitangent;
	};

	class Mesh {
	public:
		// mesh data
		std::vector<Vertex>       vertices;
		std::vector<unsigned int> indices;
		glm::vec3 Position;

		Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
		{
			this->vertices = vertices;
			this->indices = indices;
			
			setupMesh();
		}


		void Draw();


	private:

		void setupMesh();

	};




	class Model
	{
	public:
	
		Model() : directory("abc"), gammaCorrection(false){}
	
		// model data 
		std::vector<Mesh>    meshes;
		std::string directory;
		bool gammaCorrection;
	
		// constructor, expects a filepath to a 3D model.
		Model(std::string const& path, bool gamma = false) : gammaCorrection(gamma)
		{
			//loadModel(path);
		}
	
		// draws the model, and thus all its meshes
	
	
	private:
	
	};



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

		glm::vec3 Position;


		// model data 
		std::vector<Mesh>    meshes;
		std::string directory;

		OpenGLModel() :primitive_type(0), primitive_cnt(0), vaoid(0), draw_cnt(0), idx_elem_cnt(0), directory("") {}

		OpenGLModel(std::string const& path) : primitive_type(0), primitive_cnt(0), vaoid(0), draw_cnt(0), idx_elem_cnt(0), directory("")
		{
		}

		OpenGLModel(std::vector<std::string> const& path) : primitive_type(0), primitive_cnt(0), vaoid(0), draw_cnt(0), idx_elem_cnt(0), directory("")
		{
		}



		void Draw()
		{
			for (unsigned int i = 0; i < meshes.size(); i++)
				meshes[i].Draw();
		}


	}; 

	static void InputBoundingBoxes();

	OpenGLModel ModelControl;


	static OpenGLModel LoadModel(const std::string& path, const std::string& path2);


	static OpenGLModel ProcessMesh(aiMesh* mesh, const aiScene* scene);

	/**************************************************************************
	* @brief Create Object with model and Tag
	*************************************************************************/
	static void CreateObject(OpenGLModel model, int Tag, int Counter = 1);

	//static void CreateObjectFromFile(Model model, int Tag);

	/**************************************************************************
	* @brief What the model to create is
	*************************************************************************/
	OpenGLModel model_to_create;
	//Model modelModelCreate;

	/**************************************************************************
	* @brief Container for OpenGL Models
	*************************************************************************/
	static std::vector<OpenGLModel> models;

	//static std::vector<Model> OURModel;

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
	static OpenGLModel PowerPlantModel();
	static OpenGLModel TestingModel();
	static OpenGLModel SphereModelMethodRITTER();
	static OpenGLModel SphereModelMethodLARSSON();
	static OpenGLModel SphereModelMethodPCA();

	/**************************************************************************
	* @brief size
	*************************************************************************/
	float size = 1.0f;


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
	glm::vec3 ScaleRight;



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

	// Constructor with parameters
	//Graphics(const Model model, int id = 0)
	//	: model_To_NDC_xform(1.0f), Position(0.0f), ScaleModel(0.5f),
	//	setColor(1.0f), setColorLeft(1.0f), setColorRight(1.0f),
	//	PositionLeft(0.0f), PositionRight(0.0f),
	//	AngleLeft(0.0f), AngleRight(0.0f),
	//	angleX(0.0f), angleY(0.0f), angleZ(0.0f),
	//	angleSpeed(0.0f), TagID(id), modelModelCreate(model) {}

	// Deconstructor 
	~Graphics() {}




private:

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

extern Graphics::OpenGLModel ModelControl;


#endif