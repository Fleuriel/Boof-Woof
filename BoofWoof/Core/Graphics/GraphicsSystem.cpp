#include "GraphicsSystem.h"
#include "Shader.h"
#include <utility>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <unordered_map>
#include "../ECS/pch.h"
#include "../Input/Input.h"
#include "../AssetManager/AssetManager.h"

#include "Windows/WindowManager.h"

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




void GraphicsSystem::UpdateLoop() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClearColor(0.1f, 0.2f, 0.3f, 1.0f);


}



void GraphicsSystem::UpdateObject(Entity entity, GraphicsComponent& graphicsComp, float deltaTime)
{

	using glm::radians;

	// Compute matrices
	glm::mat4 ScaleToWorldToNDC = glm::mat4{
		2.0f / g_WindowX, 0, 0, 0,
		0, 2.0f / g_WindowY, 0, 0,
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
	float aspectRatio = static_cast<float>(g_WindowX) / static_cast<float>(g_WindowY);
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


OpenGLModel SphereModel() {
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












//DEPRECIATED
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
