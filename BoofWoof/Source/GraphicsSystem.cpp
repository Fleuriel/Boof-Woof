#include "GraphicsSystem.h"
#include "WindowComponent.h"
#include "Shader.h"
#include <utility>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <unordered_map>
#include "pch.h"
#include "TestSystem.h"
#include "Input.h"


// Assignment 1
#include "BoundingVolume.h"
#include "TestCases.h"


// Static members
GLuint GraphicsSystem::mdl_ref = 0;
GLuint GraphicsSystem::shd_ref = 0;
unsigned int GraphicsSystem::VBO = 0, GraphicsSystem::VAO = 0, GraphicsSystem::EBO = 0;

bool GraphicsSystem::glewInitialized = false;


void GraphicsSystem::initGraphicsPipeline(const GraphicsComponent& graphicsComponent) {
    // Implement graphics pipeline initialization
		// OpenGL Initialization
	std::cout << "Initializing Graphics Pipeline\n";

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


	shd_ref = 0;
}




void GraphicsSystem::UpdateLoop(GraphicsComponent& graphicsComp) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//if (inputSystem.GetKeyState(GLFW_KEY_A) == 1)
	//{
	//	std::cout << "Position left " << PositionLeft.x << '\t' << PositionLeft.y << '\t' << PositionLeft.z << '\n';
	//	std::cout << "PositionRight " << PositionRight.x << '\t' << PositionRight.y << '\t' << PositionRight.z << '\n';
	//}


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
				obj.UpdateObject(PositionLeft, glm::vec3(1.0f, 1.0f, 1.0f), changePlaneRot);
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

	// Draw Objects after calculations etc.
	DrawObject();
}

void GraphicsSystem::UpdateObject(Entity entity, GraphicsComponent& graphicsComp, const WindowComponent& windowComp, float deltaTime)
{
	using glm::radians;

	// Compute matrices
	glm::mat4 ScaleToWorldToNDC = glm::mat4{
		2.0f / windowComp.windowSize.first, 0, 0, 0,
		0, 2.0f / windowComp.windowSize.second, 0, 0,
		0, 0, 2.0f / 1000000000.0f, 0,
		0, 0, 0, 1
	};

	// Compute the scale matrix
	glm::mat4 ScaledVector = glm::mat4(
		graphicsComp.ScaleModel.x, 0.0f, 0.0f, 0.0f,
		0.0f, graphicsComp.ScaleModel.y, 0.0f, 0.0f,
		0.0f, 0.0f, graphicsComp.ScaleModel.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	glm::mat4 Translate = glm::mat4{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		graphicsComp.Position.x, graphicsComp.Position.y, graphicsComp.Position.z, 1
	};
	glm::mat4 Rotation_x_Axis = glm::mat4{
		1, 0, 0, 0,
		0, cosf(radians(graphicsComp.Angle.x)), sinf(radians(graphicsComp.Angle.x)), 0,
		0, -sinf(radians(graphicsComp.Angle.x)), cosf(radians(graphicsComp.Angle.x)), 0,
		0, 0, 0, 1
	};

	glm::mat4 Rotation_z_Axis = glm::mat4{
		cosf(radians(graphicsComp.Angle.z)), sinf(radians(graphicsComp.Angle.z)), 0, 0,
		-sinf(radians(graphicsComp.Angle.z)), cosf(radians(graphicsComp.Angle.z)), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	glm::mat4 Rotation_y_Axis = glm::mat4{
		cosf(radians(graphicsComp.Angle.y)), 0, -sinf(radians(graphicsComp.Angle.y)), 0,
		0, 1, 0, 0,
		sinf(radians(graphicsComp.Angle.y)), 0, cosf(radians(graphicsComp.Angle.y)), 0,
		0, 0, 0, 1
	};

	glm::mat4 Rotation = Rotation_z_Axis * Rotation_y_Axis * Rotation_x_Axis;

	// View and Projection matrices
	glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 5.0f);
	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 viewMatrix = glm::lookAt(cameraPosition, cameraTarget, cameraUp);

	float fov = 45.0f; // Field of view in degrees
	float aspectRatio = static_cast<float>(windowComp.windowSize.first) / static_cast<float>(windowComp.windowSize.second);
	float nearPlane = 1.0f;
	float farPlane = 100.0f;
	glm::mat4 PerspectiveProjection = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);



	// Final transformation matrix
	graphicsComp.model_To_NDC_xform = viewMatrix * Translate * Rotation * ScaledVector;

	//*ScaleToWorldToNDC;
	graphicsComp.model_To_NDC_xform = PerspectiveProjection * graphicsComp.model_To_NDC_xform;

	

}

void GraphicsSystem::Draw(std::vector<GraphicsComponent>& components) {
    // Loop through components and draw them
    for (auto& component : components) {
        DrawObject(component);
    }
}

void GraphicsSystem::DrawObject(GraphicsComponent& component) {
    // Draw logic using component data
}

void GraphicsSystem::CreateObject(OpenGLModel model, int Tag) {
    // Implement object creation
}


void GraphicsSystem::CheckTestsCollisions()
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
		if (checkCollisionAABB_SPHERE(SphereContainer[0], AABBContainer[1]))
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
		if (checkCollisionPointTriangle(PointContainer[0], TriangleContainer[1]))
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
		if (checkCollisionRayPlane(RayContainer[0], PlaneContainer[1]))
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
		if (checkCollisionRayTriangle(RayContainer[0], TriangleContainer[1]))
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
		if (checkCollisionRayAABB(RayContainer[0], AABBContainer[1]))
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
		if (checkCollisionRaySphere(RayContainer[0], SphereContainer[1]))
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
		if (checkCollisionPlaneAABB(PlaneContainer[0], AABBContainer[1]))
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
		if (checkCollisionPlaneSphere(PlaneContainer[0], SphereContainer[1]))
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
