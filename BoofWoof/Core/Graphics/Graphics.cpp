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

#include "Graphics.h"

#include <iostream>
#include <array>
#include <list>
#include <sstream>
#include <string>
#include <initializer_list>




GLFWwindow* newWindow;
AssetManager assetManager;

Graphics graphicObject;

std::list<Graphics> objects;					// singleton

GLuint Graphics::mdl_ref;// , Graphics::shd_ref; // Model and Shader Reference

GLuint Graphics::shd_ref;

unsigned int Graphics::VAO;
unsigned int Graphics::EBO;
unsigned int Graphics::VBO;

GLuint Graphics::vaoid;

float boundingRadiuses[3];


std::vector<Graphics::OpenGLModel> Graphics::models;
//std::vector<Graphics::Model> Graphics::OURModel;

std::pair<int, int> windowSize;

bool glewInitialized = false;

glm::vec3 CUBE_HALFEXTENTS = glm::vec3(0.25f);
float SPHERE_RADIUS = 0.25f;


glm::vec3 arrowScale = glm::vec3(0.0f, 0.0f, 0.0f);



/**************************************************************************
* @brief Initialize Window
* @return void
*************************************************************************/
void Graphics::initWindow()
{
	std::cout << "Initializing OpenGL Window Settings\n";


	// As of now.
	windowSize.first = 1920;
	windowSize.second = 1080;


	newWindow = glfwCreateWindow(windowSize.first, windowSize.second, "Assignment 1", NULL, NULL);

	if (!newWindow)
	{
		glfwTerminate();

		std::cout << "Window parameters are wrong.\n";
		std::cout << "Parameter: WinSize Width: " << windowSize.first << " (If it is 0, Error)" << '\n';
		std::cout << "Parameter: WinSize Height: " << windowSize.second << " (If it is 0, Error)" << '\n';

		return;
	}

	// Tell GLFW we are using OpenGL 4.5
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);


	// Tell GLFW that we are using the CORE Profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create viewport of width and height.
	glViewport(0, 0, windowSize.first, windowSize.second);

	glfwSwapInterval(1);

	// Receives Key input/output [Checks for Key Presses]
	glfwSetKeyCallback(newWindow, KeyCallBack);

	// Receives Mouse input/output [Checks for Mouse Clicks]
	glfwSetMouseButtonCallback(newWindow, MouseCallBack);

	//glfwSetCursorPosCallback(newWindow, MouseMovementCallBack);


	//glfwSetScrollCallback(newWindow, ScrollCallBack);
	//
	//glfwSetWindowFocusCallback(newWindow, windowFocusCallback);


	glfwSetWindowSizeCallback(newWindow, OpenGLWindowResizeCallback);

	// Make the current window the current context
	glfwMakeContextCurrent(newWindow);

	// Set input mode for the window with the cursor (Enables Cursor Input)
	glfwSetInputMode(newWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	glEnable(GL_DEPTH_TEST);

	glDepthRange(0.0f, 10.0f);


	if (!glewInitialized)
	{
		GLenum err = glewInit();
		if (err != GLEW_OK)
		{
			std::cerr << "Error initializing GLEW: " << glewGetErrorString(err) << std::endl;
			return;
		}
		glewInitialized = true;
	}

}

/**************************************************************************
* @brief Window Resize Callback
* @param GLFWwindow 
* @param width
* @param height
* @return void
*************************************************************************/
void Graphics::OpenGLWindowResizeCallback(GLFWwindow* window, int width, int height)
{
	(void)window;

	windowSize.first = width;
	windowSize.second = height;
	
	glViewport(0, 0, windowSize.first, windowSize.second);
}


/**************************************************************************
* @brief Graphics Pipeline
* @return void
*************************************************************************/
void Graphics::initGraphicsPipeline()
{
	// OpenGL Initialization
	std::cout << "Initializing Graphics Pipeline\n";


	graphicObject.setColor = glm::vec3(1.0f, 0.5f, 0.25f);


	//Graphics::Model ourModel("../Adv Computer Graphics/Model/TestingModel/teapot_mid_poly.obj");

	//OURModel.push_back(ourModel);

	shd_ref = 0;



	assetManager.txtFile = assetManager.readFilePathsFromFile(FILEPATH_LOADING_MODELS);
	assetManager.mtlFile = assetManager.readFilePathsFromFileMaterial(FILEPATH_LOADING_MODELS);





		// Initiate Model
	models.emplace_back(Graphics::BoxModel());
	models.emplace_back(Graphics::SphereModel());
	models.emplace_back(Graphics::PointModel());
	models.emplace_back(Graphics::RayModel());
	models.emplace_back(Graphics::PlaneModel());
	models.emplace_back(Graphics::TriangleModel());
	models.emplace_back(Graphics::ArrowModel());
	models.emplace_back(Graphics::SphereModelMethodRITTER());
	models.emplace_back(Graphics::SphereModelMethodLARSSON());
	models.emplace_back(Graphics::SphereModelMethodPCA());




//	models.emplace_back(Graphics::PowerPlantModel());
	
	for (int i = 0; i < assetManager.txtFile.size(); ++i)
	{
//		std::cout << assetManager.txtFile[i] << '\n';
		models.emplace_back(LoadModel(assetManager.txtFile[i], assetManager.mtlFile[i]));
	}
	

	if (PROJECT_ONE)
	{
		// Also for convenience sake, creating 2x of each BB would give us 2 data variable to compare against.
		for (int i = 0; i < 2; ++i)
		{
			CreateSphere();
			CreateAABB();
			CreateRay();
			CreatePlane();
			CreatePoint();
			CreateTriangle();
		}


#ifdef _DEBUG
		std::cout << "======================================================================\n";
		std::cout << "AABB Container Size: \t\t" << AABBContainer.size() << '\n';
		std::cout << "Sphere Container Size: \t\t" << SphereContainer.size() << '\n';
		std::cout << "Ray Container Size: \t\t" << RayContainer.size() << '\n';
		std::cout << "Point Container Size: \t\t" << PointContainer.size() << '\n';
		std::cout << "Triangle Container Size: \t" << TriangleContainer.size() << '\n';
		std::cout << "Plane Contianer Size: \t\t" << PlaneContainer.size() << '\n';
		std::cout << "======================================================================\n";
#endif



#ifdef _DEBUG
		std::cout << "======================================================================\n";
		std::cout << "Primitive Counts for each :\n";
#endif


		CreateObject(models[BOX], 1);

#ifdef _DEBUG
		std::cout << "Box\t" << models[BOX].primitive_cnt << '\t';
#endif


		CreateObject(models[SPHERE], 2);


#ifdef _DEBUG
		std::cout << "Sphere\t\t" << models[SPHERE].primitive_cnt << '\n';

#endif

		CreateObject(models[POINTT], 3);

#ifdef _DEBUG
		std::cout << "Point\t" << models[POINTT].primitive_cnt << '\t';

#endif

		CreateObject(models[RAY], 4);

#ifdef _DEBUG
		std::cout << "Ray\t\t" << models[RAY].primitive_cnt << '\n';

#endif

		CreateObject(models[PLANE], 5);

#ifdef _DEBUG
		std::cout << "Plane\t" << models[PLANE].primitive_cnt << '\t';
#endif

		CreateObject(models[TRIANGLE], 6);

#ifdef _DEBUG
		std::cout << "Triangle\t" << models[TRIANGLE].primitive_cnt << '\n';
#endif


#ifdef _DEBUG
		std::cout << "===============================ARROW==================================\n";
#endif 

		// Point Pointer. 
		CreateObject(models[ARROW], 7);

#ifdef _DEBUG
		std::cout << "======================================================================\n";
#endif 
	}
	else if (PROJECT_TWO)
	{
#ifdef _DEBUG
		std::cout << "===============================POWERPLANT=============================\n";
#endif 


		for (int i = 0; i < assetManager.txtFile.size(); ++i)
		{
			CreateObject(models[POWERPLANT + i], POWERPLANT + 1 + i);
			CreateObject(models[BOX], 1, i);
			CreateObject(models[SPHERE], 2, i);
			CreateObject(models[RITTER], 8, i);
			CreateObject(models[LARSSON], 9, i);
			CreateObject(models[PCA], 10, i);
			// Creating AABB objects.
			CreateAABB();

		
		}
		


		


#ifdef _DEBUG
		std::cout << "======================================================================\n";
#endif 
		
	}



	glClearColor(0.22f, 0.4f, 0.5f, 0.6f);
	std::cout << "nth wrong\n";
}


void Graphics::Mesh::setupMesh()
{
	// create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// A great thing about structs is that their memory layout is sequential for all its items.
	// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
	// again translates to 3/2 floats which translates to a byte array.
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// set the vertex attribute pointers
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	// vertex tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
	// vertex bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));


	glBindVertexArray(0);
}

void Graphics::Mesh::Draw()
{

	assetManager.shdrpgms[shd_ref].Use();



	// send data to shaders
	GLint uniform_var_loc1 = glGetUniformLocation(assetManager.shdrpgms[0].GetHandle(), "colorSet");
	if (uniform_var_loc1 >= 0) {
		glUniform3fv(uniform_var_loc1, 1, glm::value_ptr(graphicObject.setColor));
	}
	else {
		std::cout << "Uniform 'colorSet' doesn't exist!!!" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	uniform_var_loc1 = glGetUniformLocation(assetManager.shdrpgms[0].GetHandle(), "size");
	if (uniform_var_loc1 >= 0) {
		glUniform1f(uniform_var_loc1, graphicObject.size);
	}
	else {
		std::cout << "Uniform 'size' doesn't exist!!!" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	uniform_var_loc1 = glGetUniformLocation(assetManager.shdrpgms[0].GetHandle(), "uModel_to_NDC");
	if (uniform_var_loc1 >= 0) {
		glUniformMatrix4fv(uniform_var_loc1, 1, GL_FALSE, glm::value_ptr(graphicObject.model_To_NDC_xform));
	}



	// draw mesh
	glBindVertexArray(Graphics::VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	assetManager.shdrpgms[shd_ref].UnUse();

}


/**************************************************************************
* @brief Creation of Objects
* @param OpenGLModel
* @param TagID
* @return void
*************************************************************************/
void Graphics::CreateObject(OpenGLModel model, int Tag, int Counter)
{
	int value = 0;
	if (PROJECT_ONE)
	{

		if (model.Name == "Box Model")
			value = BOX;
		if (model.Name == "Sphere Model")
			value = SPHERE;
		if (model.Name == "Point Model")
			value = POINTT;
		if (model.Name == "Ray Model")
			value = RAY;
		if (model.Name == "Plane Model")
			value = PLANE;
		if (model.Name == "Triangle Model")
			value = TRIANGLE;
		if (model.Name == "Arrow Model")
			value = ARROW;

	}
	else if (PROJECT_TWO)
	{
		if (model.Name == "PowerPlant Model")
			value = 7;
		else
			value = 8;
	}


	Graphics newObject(model, Tag);
	newObject.mdl_ref = value;
	newObject.count = Counter;


	std::cout  <<"Creation: Model Reference: " << newObject.mdl_ref << '\n';

	objects.emplace_back(newObject);

	std::cout << objects.size() << '\n';
}


//void Graphics::CreateObjectFromFile(Model model, int Tag)
//{
//
//	Graphics newObject(model, Tag);
//
//	objects.emplace_back(newObject);
//}


/**************************************************************************
* @brief Collection of Test CASES
* @return void
*************************************************************************/
void Graphics::CheckTestsCollisions()
{
	if (PROJECT_ONE)
	{

	if (TESTCASE::Sphere_vs_Sphere)
	{
		if (checkCollisionSphere(SphereContainer[0], SphereContainer[1]))
		{
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		}
		else
		{
			glClearColor(0.22f, 0.4f, 0.5f, 0.6f);
		}
	}
	else if (TESTCASE::AABB_vs_Sphere)
	{
		if (checkCollisionAABB_SPHERE(AABBContainer[0], SphereContainer[1]))
		{
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		}
		else
		{
			glClearColor(0.22f, 0.4f, 0.5f, 0.6f);
		}
	}
	else if (TESTCASE::Sphere_vs_AABB)
	{
		if(checkCollisionAABB_SPHERE(SphereContainer[0], AABBContainer[1])) 
		{
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		}
		else
		{
			glClearColor(0.22f, 0.4f, 0.5f, 0.6f);
		}
	}
	else if (TESTCASE::AABB_vs_AABB)
	{
		if (checkCollisionAABB(AABBContainer[0], AABBContainer[1]))
		{
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		}
		else
		{
			glClearColor(0.22f, 0.4f, 0.5f, 0.6f);
		}
	}
	else if (TESTCASE::Point_vs_Sphere)
	{
		if (checkCollisionPointSphere(PointContainer[0], SphereContainer[1]))
		{
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		}
		else
		{
			glClearColor(0.22f, 0.4f, 0.5f, 0.6f);
		}
	}
	else if (TESTCASE::Point_vs_AABB)
	{

		if (checkCollisionPointAABB(PointContainer[0], AABBContainer[1]))
		{
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		}
		else
		{
			glClearColor(0.22f, 0.4f, 0.5f, 0.6f);
		}

	}
	else if (TESTCASE::Point_vs_Plane)
	{
		if (checkCollisionPointPlane(PointContainer[0], PlaneContainer[1]))
		{
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		}
		else
		{
			glClearColor(0.22f, 0.4f, 0.5f, 0.6f);
		}

	}
	else if (TESTCASE::Point_vs_Triangle)
	{
		if(checkCollisionPointTriangle(PointContainer[0], TriangleContainer[1]))
		{
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		}
		else
		{
			glClearColor(0.22f, 0.4f, 0.5f, 0.6f);
		}


	}
	else if (TESTCASE::Ray_vs_Plane)
	{
		if(checkCollisionRayPlane(RayContainer[0], PlaneContainer[1]))
		{
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		}
		else
		{
			glClearColor(0.22f, 0.4f, 0.5f, 0.6f);
		}

	}
	else if (TESTCASE::Ray_vs_Triangle)
	{
		if(checkCollisionRayTriangle(RayContainer[0],TriangleContainer[1]))
		{
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		}
		else
		{
			glClearColor(0.22f, 0.4f, 0.5f, 0.6f);
		}

	}
	else if (TESTCASE::Ray_vs_AABB)
	{
		if(checkCollisionRayAABB(RayContainer[0], AABBContainer[1]))
		{
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		}
		else
		{
			glClearColor(0.22f, 0.4f, 0.5f, 0.6f);
		}

	}
	else if (TESTCASE::Ray_vs_Sphere)
	{
		if(checkCollisionRaySphere(RayContainer[0], SphereContainer[1]))
		{
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		}
		else
		{
			glClearColor(0.22f, 0.4f, 0.5f, 0.6f);
		}

	}
	else if (TESTCASE::Plane_vs_AABB)
	{
		if(checkCollisionPlaneAABB(PlaneContainer[0], AABBContainer[1]))
		{
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		}
		else
		{
			glClearColor(0.22f, 0.4f, 0.5f, 0.6f);
		}
	}
	else if (TESTCASE::Plane_vs_Sphere)
	{
		if(checkCollisionPlaneSphere(PlaneContainer[0], SphereContainer[1]))
		{
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		}
		else
		{
			glClearColor(0.22f, 0.4f, 0.5f, 0.6f);
		}
	}
	else
	{
		glClearColor(0.22f, 0.4f, 0.5f, 0.6f);
	}

	}


}


/**************************************************************************
* @brief Update Loop
* @return void
*************************************************************************/
void Graphics::UpdateLoop()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	graphicObject.model_To_NDC_xform = model_To_NDC_xform;

	if (PROJECT_ONE)
	{

		if (inputSystem.GetKeyState(GLFW_KEY_A) == 1)
		{
			std::cout << "Position left " << PositionLeft.x << '\t' << PositionLeft.y << '\t' << PositionLeft.z << '\n';
			std::cout << "PositionRight " << PositionRight.x << '\t' << PositionRight.y << '\t' << PositionRight.z << '\n';
		}


		CheckTestsCollisions();


		// Iterate through all objects and set variables and/or stuff so that it matches what is on the screen
		for (auto& obj : objects)
		{

			if (obj.TagID == 1)
			{
				glm::vec3 changePlaneRot = glm::vec3(0.0f, 0.0f, 0.0f);	

				if (TESTCASE::Plane_vs_AABB || TESTCASE::Plane_vs_Sphere)
				{
					changePlaneRot = glm::vec3(0.0f, 0.0f, 90.0f);
					obj.Update(PositionLeft, glm::vec3(1.0f, 1.0f, 1.0f), changePlaneRot);
				}
				else
					obj.Update(PositionLeft, glm::vec3(1.0f, 1.0f, 1.0f), AngleLeft);
			}

			if (obj.TagID == 2)
			{
				// ONLY FOR PLANE FOR OBJECT 2
				glm::vec3 changePlaneRot = glm::vec3(0.0f, 0.0f, 0.0f);
				if (TESTCASE::Point_vs_Plane || TESTCASE::Ray_vs_Plane)
				{
					changePlaneRot = glm::vec3(0.0f, 0.0f, 90.0f);

					obj.Update(PositionRight, glm::vec3(1.0f, 1.0f, 1.0f), changePlaneRot);
				}
				else
					obj.Update(PositionRight, glm::vec3(1.0f, 1.0f, 1.0f), AngleRight);


			}
			// ARROW to show Point
			if (obj.TagID == 7)
			{

				if (TESTCASE::Point_vs_Sphere || TESTCASE::Point_vs_AABB || TESTCASE::Point_vs_Plane || TESTCASE::Point_vs_Triangle)
				{
					arrowScale = glm::vec3(1.0f, 1.0f, 1.0f);
				}
				else
				{
					arrowScale = glm::vec3(0.0f, 0.0f, 0.0f);
				}
				obj.Update(PositionLeft + glm::vec3(0.0f, 0.7f, 0.0f), arrowScale, glm::vec3(0.0f, 0.0f, 180.0f));

			}
		}
	}
	else if (PROJECT_TWO)
	{
		static int speedUp = 1;
		if (inputSystem.GetKeyState(GLFW_KEY_LEFT_SHIFT))
		{
			speedUp = 5;
		}
		else
		{
			speedUp = 1;
		}


		if (inputSystem.GetKeyState(GLFW_KEY_A))
			Position1[0] -= 100.f * speedUp;

		if (inputSystem.GetKeyState(GLFW_KEY_D))
			Position1[0] += 100.f * speedUp;


		if (inputSystem.GetKeyState(GLFW_KEY_W))
			Position1[2] -= 100.f * speedUp;

		if (inputSystem.GetKeyState(GLFW_KEY_S))
			Position1[2] += 100.f * speedUp;


		if (inputSystem.GetKeyState(GLFW_KEY_Z))
			Position1[1] += 100.f * speedUp;

		if (inputSystem.GetKeyState(GLFW_KEY_X))
			Position1[1] -= 100.f * speedUp;


		if (TESTCASE::mouseEnabled)
		{
			static double mouseX, mouseY;
			static int fB_X, fB_Y;


			glfwGetCursorPos(newWindow, &mouseX, &mouseY);

			glfwGetFramebufferSize(newWindow, &fB_X, &fB_Y);


			// Convert to Cartesian Coordiates

			mouseX = mouseX - fB_X / 2.0;
			mouseY = fB_Y / 2.0 - mouseY; // Note the inversion for Y-axis


			static int Speed = 200;

			LookAt[0] = Speed * -mouseX - graphicObject.PositionLeft.x;
			LookAt[1] = Speed * mouseY - graphicObject.PositionLeft.y;


		}








		//std::cout <<"Obj Size: " << objects.size() << '\n';
		for (auto& obj : objects)
		{
			std::cout << obj.TagID << '\t';
			std::cout << obj.mdl_ref << '\n';

			if (obj.TagID == 1 && TESTCASE::AABB_BOUNDING_VOLUME)
			{

				for (int i = 0; i < assetManager.txtFile.size(); ++i)
				{
					if (obj.count == i)
					{
						float AABBScaling = std::max({ assetManager.modelAABBHalfExtents[i].x,assetManager.modelAABBHalfExtents[i].y,assetManager.modelAABBHalfExtents[i].z });

						
						obj.Update(PositionRight + assetManager.modelCenterCoord[i] * glm::vec3(0.5f), glm::vec3(AABBScaling), AngleRight);
					}

				}


			}
			else if (obj.TagID == 2)
			{				
				for (int i = 0; i < assetManager.txtFile.size(); ++i)
				{
					if (obj.count == i)
					{
						float AABBScaling = std::max({ assetManager.modelAABBHalfExtents[i].x,assetManager.modelAABBHalfExtents[i].y,assetManager.modelAABBHalfExtents[i].z });

						obj.Update(PositionRight + assetManager.modelCenterCoord[i] * glm::vec3(0.5f), glm::vec3(AABBScaling), AngleRight);
					}
				}

			}
			else if (obj.TagID == 3) 
			{
				if (TESTCASE::SPHERE_RITTER)
				{
					for (int i = 0; i < assetManager.txtFile.size(); ++i)
					{
						if (obj.count == i)
						{
							float AABBScaling = std::max({ assetManager.modelAABBHalfExtents[i].x,assetManager.modelAABBHalfExtents[i].y,assetManager.modelAABBHalfExtents[i].z });
			
							obj.Update(PositionRight + assetManager.modelCenterCoord[i] * glm::vec3(0.5f), glm::vec3(boundingRadiuses[0]), AngleRight);
						}
					}
				}
				else
				{
					continue;
				}
			
			}
			else if (obj.TagID == 4)
			{
				if (TESTCASE::SPHERE_LARSSON)
				{
					for (int i = 0; i < assetManager.txtFile.size(); ++i)
					{
						if (obj.count == i)
						{
							float AABBScaling = std::max({ assetManager.modelAABBHalfExtents[i].x,assetManager.modelAABBHalfExtents[i].y,assetManager.modelAABBHalfExtents[i].z });

							obj.Update(PositionRight + assetManager.modelCenterCoord[i] * glm::vec3(0.5f), glm::vec3(boundingRadiuses[1]), AngleRight);
						}
					}
				}
				else
					continue;
			}
			else if (obj.TagID == 5)
			{
				if (TESTCASE::SPHERE_PCA)
				{
					for (int i = 0; i < assetManager.txtFile.size(); ++i)
					{
						if (obj.count == i)
						{
							float AABBScaling = std::max({ assetManager.modelAABBHalfExtents[i].x,assetManager.modelAABBHalfExtents[i].y,assetManager.modelAABBHalfExtents[i].z });
			
							obj.Update(PositionRight + assetManager.modelCenterCoord[i] * glm::vec3(0.5f), glm::vec3(boundingRadiuses[2]), AngleRight);
						}
					}
				}
			}
			else
			{
				// Powerplant  Section V
				obj.Update(PositionRight, ScaleRight, AngleRight);
			}
			


			//std::cout << obj.mdl_ref << '\t' << obj.count << '\t' << obj.TagID << '\t';
			//PrintData("Object Pos", obj.Position);


		}


	}


	// If True, set to line else FILL it with color.
	if (togglePolygonMode == true) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (togglePolygonMode == false) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	// Update the state for keypresses
	inputSystem.UpdateStatesForNextFrame();

	DrawObject();
}


/**************************************************************************
* @brief Draw Objects
* @return void
*************************************************************************/
void Graphics::DrawObject()
{
	for (auto &obj : objects)
	{
		
		if (PROJECT_ONE)
		{
			if (obj.TagID == 1)
			{
				if (TESTCASE::Sphere_vs_Sphere || TESTCASE::Sphere_vs_AABB)
				{
					obj.mdl_ref = SPHERE;
					SphereContainer[0].m_Position = obj.Position;
					SphereContainer[0].m_Radius = SPHERE_RADIUS;

				}
				else if (TESTCASE::AABB_vs_Sphere || TESTCASE::AABB_vs_AABB)
				{
					obj.mdl_ref = BOX;
					AABBContainer[0].m_Center = obj.Position;
					AABBContainer[0].m_HalfExtents = CUBE_HALFEXTENTS;

				}
				else if (TESTCASE::Point_vs_Sphere || TESTCASE::Point_vs_AABB || TESTCASE::Point_vs_Plane || TESTCASE::Point_vs_Triangle)
				{
					PointContainer[0].point = obj.Position;
					obj.mdl_ref = POINTT;
				}
				else if (TESTCASE::Ray_vs_Plane || TESTCASE::Ray_vs_Triangle || TESTCASE::Ray_vs_AABB || TESTCASE::Ray_vs_Sphere)
				{
					RayContainer[0].startPoint = obj.Position;
					RayContainer[0].endPoint = obj.Position + glm::vec3(0.5f, 0.0f, 0.0f);
					obj.mdl_ref = RAY;
				}
				else if (TESTCASE::Plane_vs_AABB || TESTCASE::Plane_vs_Sphere)
				{
					PlaneContainer[0].m_CenterPosition = obj.Position;
					PlaneContainer[0].m_HalfExtents = CUBE_HALFEXTENTS - glm::vec3(0.25f, 0.0f, 0.0f);


					obj.mdl_ref = PLANE;
				}
				else
				{
					// DEFAULT
					obj.mdl_ref = BOX;
				}
			}
			if (obj.TagID == 2)
			{
				if (TESTCASE::Sphere_vs_Sphere || TESTCASE::Point_vs_Sphere || TESTCASE::Ray_vs_Sphere || TESTCASE::Plane_vs_Sphere || TESTCASE::AABB_vs_Sphere)
				{
					obj.mdl_ref = SPHERE;
					SphereContainer[1].m_Position = obj.Position;
					SphereContainer[1].m_Radius = SPHERE_RADIUS;
				}
				if (TESTCASE::Sphere_vs_AABB || TESTCASE::AABB_vs_AABB || TESTCASE::Point_vs_AABB || TESTCASE::Ray_vs_AABB || TESTCASE::Plane_vs_AABB)
				{
					obj.mdl_ref = BOX;
					AABBContainer[1].m_Center = obj.Position;
					AABBContainer[1].m_HalfExtents = CUBE_HALFEXTENTS;
				}
				if (TESTCASE::Point_vs_Triangle || TESTCASE::Ray_vs_Triangle)
				{
					obj.mdl_ref = TRIANGLE;
					// We do not want to touch Z Axis as "Triangle" is 2D, and so XY Coords ONLY
					// Up
					TriangleContainer[1].firstVertex = obj.Position + glm::vec3(0.0f, 0.25, 0.0f);
					// Bottom left
					TriangleContainer[1].secondVertex = obj.Position + glm::vec3(-0.25f, -0.25f, 0.0f);
					// Bottom Right
					TriangleContainer[1].thirdVertex = obj.Position + glm::vec3(0.25f, -0.25f, 0.0f);
				}
				if (TESTCASE::Point_vs_Plane || TESTCASE::Ray_vs_Plane)
				{
					PlaneContainer[1].m_CenterPosition = obj.Position;
					PlaneContainer[1].m_HalfExtents = CUBE_HALFEXTENTS - glm::vec3(0.25f, 0.0f, 0.0f);
					//PlaneContainer[1].vertex1 = obj.Position;
					//PlaneContainer[1].vertex2 = obj.Position + glm::vec3(0.0f, 0.25f, 0.0f);
					//PlaneContainer[1].vertex3 = obj.Position + glm::vec3(0.0f, 0.0f, 0.25f);
					//PlaneContainer[1].vertex4 = obj.Position + glm::vec3(0.0f, 0.25f, 0.25f);
					obj.mdl_ref = PLANE;
				}
			}


			if (obj.TagID == 3)
			{
				continue;
			}
			if (obj.TagID == 4)
			{
				continue;
			}
			if (obj.TagID == 5)
			{
				continue;
			}
			if (obj.TagID == 6)
			{
				continue;
			}
			if (obj.TagID == 7)
			{
				if (TESTCASE::Point_vs_Sphere || TESTCASE::Point_vs_AABB || TESTCASE::Point_vs_Plane || TESTCASE::Point_vs_Triangle)
				{
					obj.mdl_ref = ARROW;
				}
				else
					continue;
			}
			if (obj.TagID == 8)
			{

			}

			obj.Draw();
		}
		else if(PROJECT_TWO)
		{

			obj.mdl_ref = obj.TagID - 1;
			//std::cout << "Object's mdl Ref " << obj.mdl_ref << '\n';
#ifdef _DEBUG
			//std::cout<< "Obj Tag ID " << obj.TagID  << "\t" << obj.mdl_ref  << " Pos: \t";
			//std::cout << obj.Position.x << '\t' << obj.Position.y << '\t' << obj.Position.z << '\n';
#endif



			InputBoundingBoxes();
			

			//if(TESTCASE::SPHERE_PCA || TESTCASE::SPHERE_RITTER || TESTCASE::SPHERE_LARSSON || TESTCASE::AABB_BOUNDING_VOLUME || TESTCASE::ORIGINAL_SPHERE)
			
			//if(obj.TagID > POWERPLANT)
				obj.Draw();

		}
		else if (PROJECT_THREE)
		{

		}
		else
		{
		
		}


	}

	// Print FPS every second.
	if (IsTimeElapsed(1))
	{
		std::stringstream sStr;


		sStr << "Assignment 2 - Geometry Toolbox" << " | " << std::fixed
			<< std::setprecision(2) << "FPS: " << FPS();


		glfwSetWindowTitle(newWindow, sStr.str().c_str());
	}

}

/**************************************************************************
* @brief Draw via Shaders
* @return void
*************************************************************************/
void Graphics::Draw()
{
	// Use shader
	assetManager.shdrpgms[shd_ref].Use();


	// Bind vertex array to VAOID
	glBindVertexArray(models[mdl_ref].vaoid); // Bind object's VAO handle


	// send data to shaders
	GLint uniform_var_loc1 = glGetUniformLocation(assetManager.shdrpgms[0].GetHandle(), "colorSet");
	if (uniform_var_loc1 >= 0) {
		glUniform3fv(uniform_var_loc1, 1, glm::value_ptr(graphicObject.setColor));
	}
	else {
		std::cout << "Uniform 'colorSet' doesn't exist!!!" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	uniform_var_loc1 = glGetUniformLocation(assetManager.shdrpgms[0].GetHandle(), "size");
	if (uniform_var_loc1 >= 0) {
		glUniform1f(uniform_var_loc1, graphicObject.size);
	}
	else {
		std::cout << "Uniform 'size' doesn't exist!!!" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	uniform_var_loc1 = glGetUniformLocation(assetManager.shdrpgms[0].GetHandle(), "uModel_to_NDC");
	if (uniform_var_loc1 >= 0) {
		glUniformMatrix4fv(uniform_var_loc1, 1, GL_FALSE, glm::value_ptr(Graphics::model_To_NDC_xform));
	}

	if (PROJECT_ONE)
	{
		if (TagID == 1)
		{
			graphicObject.setColor = graphicObject.setColorLeft;
		}
		if (TagID == 2)
		{
			graphicObject.setColor = graphicObject.setColorRight;
		}
		if (mdl_ref != POINTT)
			glDrawElements(models[mdl_ref].primitive_type,
				models[mdl_ref].draw_cnt,
				GL_UNSIGNED_SHORT, nullptr);
		else if (mdl_ref == POINTT)
			glDrawArrays(models[POINTT].primitive_type, 0, models[POINTT].draw_cnt);
		else if (mdl_ref == ARROW)
		{
			// Draw the shaft (line)
			glDrawElements(GL_LINES, 2, GL_UNSIGNED_SHORT, 0);

			// Draw the arrowhead (triangle)
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, (void*)(2 * sizeof(GLushort)));
		}

	}
	else if (PROJECT_TWO)
	{
	

		if ( mdl_ref == BOX || mdl_ref == SPHERE)
		{
			graphicObject.setColor = graphicObject.setColorRight;

			glDrawElements(models[mdl_ref].primitive_type,
				models[mdl_ref].draw_cnt,
				GL_UNSIGNED_SHORT, nullptr);
		}
		if(mdl_ref >= POWERPLANT)
		{
			graphicObject.setColor = graphicObject.setColorLeft;
			glDrawElements(models[mdl_ref].primitive_type,
				models[mdl_ref].draw_cnt,
				GL_UNSIGNED_INT, nullptr);
		}
		else
		{
		}

	}


	glBindVertexArray(0); // Unbind VAO

	assetManager.shdrpgms[shd_ref].UnUse();	// Unuse the Shader
}



void Graphics::DrawModel()
{
	assetManager.shdrpgms[shd_ref].Use();
	glBindVertexArray(models[mdl_ref].vaoid);


	// send data to shaders
	GLint uniform_var_loc1 = glGetUniformLocation(assetManager.shdrpgms[0].GetHandle(), "colorSet");
	if (uniform_var_loc1 >= 0) {
		glUniform3fv(uniform_var_loc1, 1, glm::value_ptr(graphicObject.setColor));
	}
	else {
		std::cout << "Uniform 'colorSet' doesn't exist!!!" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	uniform_var_loc1 = glGetUniformLocation(assetManager.shdrpgms[0].GetHandle(), "size");
	if (uniform_var_loc1 >= 0) {
		glUniform1f(uniform_var_loc1, graphicObject.size);
	}
	else {
		std::cout << "Uniform 'size' doesn't exist!!!" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	uniform_var_loc1 = glGetUniformLocation(assetManager.shdrpgms[0].GetHandle(), "uModel_to_NDC");
	if (uniform_var_loc1 >= 0) {
		glUniformMatrix4fv(uniform_var_loc1, 1, GL_FALSE, glm::value_ptr(Graphics::model_To_NDC_xform));
	}




	glBindVertexArray(0);
	assetManager.shdrpgms[shd_ref].UnUse();
}

/**************************************************************************
* @brief Update Object
* @param glm::vec3 set New Position
* @param glm::vec3 Scale
* @param glm::vec3 angle
* @return void
*************************************************************************/
void Graphics::Update(glm::vec3 newPosition, glm::vec3 Scale, glm::vec3 angle) {
	//std::cout << "Object Update\n";
	// Compute the angular displacement in radians

	using glm::radians;

	//Scale the model based on float variable.
	ScaleModel = glm::vec3(Scale.x, Scale.y, Scale.z);
	//scaleModel = glm::vec2(100, 100);

	// Increase the position based on the xSpeed of the user.
	// i.e if the user acceleration is 0, then speed increase
	// of xAccel is 0, and position would not change.
	Position = glm::vec3(newPosition.x, newPosition.y, newPosition.z);
	//position = glm::vec2(0, 0);

	// Boolean from the user to set if rotation is yes or no.


	// Compute the scale matrix
	glm::mat4 ScaledVector = glm::mat4(
		ScaleModel.x, 0.0f, 0.0f, 0.0f,
		0.0f, ScaleModel.y, 0.0f, 0.0f,
		0.0f, 0.0f, ScaleModel.z, 0.0f,
		0.0f, 0.0f,		0.0f	, 1.0f
	);



	// Matrix for translation
	glm::mat4 Translate = glm::mat4{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		Position.x, Position.y, Position.z, 1
	};


	glm::mat4 Rotation_x_Axis = glm::mat4{
		1, 0, 0, 0,
		0, cosf(radians(angle.x)), sinf(radians(angle.x)), 0,
		0, -sinf(radians(angle.x)), cosf(radians(angle.x)), 0,
		0, 0, 0, 1
	};

	glm::mat4 Rotation_z_Axis = glm::mat4{
		cosf(radians(angle.z)), sinf(radians(angle.z)), 0, 0,
		-sinf(radians(angle.z)), cosf(radians(angle.z)), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	glm::mat4 Rotation_y_Axis = glm::mat4{
	   cosf(radians(angle.y)), 0, -sinf(radians(angle.y)), 0,
	   0, 1, 0, 0,
	   sinf(radians(angle.y)), 0, cosf(radians(angle.y)), 0,
	   0, 0, 0, 1
	};

	// Matrix for rotation
	glm::mat4 Rotation = Rotation_z_Axis * Rotation_y_Axis * Rotation_x_Axis;



	// in the case of orthographic perspective
	// Not in use but put here to be able to recreate next time.
	float scaleX = 2.0f / windowSize.first;
	float scaleY = 2.0f / windowSize.second;
	float scaleZ = 2.0f / 1000000000;


	//Orthographic Projection
	// Scale to world NDC-coordinates matrix
	glm::mat4 ScaleToWorldToNDC = glm::mat4{
		scaleX, 0, 0, 0,
		0, scaleY, 0, 0,
		0, 0, scaleZ, 0,
		0, 0, 0, 1
	};

	// Define camera parameters

	glm::vec3 cameraPosition = glm::vec3(graphicObject.PositionLeft.x, graphicObject.PositionLeft.y, graphicObject.PositionLeft.z + 3.0f);
	// PrintData("Cam Pos: ", cameraPosition);
	glm::vec3 cameraTarget = TESTCASE::mouseEnabled ? glm::vec3(LookAt[0], LookAt[1], LookAt[2]) 
		: glm::vec3(graphicObject.PositionLeft.x, graphicObject.PositionLeft.y, graphicObject.PositionLeft.z);
	//glm::vec3 cameraTarget = glm::vec3(LookAt[0], LookAt[1], LookAt[2]);
	//PrintData("Cam TGT : ", cameraTarget);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	// Compute the view matrix
	glm::mat4 viewMatrix = glm::lookAt(cameraPosition, cameraTarget, cameraUp);


	float fov = 60.0f; // Field of view in degrees
	float aspectRatio = static_cast<float>(windowSize.first) / static_cast<float>(windowSize.second);
	float nearPlane = 1.0f;
	float farPlane = 1000000.0f;

	// Gives the zoom in/out feel
	glm::mat4 PerspectiveProjection = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);


	// SRT
	model_To_NDC_xform = viewMatrix * Translate * Rotation * ScaledVector;
	// Compute the model-to-world-to-NDC transformation matrix
	model_To_NDC_xform = PerspectiveProjection * model_To_NDC_xform;

}







/**************************************************************************
* @brief Box Model
* @return OpenGL Model
*************************************************************************/
Graphics::OpenGLModel Graphics::BoxModel() {
	struct Vertex {
		glm::vec3 position;  // Vertex position
	};

	// Define the vertices of a cube
	std::vector<Vertex> vertices{
		// Front face
		{ glm::vec3(0.5f,  0.5f, 0.5f) }, // 0 Top-right
		{ glm::vec3(0.5f, -0.5f, 0.5f) }, // 1 Bottom-right
		{ glm::vec3(-0.5f, -0.5f, 0.5f) }, // 2 Bottom-left
		{ glm::vec3(-0.5f,  0.5f, 0.5f) }, // 3 Top-left

		// Back face
		{ glm::vec3(0.5f,  0.5f, -0.5f) }, // 4 Top-right
		{ glm::vec3(0.5f, -0.5f, -0.5f) }, // 5 Bottom-right
		{ glm::vec3(-0.5f, -0.5f, -0.5f) }, // 6 Bottom-left
		{ glm::vec3(-0.5f,  0.5f, -0.5f) }, // 7 Top-left

		// Left face
		{ glm::vec3(-0.5f,  0.5f,  0.5f) }, // 8 Top-right
		{ glm::vec3(-0.5f, -0.5f,  0.5f) }, // 9 Bottom-right
		{ glm::vec3(-0.5f, -0.5f, -0.5f) }, // 10 Bottom-left
		{ glm::vec3(-0.5f,  0.5f, -0.5f) }, // 11 Top-left

		// Right face
		{ glm::vec3(0.5f,  0.5f,  0.5f) }, // 12 Top-right
		{ glm::vec3(0.5f, -0.5f,  0.5f) }, // 13 Bottom-right
		{ glm::vec3(0.5f, -0.5f, -0.5f) }, // 14 Bottom-left
		{ glm::vec3(0.5f,  0.5f, -0.5f) }, // 15 Top-left

		// Top face
		{ glm::vec3(0.5f,  0.5f,  0.5f) }, // 16 Top-right
		{ glm::vec3(-0.5f,  0.5f,  0.5f) }, // 17 Bottom-right
		{ glm::vec3(-0.5f,  0.5f, -0.5f) }, // 18 Bottom-left
		{ glm::vec3(0.5f,  0.5f, -0.5f) }, // 19 Top-left

		// Bottom face
		{ glm::vec3(0.5f, -0.5f,  0.5f) }, // 20 Top-right
		{ glm::vec3(-0.5f, -0.5f,  0.5f) }, // 21 Bottom-right
		{ glm::vec3(-0.5f, -0.5f, -0.5f) }, // 22 Bottom-left
		{ glm::vec3(0.5f, -0.5f, -0.5f) }  // 23 Top-left
	};

	// Define the indices for the 12 triangles that make up the cube (two per face)
	std::vector<GLushort> indices = {
		// Front face
		0, 1, 2, 2, 3, 0,
		// Back face
		4, 5, 6, 6, 7, 4,
		// Left face
		8, 9, 10, 10, 11, 8,
		// Right face
		12, 13, 14, 14, 15, 12,
		// Top face
		16, 17, 18, 18, 19, 16,
		// Bottom face
		20, 21, 22, 22, 23, 20
	};

	// Create Model.
	OpenGLModel mdl;

	// Create and bind a buffer for vertex data
	GLuint vbo_hdl;
	glCreateBuffers(1, &vbo_hdl);

	// Allocate and fill data store for vertices
	glNamedBufferStorage(vbo_hdl, sizeof(Vertex) * vertices.size(), vertices.data(), GL_DYNAMIC_STORAGE_BIT);

	// Create and bind a vertex array object (VAO)
	GLuint vaoid;
	glCreateVertexArrays(1, &vaoid);

	// Vertex position attribute
	glEnableVertexArrayAttrib(vaoid, 0);
	glVertexArrayVertexBuffer(vaoid, 0, vbo_hdl, offsetof(Vertex, position), sizeof(Vertex));
	glVertexArrayAttribFormat(vaoid, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoid, 0, 0);

	// Set up index buffer for rendering
	GLuint ebo_hdl;
	glCreateBuffers(1, &ebo_hdl);
	glNamedBufferStorage(ebo_hdl, sizeof(GLushort) * indices.size(), indices.data(), GL_DYNAMIC_STORAGE_BIT);
	glVertexArrayElementBuffer(vaoid, ebo_hdl);

	// Store information in the model structure
	mdl.Name = "Box Model";
	mdl.vaoid = vaoid;
	mdl.primitive_type = GL_TRIANGLES; // Use GL_TRIANGLES for the cube
	mdl.draw_cnt = static_cast<GLsizei>(indices.size());
	mdl.primitive_cnt = vertices.size();


	// return model
	return mdl;
}



/**************************************************************************
* @brief Sphere Model
* @return OpenGL Model
*************************************************************************/
Graphics::OpenGLModel Graphics::SphereModel() {
	struct Vertex {
		glm::vec3 position;
	};

	std::vector<Vertex> vertices;
	std::vector<GLushort> indices;


	const float PI = 3.14159265359f;
	const int latitudeBands = 40;
	const int longitudeBands = 40;
	const float radius = 0.5f;

	for (int latNumber = 0; latNumber <= latitudeBands; ++latNumber) {
		float theta = latNumber * PI / latitudeBands;
		float sinTheta = sin(theta);
		float cosTheta = cos(theta);

		for (int longNumber = 0; longNumber <= longitudeBands; ++longNumber) {
			float phi = longNumber * 2 * PI / longitudeBands;
			float sinPhi = sin(phi);
			float cosPhi = cos(phi);

			glm::vec3 position = glm::vec3(
				cosPhi * sinTheta * radius,
				cosTheta * radius,
				sinPhi * sinTheta * radius
			);
			vertices.push_back({ position });

			int first = (latNumber * (longitudeBands + 1)) + longNumber;
			int second = first + longitudeBands + 1;

			if (latNumber < latitudeBands && longNumber < longitudeBands) {
				indices.push_back(first);
				indices.push_back(second);
				indices.push_back(first + 1);

				indices.push_back(second);
				indices.push_back(second + 1);
				indices.push_back(first + 1);
			}
		}
	}

	OpenGLModel mdl;

	GLuint vbo_hdl;
	glCreateBuffers(1, &vbo_hdl);
	glNamedBufferStorage(vbo_hdl, sizeof(Vertex) * vertices.size(), vertices.data(), GL_DYNAMIC_STORAGE_BIT);

	GLuint vaoid;
	glCreateVertexArrays(1, &vaoid);

	glEnableVertexArrayAttrib(vaoid, 0);
	glVertexArrayVertexBuffer(vaoid, 0, vbo_hdl, offsetof(Vertex, position), sizeof(Vertex));
	glVertexArrayAttribFormat(vaoid, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoid, 0, 0);

	GLuint ebo_hdl;
	glCreateBuffers(1, &ebo_hdl);
	glNamedBufferStorage(ebo_hdl, sizeof(GLushort) * indices.size(), indices.data(), GL_DYNAMIC_STORAGE_BIT);
	glVertexArrayElementBuffer(vaoid, ebo_hdl);

	mdl.Name = "Sphere Model";
	mdl.vaoid = vaoid;
	mdl.primitive_type = GL_TRIANGLES;
	mdl.draw_cnt = static_cast<GLsizei>(indices.size());
	mdl.primitive_cnt = vertices.size();

	return mdl;
}



/**************************************************************************
* @brief Point Model
* @return OpenGL Model
*************************************************************************/
Graphics::OpenGLModel Graphics::PointModel() {
	struct Vertex {
		glm::vec3 position;
	};

	// Define a single point vertex
	std::vector<Vertex> vertices{
		{ glm::vec3(0.0f, 0.0f, 0.0f) }
	};

	// Create Model
	OpenGLModel mdl;

	// Create and bind a buffer for vertex data
	GLuint vbo_hdl;
	glCreateBuffers(1, &vbo_hdl);
	if (vbo_hdl == 0) {
		std::cerr << "Error creating vertex buffer." << std::endl;
		return mdl;
	}

	glNamedBufferStorage(vbo_hdl, sizeof(Vertex) * vertices.size(), vertices.data(), GL_DYNAMIC_STORAGE_BIT);

	// Create and bind a vertex array object (VAO)
	GLuint vaoid;
	glCreateVertexArrays(1, &vaoid);
	if (vaoid == 0) {
		std::cerr << "Error creating vertex array object." << std::endl;
		return mdl;
	}

	// Vertex position attribute
	glEnableVertexArrayAttrib(vaoid, 0);
	glVertexArrayVertexBuffer(vaoid, 0, vbo_hdl, offsetof(Vertex, position), sizeof(Vertex));
	glVertexArrayAttribFormat(vaoid, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoid, 0, 0);

	// Store information in the model structure
	mdl.Name = "Point Model";
	mdl.vaoid = vaoid;
	mdl.primitive_type = GL_POINTS; // Use GL_POINTS for the point
	mdl.draw_cnt = static_cast<GLsizei>(vertices.size());
	mdl.primitive_cnt = vertices.size();

	// Check for OpenGL errors
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		std::cerr << "OpenGL error: " << err << std::endl;
	}

	// return model
	return mdl;
}


/**************************************************************************
* @brief Ray Model
* @return OpenGL Model
*************************************************************************/
Graphics::OpenGLModel Graphics::RayModel() {
	struct Vertex {
		glm::vec3 position;
	};

	// Define the vertices of a ray (line segment)
	std::vector<Vertex> vertices{
		{ glm::vec3(0.0f, 0.0f, 0.0f) }, // Start point
		{ glm::vec3(1.0f, 0.0f, 0.0f) }  // End point
	};

	// Define the indices for the line
	std::vector<GLushort> indices = { 0, 1 };

	// Create Model.
	OpenGLModel mdl;

	// Create and bind a buffer for vertex data
	GLuint vbo_hdl;
	glCreateBuffers(1, &vbo_hdl);
	glNamedBufferStorage(vbo_hdl, sizeof(Vertex) * vertices.size(), vertices.data(), GL_DYNAMIC_STORAGE_BIT);

	// Create and bind a vertex array object (VAO)
	GLuint vaoid;
	glCreateVertexArrays(1, &vaoid);

	// Vertex position attribute
	glEnableVertexArrayAttrib(vaoid, 0);
	glVertexArrayVertexBuffer(vaoid, 0, vbo_hdl, offsetof(Vertex, position), sizeof(Vertex));
	glVertexArrayAttribFormat(vaoid, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoid, 0, 0);

	// Set up index buffer for rendering
	GLuint ebo_hdl;
	glCreateBuffers(1, &ebo_hdl);
	glNamedBufferStorage(ebo_hdl, sizeof(GLushort) * indices.size(), indices.data(), GL_DYNAMIC_STORAGE_BIT);
	glVertexArrayElementBuffer(vaoid, ebo_hdl);

	// Store information in the model structure
	mdl.Name = "Ray Model";
	mdl.vaoid = vaoid;
	mdl.primitive_type = GL_LINES; // Use GL_LINES for the ray
	mdl.draw_cnt = static_cast<GLsizei>(indices.size());
	mdl.primitive_cnt = vertices.size();

	// return model
	return mdl;
}


/**************************************************************************
* @brief Plane Model
* @return OpenGL Model
*************************************************************************/
Graphics::OpenGLModel Graphics::PlaneModel() {
	struct Vertex {
		glm::vec3 position;
	};

	// Define the vertices of a plane (quad)
	std::vector<Vertex> vertices{
		{ glm::vec3(-0.5f, 0.0f,  0.5f) }, // Top-left
		{ glm::vec3(0.5f, 0.0f,  0.5f) }, // Top-right
		{ glm::vec3(-0.5f, 0.0f, -0.5f) }, // Bottom-left
		{ glm::vec3(0.5f, 0.0f, -0.5f) }  // Bottom-right
	};

	// Define the indices for the two triangles that make up the plane
	std::vector<GLushort> indices = {
		0, 1, 2, // First triangle
		2, 1, 3  // Second triangle
	};

	// Create Model.
	OpenGLModel mdl;

	// Create and bind a buffer for vertex data
	GLuint vbo_hdl;
	glCreateBuffers(1, &vbo_hdl);
	glNamedBufferStorage(vbo_hdl, sizeof(Vertex) * vertices.size(), vertices.data(), GL_DYNAMIC_STORAGE_BIT);

	// Create and bind a vertex array object (VAO)
	GLuint vaoid;
	glCreateVertexArrays(1, &vaoid);

	// Vertex position attribute
	glEnableVertexArrayAttrib(vaoid, 0);
	glVertexArrayVertexBuffer(vaoid, 0, vbo_hdl, offsetof(Vertex, position), sizeof(Vertex));
	glVertexArrayAttribFormat(vaoid, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoid, 0, 0);

	// Set up index buffer for rendering
	GLuint ebo_hdl;
	glCreateBuffers(1, &ebo_hdl);
	glNamedBufferStorage(ebo_hdl, sizeof(GLushort) * indices.size(), indices.data(), GL_DYNAMIC_STORAGE_BIT);
	glVertexArrayElementBuffer(vaoid, ebo_hdl);

	// Store information in the model structure
	mdl.Name = "Plane Model";
	mdl.vaoid = vaoid;
	mdl.primitive_type = GL_TRIANGLES; // Use GL_TRIANGLES for the plane
	mdl.draw_cnt = static_cast<GLsizei>(indices.size());
	mdl.primitive_cnt = vertices.size();

	// return model
	return mdl;
}


/**************************************************************************
* @brief Triangle Model
* @return OpenGL Model
*************************************************************************/
Graphics::OpenGLModel Graphics::TriangleModel() {
	struct Vertex {
		glm::vec3 position;
	};

	// Define the vertices of a triangle
	std::vector<Vertex> vertices{
		{ glm::vec3(0.0f, 0.5f, 0.0f) },  // Top vertex
		{ glm::vec3(-0.5f, -0.5f, 0.0f) }, // Bottom-left vertex
		{ glm::vec3(0.5f, -0.5f, 0.0f) }  // Bottom-right vertex
	};

	// Define the indices for the triangle
	std::vector<GLushort> indices = { 0, 1, 2 };

	// Create Model.
	OpenGLModel mdl;

	// Create and bind a buffer for vertex data
	GLuint vbo_hdl;
	glCreateBuffers(1, &vbo_hdl);
	glNamedBufferStorage(vbo_hdl, sizeof(Vertex) * vertices.size(), vertices.data(), GL_DYNAMIC_STORAGE_BIT);

	// Create and bind a vertex array object (VAO)
	GLuint vaoid;
	glCreateVertexArrays(1, &vaoid);

	// Vertex position attribute
	glEnableVertexArrayAttrib(vaoid, 0);
	glVertexArrayVertexBuffer(vaoid, 0, vbo_hdl, offsetof(Vertex, position), sizeof(Vertex));
	glVertexArrayAttribFormat(vaoid, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoid, 0, 0);

	// Set up index buffer for rendering
	GLuint ebo_hdl;
	glCreateBuffers(1, &ebo_hdl);
	glNamedBufferStorage(ebo_hdl, sizeof(GLushort) * indices.size(), indices.data(), GL_DYNAMIC_STORAGE_BIT);
	glVertexArrayElementBuffer(vaoid, ebo_hdl);

	// Store information in the model structure
	mdl.Name = "Triangle Model";
	mdl.vaoid = vaoid;
	mdl.primitive_type = GL_TRIANGLES; // Use GL_TRIANGLES for the triangle
	mdl.draw_cnt = static_cast<GLsizei>(indices.size());
	mdl.primitive_cnt = vertices.size();

	// return model
	return mdl;
}


/**************************************************************************
* @brief Arrow Model
* @return OpenGL Model
*************************************************************************/
Graphics::OpenGLModel Graphics::ArrowModel() {
	struct Vertex {
		glm::vec3 position;
	};

	// Define the vertices of the arrow
	std::vector<Vertex> vertices{
		// Line part (shaft)
		{ glm::vec3(0.0f, 0.0f, 0.0f) }, // Start of the line
		{ glm::vec3(0.0f, 0.7f, 0.0f) }, // End of the line (base of the arrowhead)

		// Arrowhead part (triangle)
		{ glm::vec3(-0.1f, 0.7f, 0.0f) }, // Left vertex of the arrowhead base
		{ glm::vec3(0.1f, 0.7f, 0.0f) },  // Right vertex of the arrowhead base
		{ glm::vec3(0.0f, 1.0f, 0.0f) }
	};

	std::vector<GLushort> indices = {
		// Line (shaft)
		0, 1,

		// Triangle (arrowhead)
		2,3,4, // The order matters to form a triangle correctly

		// I dont know why but i need to do this.
		2,4,3,

		4,2,3
	};

	// Create Model.
	OpenGLModel mdl;

	// Create and bind a buffer for vertex data
	GLuint vbo_hdl;
	glCreateBuffers(1, &vbo_hdl);
	glNamedBufferStorage(vbo_hdl, sizeof(Vertex) * vertices.size(), vertices.data(), GL_DYNAMIC_STORAGE_BIT);

	// Create and bind a vertex array object (VAO)
	GLuint vaoid;
	glCreateVertexArrays(1, &vaoid);

	// Vertex position attribute
	glEnableVertexArrayAttrib(vaoid, 0);
	glVertexArrayVertexBuffer(vaoid, 0, vbo_hdl, offsetof(Vertex, position), sizeof(Vertex));
	glVertexArrayAttribFormat(vaoid, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoid, 0, 0);

	// Set up index buffer for rendering
	GLuint ebo_hdl;
	glCreateBuffers(1, &ebo_hdl);
	glNamedBufferStorage(ebo_hdl, sizeof(GLushort) * indices.size(), indices.data(), GL_DYNAMIC_STORAGE_BIT);
	glVertexArrayElementBuffer(vaoid, ebo_hdl);

	// Store information in the model structure
	mdl.Name = "Arrow Model";
	mdl.vaoid = vaoid;
	mdl.primitive_type = GL_LINES; // Initial type, will change in rendering
	mdl.draw_cnt = static_cast<GLsizei>(indices.size());
	mdl.primitive_cnt = vertices.size();

	// return model
	return mdl;
}




Graphics::OpenGLModel Graphics::PowerPlantModel() {
	ObjData& objData = Objectdata;

	// Create Model
	OpenGLModel mdl;
	mdl.Name = "PowerPlant Model";
	mdl.primitive_type = GL_TRIANGLES;
	mdl.draw_cnt = static_cast<GLsizei>(objData.indices.size());
	mdl.primitive_cnt = static_cast<GLsizei>(objData.positions.size());
	mdl.idx_elem_cnt = objData.indices.size();

	// Create and bind a vertex array object (VAO)
	glCreateVertexArrays(1, &mdl.vaoid);

	// Create and bind a buffer for vertex data
	GLuint vbo_hdl;
	glCreateBuffers(1, &vbo_hdl);

	// Calculate total buffer size and offsets
	size_t positionSize = sizeof(glm::vec3) * objData.positions.size();
	size_t normalSize = sizeof(glm::vec3) * objData.normals.size();
	size_t totalSize = positionSize + normalSize;

	// Allocate buffer storage
	glNamedBufferStorage(vbo_hdl, totalSize, nullptr, GL_DYNAMIC_STORAGE_BIT);

	// Upload position data
	glNamedBufferSubData(vbo_hdl, 0, positionSize, objData.positions.data());

	// Upload normal data
	glNamedBufferSubData(vbo_hdl, positionSize, normalSize, objData.normals.data());

	// Set up vertex attributes
	glEnableVertexArrayAttrib(mdl.vaoid, 0); // position
	glEnableVertexArrayAttrib(mdl.vaoid, 1); // normal

	glVertexArrayVertexBuffer(mdl.vaoid, 0, vbo_hdl, 0, sizeof(glm::vec3));
	glVertexArrayAttribFormat(mdl.vaoid, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(mdl.vaoid, 0, 0);

	glVertexArrayVertexBuffer(mdl.vaoid, 1, vbo_hdl, positionSize, sizeof(glm::vec3));
	glVertexArrayAttribFormat(mdl.vaoid, 1, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(mdl.vaoid, 1, 1);

	// Set up index buffer
	GLuint ebo_hdl;
	glCreateBuffers(1, &ebo_hdl);
	glNamedBufferStorage(ebo_hdl, sizeof(GLuint) * objData.indices.size(), objData.indices.data(), GL_DYNAMIC_STORAGE_BIT);
	glVertexArrayElementBuffer(mdl.vaoid, ebo_hdl);

	// Error checking
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cerr << "OpenGL error: " << error << std::endl;
	}

	std::cout << "PowerPlant vertices: " << objData.positions.size()
		<< ", indices: " << objData.indices.size() << '\n';

	return mdl;
}


Graphics::OpenGLModel Graphics::LoadModel(const std::string& modelPath, const std::string& mtlPath) {
	Assimp::Importer importer;
	

	// Set the MTL file path before reading the model file

	// Add aiProcess_GenNormals if normals are not present in the model
	const aiScene* scene = importer.ReadFile(modelPath,
		aiProcess_Triangulate |
		aiProcess_FlipUVs |
		aiProcess_CalcTangentSpace |
		aiProcess_GenNormals);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return OpenGLModel(); // Return an empty model in case of error
	}

	// Assuming we're interested in the first mesh only
	if (scene->mNumMeshes > 0) {
		aiMesh* mesh = scene->mMeshes[0];
		return ProcessMesh(mesh, scene);
	}

	return OpenGLModel(); // Return an empty model if no meshes found
}


Graphics::OpenGLModel Graphics::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoords;
	};

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;



	glm::vec3 minCoords(FLT_MAX);
	glm::vec3 maxCoords(-FLT_MAX);

	// Process vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;

		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;

		minCoords = glm::min(minCoords, vertex.position);
		maxCoords = glm::max(maxCoords, vertex.position);


		if (mesh->HasNormals()) {
			vertex.normal.x = mesh->mNormals[i].x;
			vertex.normal.y = mesh->mNormals[i].y;
			vertex.normal.z = mesh->mNormals[i].z;
		}

		if (mesh->mTextureCoords[0]) {
			vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
			vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
		}
		else {
			vertex.texCoords = glm::vec2(0.0f, 0.0f);
		}

		vertices.push_back(vertex);


	}


	// Process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}


	glm::vec3 center = (minCoords + maxCoords) * 0.5f;
	glm::vec3 halfExtents = (maxCoords - minCoords) * 0.5f;

	PrintData("Center: ", center);

	assetManager.modelCenterCoord.push_back(center);
	assetManager.modelAABBHalfExtents.push_back(halfExtents);

	// Create OpenGLModel
	OpenGLModel model;

	// Create and bind a buffer for vertex data
	GLuint vbo;
	glCreateBuffers(1, &vbo);
	glNamedBufferStorage(vbo, sizeof(Vertex) * vertices.size(), vertices.data(), GL_DYNAMIC_STORAGE_BIT);

	// Create and bind a vertex array object (VAO)
	glCreateVertexArrays(1, &model.vaoid);

	// Vertex position attribute
	glEnableVertexArrayAttrib(model.vaoid, 0);
	glVertexArrayVertexBuffer(model.vaoid, 0, vbo, offsetof(Vertex, position), sizeof(Vertex));
	glVertexArrayAttribFormat(model.vaoid, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(model.vaoid, 0, 0);

	// Set up index buffer
	GLuint ebo;
	glCreateBuffers(1, &ebo);
	glNamedBufferStorage(ebo, sizeof(GLuint) * indices.size(), indices.data(), GL_DYNAMIC_STORAGE_BIT);
	glVertexArrayElementBuffer(model.vaoid, ebo);

	// Set OpenGLModel properties
	model.Name = mesh->mName.C_Str();
	model.primitive_type = GL_TRIANGLES;
	model.primitive_cnt = vertices.size();
	model.draw_cnt = static_cast<GLsizei>(indices.size());
	model.idx_elem_cnt = indices.size();


	std::cout << model.Name << '\t' << vertices.size() << '\n';

	return model;
}




Graphics::OpenGLModel Graphics::SphereModelMethodRITTER() {
	struct Vertex {
		glm::vec3 position;
	};

	std::vector<Vertex> vertices;
	std::vector<GLushort> indices;


	const float PI = 3.14159265359f;
	const int latitudeBands = 40;
	const int longitudeBands = 40;
	const float radius = 0.5f;

	for (int latNumber = 0; latNumber <= latitudeBands; ++latNumber) {
		float theta = latNumber * PI / latitudeBands;
		float sinTheta = sin(theta);
		float cosTheta = cos(theta);

		for (int longNumber = 0; longNumber <= longitudeBands; ++longNumber) {
			float phi = longNumber * 2 * PI / longitudeBands;
			float sinPhi = sin(phi);
			float cosPhi = cos(phi);

			glm::vec3 position = glm::vec3(
				cosPhi * sinTheta * radius,
				cosTheta * radius,
				sinPhi * sinTheta * radius
			);
			vertices.push_back({ position });

			int first = (latNumber * (longitudeBands + 1)) + longNumber;
			int second = first + longitudeBands + 1;

			if (latNumber < latitudeBands && longNumber < longitudeBands) {
				indices.push_back(first);
				indices.push_back(second);
				indices.push_back(first + 1);

				indices.push_back(second);
				indices.push_back(second + 1);
				indices.push_back(first + 1);
			}
		}
	}

	std::vector<glm::vec3> points;
	for (const auto& vertex : vertices) {
		points.push_back(vertex.position);
	}

	glm::vec4 boundingSphere;

	boundingSphere = RitterBoundingSphere(points);


	glm::vec3 center = glm::vec3(boundingSphere);
	float boundingRadius = boundingSphere.w;

	boundingRadiuses[0] = boundingRadius;

	// Adjust vertices based on the calculated bounding sphere
	for (auto& vertex : vertices) {
		vertex.position = (vertex.position - center) / boundingRadius;
	}




	OpenGLModel mdl;

	GLuint vbo_hdl;
	glCreateBuffers(1, &vbo_hdl);
	glNamedBufferStorage(vbo_hdl, sizeof(Vertex)* vertices.size(), vertices.data(), GL_DYNAMIC_STORAGE_BIT);

	GLuint vaoid;
	glCreateVertexArrays(1, &vaoid);

	glEnableVertexArrayAttrib(vaoid, 0);
	glVertexArrayVertexBuffer(vaoid, 0, vbo_hdl, offsetof(Vertex, position), sizeof(Vertex));
	glVertexArrayAttribFormat(vaoid, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoid, 0, 0);

	GLuint ebo_hdl;
	glCreateBuffers(1, &ebo_hdl);
	glNamedBufferStorage(ebo_hdl, sizeof(GLushort)* indices.size(), indices.data(), GL_DYNAMIC_STORAGE_BIT);
	glVertexArrayElementBuffer(vaoid, ebo_hdl);

	mdl.Name = "Sphere Model";
	mdl.vaoid = vaoid;
	mdl.primitive_type = GL_TRIANGLES;
	mdl.draw_cnt = static_cast<GLsizei>(indices.size());
	mdl.primitive_cnt = vertices.size();

	return mdl;

	std::cout << "Bounding Sphere Center: " << boundingSphere.x << ", " << boundingSphere.y << ", " << boundingSphere.z << ", Radius: " << boundingSphere.w << std::endl;


}


Graphics::OpenGLModel Graphics::SphereModelMethodLARSSON() {
	struct Vertex {
		glm::vec3 position;
	};

	std::vector<Vertex> vertices;
	std::vector<GLushort> indices;


	const float PI = 3.14159265359f;
	const int latitudeBands = 40;
	const int longitudeBands = 40;
	const float radius = 0.5f;

	for (int latNumber = 0; latNumber <= latitudeBands; ++latNumber) {
		float theta = latNumber * PI / latitudeBands;
		float sinTheta = sin(theta);
		float cosTheta = cos(theta);

		for (int longNumber = 0; longNumber <= longitudeBands; ++longNumber) {
			float phi = longNumber * 2 * PI / longitudeBands;
			float sinPhi = sin(phi);
			float cosPhi = cos(phi);

			glm::vec3 position = glm::vec3(
				cosPhi * sinTheta * radius,
				cosTheta * radius,
				sinPhi * sinTheta * radius
			);
			vertices.push_back({ position });

			int first = (latNumber * (longitudeBands + 1)) + longNumber;
			int second = first + longitudeBands + 1;

			if (latNumber < latitudeBands && longNumber < longitudeBands) {
				indices.push_back(first);
				indices.push_back(second);
				indices.push_back(first + 1);

				indices.push_back(second);
				indices.push_back(second + 1);
				indices.push_back(first + 1);
			}
		}
	}

	std::vector<glm::vec3> points;
	for (const auto& vertex : vertices) {
		points.push_back(vertex.position);
	}

	glm::vec4 boundingSphere;

	boundingSphere = LarssonBoundingSphere(points);


	glm::vec3 center = glm::vec3(boundingSphere);
	float boundingRadius = boundingSphere.w;


	boundingRadiuses[1] = boundingRadius;

	// Adjust vertices based on the calculated bounding sphere
	for (auto& vertex : vertices) {
		vertex.position = (vertex.position - center) / boundingRadius;
	}




	OpenGLModel mdl;

	GLuint vbo_hdl;
	glCreateBuffers(1, &vbo_hdl);
	glNamedBufferStorage(vbo_hdl, sizeof(Vertex) * vertices.size(), vertices.data(), GL_DYNAMIC_STORAGE_BIT);

	GLuint vaoid;
	glCreateVertexArrays(1, &vaoid);

	glEnableVertexArrayAttrib(vaoid, 0);
	glVertexArrayVertexBuffer(vaoid, 0, vbo_hdl, offsetof(Vertex, position), sizeof(Vertex));
	glVertexArrayAttribFormat(vaoid, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoid, 0, 0);

	GLuint ebo_hdl;
	glCreateBuffers(1, &ebo_hdl);
	glNamedBufferStorage(ebo_hdl, sizeof(GLushort) * indices.size(), indices.data(), GL_DYNAMIC_STORAGE_BIT);
	glVertexArrayElementBuffer(vaoid, ebo_hdl);

	mdl.Name = "Sphere Model";
	mdl.vaoid = vaoid;
	mdl.primitive_type = GL_TRIANGLES;
	mdl.draw_cnt = static_cast<GLsizei>(indices.size());
	mdl.primitive_cnt = vertices.size();

	return mdl;

	std::cout << "Bounding Sphere Center: " << boundingSphere.x << ", " << boundingSphere.y << ", " << boundingSphere.z << ", Radius: " << boundingSphere.w << std::endl;

	//return mdl;
}

Graphics::OpenGLModel Graphics::SphereModelMethodPCA() {
	struct Vertex {
		glm::vec3 position;
	};

	std::vector<Vertex> vertices;
	std::vector<GLushort> indices;


	const float PI = 3.14159265359f;
	const int latitudeBands = 40;
	const int longitudeBands = 40;
	const float radius = 0.5f;

	for (int latNumber = 0; latNumber <= latitudeBands; ++latNumber) {
		float theta = latNumber * PI / latitudeBands;
		float sinTheta = sin(theta);
		float cosTheta = cos(theta);

		for (int longNumber = 0; longNumber <= longitudeBands; ++longNumber) {
			float phi = longNumber * 2 * PI / longitudeBands;
			float sinPhi = sin(phi);
			float cosPhi = cos(phi);

			glm::vec3 position = glm::vec3(
				cosPhi * sinTheta * radius,
				cosTheta * radius,
				sinPhi * sinTheta * radius
			);
			vertices.push_back({ position });

			int first = (latNumber * (longitudeBands + 1)) + longNumber;
			int second = first + longitudeBands + 1;

			if (latNumber < latitudeBands && longNumber < longitudeBands) {
				indices.push_back(first);
				indices.push_back(second);
				indices.push_back(first + 1);

				indices.push_back(second);
				indices.push_back(second + 1);
				indices.push_back(first + 1);
			}
		}
	}

	std::vector<glm::vec3> points;
	for (const auto& vertex : vertices) {
		points.push_back(vertex.position);
	}

	glm::vec4 boundingSphere;

	boundingSphere = PCABoundingSphere(points);
	

	glm::vec3 center = glm::vec3(boundingSphere);
	float boundingRadius = boundingSphere.w;


	boundingRadiuses[2] = boundingRadius;

	// Adjust vertices based on the calculated bounding sphere
	for (auto& vertex : vertices) {
		vertex.position = (vertex.position - center) / boundingRadius;
	}




	OpenGLModel mdl;

	GLuint vbo_hdl;
	glCreateBuffers(1, &vbo_hdl);
	glNamedBufferStorage(vbo_hdl, sizeof(Vertex) * vertices.size(), vertices.data(), GL_DYNAMIC_STORAGE_BIT);

	GLuint vaoid;
	glCreateVertexArrays(1, &vaoid);

	glEnableVertexArrayAttrib(vaoid, 0);
	glVertexArrayVertexBuffer(vaoid, 0, vbo_hdl, offsetof(Vertex, position), sizeof(Vertex));
	glVertexArrayAttribFormat(vaoid, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoid, 0, 0);

	GLuint ebo_hdl;
	glCreateBuffers(1, &ebo_hdl);
	glNamedBufferStorage(ebo_hdl, sizeof(GLushort) * indices.size(), indices.data(), GL_DYNAMIC_STORAGE_BIT);
	glVertexArrayElementBuffer(vaoid, ebo_hdl);

	mdl.Name = "Sphere Model";
	mdl.vaoid = vaoid;
	mdl.primitive_type = GL_TRIANGLES;
	mdl.draw_cnt = static_cast<GLsizei>(indices.size());
	mdl.primitive_cnt = vertices.size();

	return mdl;

	std::cout << "Bounding Sphere Center: " << boundingSphere.x << ", " << boundingSphere.y << ", " << boundingSphere.z << ", Radius: " << boundingSphere.w << std::endl;

	//return mdl;
}

void Graphics::InputBoundingBoxes()
{
	//for (int i = 0; i < assetManager.modelCenterCoord.size(); ++i)
	//{
		//PrintData("Coordinates Center: ", assetManager.modelCenterCoord[i]);
	//	PrintData("Coordaintes Half Extents: ", assetManager.modelAABBHalfExtents[i]);
	//}


	for (int i = 0; i < assetManager.txtFile.size(); ++i)
	{
		AABBContainer[i].m_Center = assetManager.modelCenterCoord[i];
		AABBContainer[i].m_HalfExtents = assetManager.modelAABBHalfExtents[i];



		//SphereContainer[i].m_Position = assetManager.modelCenterCoord[i];
		//SphereContainer[i].m_Radius = std::max({ assetManager.modelAABBHalfExtents[i].x, assetManager.modelAABBHalfExtents[i].y, assetManager.modelAABBHalfExtents[i].z });
	
		
	
	}





}