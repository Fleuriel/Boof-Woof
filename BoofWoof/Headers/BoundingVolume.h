/**************************************************************************
 * @file BoundingVolume.h
 * @author 	TAN Angus Yit Hoe
 * @param DP email: tan.a@digipen.edu [0067684]
 * @param Course: CS 350
 * @param Course: Advanced Computer Graphics II
 * @date  06/14/2024 (14 JUNE 2024)
 * @brief
 *
 * This file allow creation of Bounding Volumes to clip onto an existing
 * Graphic Model, while at the same time allow checking of collisions for
 * the following:
 *
 *              Sphere_vs_Sphere
 *              AABB_vs_Sphere
 *              Sphere_vs_AABB
 *              AABB_vs_AABB
 *
 *              Point_vs_Sphere
 *              Point_vs_AABB
 *              Point_vs_Plane
 *              Point_vs_Triangle
 *
 *              Ray_vs_Plane
 *              Ray_vs_Triangle
 *              Ray_vs_AABB
 *              Ray_vs_Sphere
 *
 *              Plane_vs_AABB
 *              Plane_vs_Sphere
 *
 *
 *************************************************************************/
#pragma once

#ifndef BVOLUME_H
#define BVOLUME_H

#include "Graphics.h"
#include <vector>


/**************************************************************************
* @brief AABB Class
*************************************************************************/
class AABB {
public:
	AABB(const glm::vec3& setCenter, const glm::vec3& setHalf)
		: m_Center(setCenter), m_HalfExtents(setHalf) {}

	AABB()
		: m_Center(0.0f, 0.0f, 0.0f), m_HalfExtents(0.0f, 0.0f, 0.0f) {}

	~AABB() {}

	glm::vec3 m_Center;
	glm::vec3 m_HalfExtents;
};


/**************************************************************************
* @brief OBB Class < AABB
*************************************************************************/
class OBB :public AABB
{
public:
	OBB(const glm::vec3& setCenter, const glm::vec3& setHalf, const glm::mat3& rotation)
		: AABB(setCenter, setHalf), r_rotation(rotation) {}

	OBB() : r_rotation(glm::mat3(1.0f)){};
	~OBB() {};

	glm::mat3 r_rotation;
};



/**************************************************************************
* @brief Sphere Class
*************************************************************************/
class Sphere {
public:
	Sphere(glm::vec3 setPos, float setRadius) : m_Position(setPos), m_Radius(setRadius) {};

	Sphere() : m_Position(0.0f,0.0f,0.0f), m_Radius(0.0f){};

	~Sphere() {};

	glm::vec3 m_Position;
	float m_Radius;
};


/**************************************************************************
* @brief Ray Class
*************************************************************************/
class Ray {
public:
	Ray(): startPoint(0.0f,0.0f,0.0f), endPoint(1.0f,0.0f,0.0f), rotation(glm::mat4(1.0f)){}

	glm::vec3 startPoint;
	glm::vec3 endPoint;
	glm::mat4 rotation;
	~Ray() {};
};


/**************************************************************************
* @brief Point Class
*************************************************************************/
class Point {
public:
	Point() : point(0.0f, 0.0f, 0.0f) {}
	glm::vec3 point;
	~Point() {};
};



/**************************************************************************
* @brief Plane Class
*************************************************************************/
class Plane
{
public:
	Plane(const glm::vec3& centerPos, const glm::vec3& halfExtents)
		: m_CenterPosition(centerPos), m_HalfExtents(halfExtents) {}

	glm::vec3 m_CenterPosition;
	glm::vec3 m_HalfExtents;

	~Plane() {}
};



/**************************************************************************
* @brief Triangles Class
*************************************************************************/
class Triangles {
public:
	Triangles() :firstVertex(0.0f, 0.0f, 0.0f), secondVertex(0.0f, 0.0f, 0.0f),
		thirdVertex(0.0f, 0.0f, 0.0f) {}
	glm::vec3 firstVertex;
	glm::vec3 secondVertex;
	glm::vec3 thirdVertex;
	~Triangles() {};
};


/**************************************************************************
* @brief Print glm::vec3 paramters XYZ to easier debug
*************************************************************************/
void PrintData(const char* name, glm::vec3 &data);


/**************************************************************************
* @brief Functions to create Bounding Volumes
*************************************************************************/
void CreateSphere();
void CreateAABB();
void CreatePoint();
void CreatePlane();
void CreateRay();
void CreateTriangle();

/**************************************************************************
* @brief Functions to check if collision has met with another object.
*************************************************************************/
bool checkCollisionSphere(const Sphere& BB1, const Sphere& BB2);						// Sphere Vs Sphere
bool checkCollisionAABB_SPHERE(const AABB& BB1, const Sphere& BB2);						// AABB Vs Sphere  [2 same Functions but overloaded]
bool checkCollisionAABB_SPHERE(const Sphere& BB1, const AABB& BB2);						// Sphere Vs AABB  [2 same Functions but overloaded] 
bool checkCollisionAABB(const AABB& BB1, const AABB& BB2);								// AABB Vs AABB

bool checkCollisionPointSphere(const Point& BB1, const Sphere& BB2);					// Point Vs Sphere
bool checkCollisionPointAABB(const Point& BB1, const AABB& BB2);						// Point Vs AABB
bool checkCollisionPointPlane(const Point& BB1, const Plane& BB2);						// Point Vs Plane
bool checkCollisionPointTriangle(const Point& BB1, const Triangles& BB2);				// Point Vs Triangle (Barycentric Coordintes)


bool checkCollisionRayAABB(const Ray& BB1, const AABB& BB2);							// Ray Vs AABB
bool checkCollisionRayPlane(const Ray& BB1, const Plane& BB2);							// Ray Vs Plane
bool checkCollisionRayTriangle(const Ray& BB1, const Triangles& BB2);					// Ray Vs Triangle
bool checkCollisionRaySphere(const Ray& BB1, const Sphere& BB2);						// Ray Vs Sphere

bool checkCollisionPlaneAABB(const Plane& BB1, const AABB& BB2);						// Plane Vs AABB
bool checkCollisionPlaneSphere(const Plane& BB1, const Sphere& BB2);					// Plane Vs Sphere




/**************************************************************************
* @brief Containers for each of the primitives/models above.
*************************************************************************/
inline std::vector <AABB> AABBContainer;												// AABB
inline std::vector <OBB> OBBContainer;													// OBB
inline std::vector<Sphere> SphereContainer;												// Sphere
inline std::vector<Point> PointContainer;												// Point
inline std::vector<Triangles> TriangleContainer;										// Triangle
inline std::vector<Ray> RayContainer;													// Ray
inline std::vector<Plane> PlaneContainer;												// Plane



#endif