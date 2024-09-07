/**************************************************************************
 * @file BoundingVolume.cpp
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
#include "BoundingVolume.h"


/**************************************************************************
* @brief Print glm::vec3 paramters XYZ to easier debug
* @param void [Prints Data]
*************************************************************************/
void PrintData(const char* name, glm::vec3 &data)
{
    
    std::cout << name << ": " << data.x << '\t' << data.y << '\t' << data.z << '\n';
}

void PrintData2(const char* name, glm::vec3& data)
{

    std::cout << name << ": " << data.x << '\t' << data.y << '\t' << data.z << '\t';
}

/**************************************************************************
* @brief Creating Sphere
* @param void
*************************************************************************/
void CreateSphere()
{
    glm::vec3 Position{ 0.0f,0.0f,0.0f };
    float Radius = 0.5f;
    
    Sphere SphereBVolume(Position, Radius);

    SphereContainer.emplace_back(SphereBVolume);
}

/**************************************************************************
* @brief Creating AABB
* @param void
*************************************************************************/
void CreateAABB()
{
    glm::vec3 Position{ 0.0f,0.0f,0.0f };
    glm::vec3 HalfPosition{ 0.5f,0.5f,0.5f };

    AABB AABB_BVolume(Position,HalfPosition);
    
    AABBContainer.emplace_back(AABB_BVolume);
}

/**************************************************************************
* @brief Creating Point
* @param void
*************************************************************************/
void CreatePoint()
{
    Point createPoint;

    createPoint.point = glm::vec3(0.0f, 0.0f, 0.0f);
    
    PointContainer.emplace_back(createPoint);
}

/**************************************************************************
* @brief Creating Plane
* @param void
*************************************************************************/
void CreatePlane()
{
    Plane createPlane(glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,0.0f,0.0f));

    PlaneContainer.emplace_back(createPlane);
}


/**************************************************************************
* @brief Creating Ray
* @param void
*************************************************************************/
void CreateRay()
{
    glm::vec3 RayStart{ 0.0f,0.0f,0.0f };
    glm::vec3 RayEnd{ 1.0f, 0.0f, 0.0f };

    Ray createRay;

    createRay.startPoint = RayStart;
    createRay.endPoint = RayEnd;

    RayContainer.emplace_back(createRay);
}

/**************************************************************************
* @brief Creating Triangle
* @param void
*************************************************************************/
void CreateTriangle()
{
    Triangles triangle;

    triangle.firstVertex = glm::vec3(0.0f, 0.5f, 0.0f);
    triangle.secondVertex = glm::vec3(-0.5f, -0.5f, 0.0f);
    triangle.thirdVertex = glm::vec3(0.5f, -0.5f, 0.0f);

    TriangleContainer.emplace_back(triangle);
}


/**************************************************************************
* @brief Checking Collisions
* @return bool Collided (TRUE)  | no Collisions (FALSE)
*************************************************************************/
bool checkCollisionAABB(const AABB& BB1, const AABB& BB2) 
{
    glm::vec3 BB1_MinPos = BB1.m_Center - BB1.m_HalfExtents;
    glm::vec3 BB1_MaxPos = BB1.m_Center + BB1.m_HalfExtents;


    glm::vec3 BB2_MinPos = BB2.m_Center - BB2.m_HalfExtents;
    glm::vec3 BB2_MaxPos = BB2.m_Center + BB2.m_HalfExtents;

    std::cout << "BB1 Min Position " << BB1_MinPos.x << '\t' << BB1_MinPos.y << '\t' << BB1_MinPos.z << '\n';
    std::cout << "BB1 Max Position " << BB1_MaxPos.x << '\t' << BB1_MaxPos.y << '\t' << BB1_MaxPos.z << '\n';



    std::cout << "BB2 Min Position " << BB2_MinPos.x << '\t' << BB2_MinPos.y << '\t' << BB2_MinPos.z << '\n';
    std::cout << "BB2 Max Position " << BB2_MaxPos.x << '\t' << BB2_MaxPos.y << '\t' << BB2_MaxPos.z << "\n\n";

    if (BB1_MaxPos.x < BB2_MinPos.x || BB1_MinPos.x > BB2_MaxPos.x) {
        return false;
    }

    // Check for overlap in the y dimension
    if (BB1_MaxPos.y < BB2_MinPos.y || BB1_MinPos.y > BB2_MaxPos.y) {
        return false;
    }

    // Check for overlap in the z dimension
    if (BB1_MaxPos.z < BB2_MinPos.z || BB1_MinPos.z > BB2_MaxPos.z) {
        return false;
    }

    // If none of the above conditions were met, the boxes intersect
    return true;
}


/**************************************************************************
* @brief Checking Collisions
* @return bool Collided (TRUE)  | no Collisions (FALSE)
*************************************************************************/
bool checkCollisionAABB_SPHERE(const Sphere& BB1, const AABB& BB2) {
    // Calculate the min and max positions for AABB
    glm::vec3 aabbMin = BB2.m_Center - BB2.m_HalfExtents;
    glm::vec3 aabbMax = BB2.m_Center + BB2.m_HalfExtents;

    // Find the closest point on the AABB to the sphere's center
    glm::vec3 closestPoint;

    // Calculate closest point for x-coordinate
    if (BB1.m_Position.x < aabbMin.x)
        closestPoint.x = aabbMin.x;
    else if (BB1.m_Position.x > aabbMax.x)
        closestPoint.x = aabbMax.x;
    else
        closestPoint.x = BB1.m_Position.x;

    // Calculate closest point for y-coordinate
    if (BB1.m_Position.y < aabbMin.y)
        closestPoint.y = aabbMin.y;
    else if (BB1.m_Position.y > aabbMax.y)
        closestPoint.y = aabbMax.y;
    else
        closestPoint.y = BB1.m_Position.y;

    // Calculate closest point for z-coordinate
    if (BB1.m_Position.z < aabbMin.z)
        closestPoint.z = aabbMin.z;
    else if (BB1.m_Position.z > aabbMax.z)
        closestPoint.z = aabbMax.z;
    else
        closestPoint.z = BB1.m_Position.z;

    // Calculate the distance between the BB1's center and this closest point
    glm::vec3 distance = closestPoint - BB1.m_Position;
    float distanceSquared = glm::dot(distance, distance);

    // Check if the distance is less than or equal to the BB1's radius squared
    if (distanceSquared <= (BB1.m_Radius * BB1.m_Radius)) {
        return true;
    }
    else {
        return false;
    }
}


/**************************************************************************
* @brief Checking Collisions
* @return bool Collided (TRUE)  | no Collisions (FALSE)
*************************************************************************/
bool checkCollisionAABB_SPHERE(const AABB& BB1, const Sphere& BB2) {
    // Calculate the min and max positions for AABB
    glm::vec3 aabbMin = BB1.m_Center - BB1.m_HalfExtents;
    glm::vec3 aabbMax = BB1.m_Center + BB1.m_HalfExtents;

    // Find the closest point on the AABB to the sphere's center
    glm::vec3 closestPoint;


    // Calculate closest point for x-coordinate
    if (BB2.m_Position.x < aabbMin.x)
        closestPoint.x = aabbMin.x;
    else if (BB2.m_Position.x > aabbMax.x)
        closestPoint.x = aabbMax.x;
    else
        closestPoint.x = BB2.m_Position.x;

    // Calculate closest point for y-coordinate
    if (BB2.m_Position.y < aabbMin.y)
        closestPoint.y = aabbMin.y;
    else if (BB2.m_Position.y > aabbMax.y)
        closestPoint.y = aabbMax.y;
    else
        closestPoint.y = BB2.m_Position.y;

    // Calculate closest point for z-coordinate
    if (BB2.m_Position.z < aabbMin.z)
        closestPoint.z = aabbMin.z;
    else if (BB2.m_Position.z > aabbMax.z)
        closestPoint.z = aabbMax.z;
    else
        closestPoint.z = BB2.m_Position.z;

    // Calculate the distance between the BB2's center and this closest point
    glm::vec3 distance = closestPoint - BB2.m_Position;
    float distanceSquared = glm::dot(distance, distance);

    // Check if the distance is less than or equal to the BB2's radius squared
    if (distanceSquared <= (BB2.m_Radius * BB2.m_Radius)) {
        return true;
    }
    else {
        return false;
    }
}


/**************************************************************************
* @brief Checking Collisions
* @return bool Collided (TRUE)  | no Collisions (FALSE)
*************************************************************************/
bool checkCollisionSphere(const Sphere& BB1, const Sphere& BB2) {
    // Calculate the distance between the centers of the spheres
    glm::vec3 distanceVec = BB2.m_Position - BB1.m_Position;
    float distanceSquared = glm::dot(distanceVec, distanceVec);

    // Calculate the sum of the radii squared
    float sumRadiiSquared = (BB1.m_Radius + BB2.m_Radius) * (BB1.m_Radius + BB2.m_Radius);

    // Check if the distance is less than or equal to the sum of the radii squared
    if (distanceSquared <= sumRadiiSquared) {
        return true;
    }
    else {
        return false;
    }
}


/**************************************************************************
* @brief Checking Collisions
* @return bool Collided (TRUE)  | no Collisions (FALSE)
*************************************************************************/
bool checkCollisionRaySphere(const Ray& BB1, const Sphere& BB2) {
    glm::vec3 aabbMin = BB1.startPoint;
    glm::vec3 aabbMax = BB1.endPoint;

    // Find the closest point on the AABB to the sphere's center
    glm::vec3 closestPoint;


    // Calculate closest point for x-coordinate
    if (BB2.m_Position.x < aabbMin.x)
        closestPoint.x = aabbMin.x;
    else if (BB2.m_Position.x > aabbMax.x)
        closestPoint.x = aabbMax.x;
    else
        closestPoint.x = BB2.m_Position.x;

    // Calculate closest point for y-coordinate
    if (BB2.m_Position.y < aabbMin.y)
        closestPoint.y = aabbMin.y;
    else if (BB2.m_Position.y > aabbMax.y)
        closestPoint.y = aabbMax.y;
    else
        closestPoint.y = BB2.m_Position.y;

    // Calculate closest point for z-coordinate
    if (BB2.m_Position.z < aabbMin.z)
        closestPoint.z = aabbMin.z;
    else if (BB2.m_Position.z > aabbMax.z)
        closestPoint.z = aabbMax.z;
    else
        closestPoint.z = BB2.m_Position.z;

    // Calculate the distance between the BB2's center and this closest point
    glm::vec3 distance = closestPoint - BB2.m_Position;
    float distanceSquared = glm::dot(distance, distance);

    // Check if the distance is less than or equal to the BB2's radius squared
    if (distanceSquared <= (BB2.m_Radius * BB2.m_Radius)) {
        return true;
    }
    else {
        return false;
    }
}



/**************************************************************************
* @brief Checking Collisions
* @return bool Collided (TRUE)  | no Collisions (FALSE)
*************************************************************************/
bool checkCollisionPointSphere(const Point& BB1, const Sphere& BB2)
{
    // Calculate the distance vector between the point and the center of the sphere
    glm::vec3 distanceVec = BB1.point - BB2.m_Position;

    // Calculate the squared distance
    float distanceSquared = glm::dot(distanceVec, distanceVec);

    // Calculate the squared radius of the sphere
    float radiusSquared = BB2.m_Radius * BB2.m_Radius;

    // Check if the distance is less than or equal to the radius
    if (distanceSquared <= radiusSquared) {
        return true; // There is a collision
    }
    else {
        return false; // No collision
    }
}


/**************************************************************************
* @brief Checking Collisions
* @return bool Collided (TRUE)  | no Collisions (FALSE)
*************************************************************************/
bool checkCollisionPointAABB(const Point& BB1, const AABB& BB2) {
    glm::vec3 minPos = BB2.m_Center - BB2.m_HalfExtents;
    glm::vec3 maxPos = BB2.m_Center + BB2.m_HalfExtents;

    if (BB1.point.x >= minPos.x && BB1.point.x <= maxPos.x &&
        BB1.point.y >= minPos.y && BB1.point.y <= maxPos.y &&
        BB1.point.z >= minPos.z && BB1.point.z <= maxPos.z) {
        return true;
    }
    else {
        return false;
    }
}


/**************************************************************************
* @brief Checking Collisions
* @return bool Collided (TRUE)  | no Collisions (FALSE)
*************************************************************************/
bool checkCollisionPointPlane(const Point& point, const Plane& plane) {
    // Calculate the vertices of the plane's AABB
    glm::vec3 vertex1 = plane.m_CenterPosition - plane.m_HalfExtents;
    glm::vec3 secondVertex = plane.m_CenterPosition + plane.m_HalfExtents;

    // Find the closest point on the AABB to the point
    glm::vec3 closestPoint;

    std::cout << plane.m_HalfExtents.x << '\n';

    // Calculate closest point for x-coordinate
    if (point.point.x < plane.m_CenterPosition.x - plane.m_HalfExtents.x)
        closestPoint.x = plane.m_CenterPosition.x - plane.m_HalfExtents.x;
    else if (point.point.x > plane.m_CenterPosition.x + plane.m_HalfExtents.x)
        closestPoint.x = plane.m_CenterPosition.x + plane.m_HalfExtents.x;
    else
        closestPoint.x = point.point.x;

    // Calculate closest point for y-coordinate
    if (point.point.y < vertex1.y)
        closestPoint.y = vertex1.y;
    else if (point.point.y > secondVertex.y)
        closestPoint.y = secondVertex.y;
    else
        closestPoint.y = point.point.y;

    // Calculate closest point for z-coordinate
    if (point.point.z < vertex1.z)
        closestPoint.z = vertex1.z;
    else if (point.point.z > secondVertex.z)
        closestPoint.z = secondVertex.z;
    else
        closestPoint.z = point.point.z;

    // Calculate the distance between the point and this closest point
    glm::vec3 distance = closestPoint - point.point;
    float distanceSquared = glm::dot(distance, distance);

    // Check if the distance is negligible (considering some epsilon)
    return glm::abs(distanceSquared) < 1e-6; // You can adjust the epsilon value as needed
}



/**************************************************************************
* @brief Checking Collisions
* @return bool Collided (TRUE)  | no Collisions (FALSE)
*************************************************************************/
bool checkCollisionPointTriangle(const Point& BB1, const Triangles& BB2) {
    glm::vec3 v0 = BB2.secondVertex - BB2.firstVertex;
    glm::vec3 v1 = BB2.thirdVertex - BB2.firstVertex;
    glm::vec3 v2 = BB1.point - BB2.firstVertex;

    float d00 = glm::dot(v0, v0);
    float d01 = glm::dot(v0, v1);
    float d11 = glm::dot(v1, v1);
    float d20 = glm::dot(v2, v0);
    float d21 = glm::dot(v2, v1);

    float denom = d00 * d11 - d01 * d01;
    if (denom == 0) {
        return false; // The triangle is degenerate
    }
    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;

    if (u >= 0 && v >= 0 && w >= 0 && u <= 1 && v <= 1 && w <= 1) {
        return true; // The point lies within the triangle
    }
    else {
        return false; // The point does not lie within the triangle
    }
}



/**************************************************************************
* @brief Checking Collisions
* @return bool Collided (TRUE)  | no Collisions (FALSE)
*************************************************************************/
bool checkCollisionRayAABB(const Ray& BB1, const AABB& BB2) {
    glm::vec3 rayDir = glm::normalize(BB1.endPoint - BB1.startPoint);
    glm::vec3 currentPoint = BB1.startPoint;
    float stepSize = 0.01f; // Adjust the step size for accuracy/speed

    glm::vec3 minPos = BB2.m_Center - BB2.m_HalfExtents;
    glm::vec3 maxPos = BB2.m_Center + BB2.m_HalfExtents;

    // Iterate along the ray from start to end point
    while (glm::length(currentPoint - BB1.startPoint) <= glm::length(BB1.endPoint - BB1.startPoint)) {
        if (currentPoint.x >= minPos.x && currentPoint.x <= maxPos.x &&
            currentPoint.y >= minPos.y && currentPoint.y <= maxPos.y &&
            currentPoint.z >= minPos.z && currentPoint.z <= maxPos.z) {
            std::cout << "Intersection Point: ("
                << currentPoint.x << ", "
                << currentPoint.y << ", "
                << currentPoint.z << ")" << std::endl;
            return true;
        }
        currentPoint += rayDir * stepSize;
    }

    return false;
}



/**************************************************************************
* @brief Checking Collisions
* @return bool Collided (TRUE)  | no Collisions (FALSE)
*************************************************************************/
bool checkCollisionRayPlane(const Ray& BB1, const Plane& BB2) {
    glm::vec3 BB1_MinPos = BB1.startPoint;
    glm::vec3 BB1_MaxPos = BB1.endPoint;


    glm::vec3 BB2_MinPos = BB2.m_CenterPosition - BB2.m_HalfExtents;
    glm::vec3 BB2_MaxPos = BB2.m_CenterPosition + BB2.m_HalfExtents;

 

    if (BB1_MaxPos.x < BB2_MinPos.x || BB1_MinPos.x > BB2_MaxPos.x) {
        return false;
    }

    // Check for overlap in the y dimension
    if (BB1_MaxPos.y < BB2_MinPos.y || BB1_MinPos.y > BB2_MaxPos.y) {
        return false;
    }

    // Check for overlap in the z dimension
    if (BB1_MaxPos.z < BB2_MinPos.z || BB1_MinPos.z > BB2_MaxPos.z) {
        return false;
    }

    // If none of the above conditions were met, the boxes intersect
    return true;
}


/**************************************************************************
* @brief Checking Collisions
* @return bool Collided (TRUE)  | no Collisions (FALSE)
*************************************************************************/
bool checkCollisionRayTriangle(const Ray& ray, const Triangles& triangle) {
    auto isInsideTriangle = [](const glm::vec3& point, const Triangles& tri) {
        glm::vec3 v0 = tri.secondVertex - tri.firstVertex;
        glm::vec3 v1 = tri.thirdVertex - tri.firstVertex;
        glm::vec3 v2 = point - tri.firstVertex;

        float d00 = glm::dot(v0, v0);
        float d01 = glm::dot(v0, v1);
        float d11 = glm::dot(v1, v1);
        float d20 = glm::dot(v2, v0);
        float d21 = glm::dot(v2, v1);

        float denom = d00 * d11 - d01 * d01;
        if (denom == 0) {
            return false; // The triangle is degenerate
        }

        float v = (d11 * d20 - d01 * d21) / denom;
        float w = (d00 * d21 - d01 * d20) / denom;
        float u = 1.0f - v - w;

        return (u >= 0 && v >= 0 && w >= 0 && u <= 1 && v <= 1 && w <= 1);
    };

    // Check if the start or end point of the ray is inside the triangle
    if (isInsideTriangle(ray.startPoint, triangle) || isInsideTriangle(ray.endPoint, triangle))
        return true;

    // Sample points along the ray and check if any of them are inside the triangle
    glm::vec3 rayDir = glm::normalize(ray.endPoint - ray.startPoint);
    float stepSize = 0.001f; // Adjust as needed
    float t = 0.0f;
    while (t <= 1.0f) {
        glm::vec3 samplePoint = ray.startPoint + t * (ray.endPoint - ray.startPoint);
        if (isInsideTriangle(samplePoint, triangle))
            return true;
        t += stepSize;
    }

    return false; // No intersection found
}

/**************************************************************************
* @brief Checking Collisions
* @return bool Collided (TRUE)  | no Collisions (FALSE)
*************************************************************************/
bool checkCollisionPlaneAABB(const Plane& plane, const AABB& aabb) {
    glm::vec3 AABB_MinPos = aabb.m_Center - aabb.m_HalfExtents;
    glm::vec3 AABB_MaxPos = aabb.m_Center + aabb.m_HalfExtents;

    glm::vec3 Plane_MinPos = plane.m_CenterPosition - plane.m_HalfExtents;
    glm::vec3 Plane_MaxPos = plane.m_CenterPosition + plane.m_HalfExtents;

    bool intersectX = AABB_MaxPos.x >= Plane_MinPos.x && AABB_MinPos.x <= Plane_MaxPos.x;
    bool intersectY = AABB_MaxPos.y >= Plane_MinPos.y && AABB_MinPos.y <= Plane_MaxPos.y;
    bool intersectZ = AABB_MaxPos.z >= Plane_MinPos.z && AABB_MinPos.z <= Plane_MaxPos.z;

    return intersectX && intersectY && intersectZ;
}


/**************************************************************************
* @brief Checking Collisions
* @return bool Collided (TRUE)  | no Collisions (FALSE)
*************************************************************************/
bool checkCollisionPlaneSphere(const Plane& plane, const Sphere& sphere) {

    // Calculate the min and max positions for AABB
    glm::vec3 aabbMin = plane.m_CenterPosition - plane.m_HalfExtents;
    glm::vec3 aabbMax = plane.m_CenterPosition + plane.m_HalfExtents;

    // Find the closest point on the AABB to the sphere's center
    glm::vec3 closestPoint;


    // Calculate closest point for x-coordinate
    if (sphere.m_Position.x < aabbMin.x)
        closestPoint.x = aabbMin.x;
    else if (sphere.m_Position.x > aabbMax.x)
        closestPoint.x = aabbMax.x;
    else
        closestPoint.x = sphere.m_Position.x;

    // Calculate closest point for y-coordinate
    if (sphere.m_Position.y < aabbMin.y)
        closestPoint.y = aabbMin.y;
    else if (sphere.m_Position.y > aabbMax.y)
        closestPoint.y = aabbMax.y;
    else
        closestPoint.y = sphere.m_Position.y;

    // Calculate closest point for z-coordinate
    if (sphere.m_Position.z < aabbMin.z)
        closestPoint.z = aabbMin.z;
    else if (sphere.m_Position.z > aabbMax.z)
        closestPoint.z = aabbMax.z;
    else
        closestPoint.z = sphere.m_Position.z;

    // Calculate the distance between the sphere's center and this closest point
    glm::vec3 distance = closestPoint - sphere.m_Position;
    float distanceSquared = glm::dot(distance, distance);

    // Check if the distance is less than or equal to the sphere's radius squared
    if (distanceSquared <= (sphere.m_Radius * sphere.m_Radius)) {
        return true;
    }
    else {
        return false;
    }
}



glm::vec4 RitterBoundingSphere(const std::vector<glm::vec3>& points) {
    glm::vec3 minCoords(FLT_MAX);
    glm::vec3 maxCoords(-FLT_MAX);

    for (const auto& point : points) {
        minCoords = glm::min(minCoords, point);
        maxCoords = glm::max(maxCoords, point);
    }

    glm::vec3 center = (minCoords + maxCoords) * 0.5f;
    float radius = glm::length(maxCoords - center);

    // Refinement step
    for (const auto& point : points) {
        float dist = glm::length(point - center);
        if (dist > radius) {
            radius = (radius + dist) * 0.5f;
            center = center + (dist - radius) / dist * (point - center);
        }
    }

    return glm::vec4(center, radius);
}


glm::vec4 LarssonBoundingSphere(const std::vector<glm::vec3>& points) {
    // Placeholder for Larsson's method
    return RitterBoundingSphere(points);
}


glm::vec3 Mean(const std::vector<glm::vec3>& points) {
    glm::vec3 mean(0.0f);
    for (const auto& point : points) {
        mean += point;
    }
    return mean / static_cast<float>(points.size());
}

std::vector<std::vector<float>> CovarianceMatrix(const std::vector<glm::vec3>& points, const glm::vec3& mean) {
    std::vector<std::vector<float>> covMatrix(3, std::vector<float>(3, 0.0f));
    for (const auto& point : points) {
        glm::vec3 centered = point - mean;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                covMatrix[i][j] += centered[i] * centered[j];
            }
        }
    }
    return covMatrix;
}

std::vector<float> Eigenvalues(const std::vector<std::vector<float>>& covMatrix) {
    // Placeholder for eigenvalue computation
    return { 1.0f, 1.0f, 1.0f };
}

std::vector<glm::vec3> Eigenvectors(const std::vector<std::vector<float>>& covMatrix) {
    // Placeholder for eigenvector computation
    return { glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f) };
}

glm::vec4 PCABoundingSphere(const std::vector<glm::vec3>& points) {
    glm::vec3 mean = Mean(points);
    auto covMatrix = CovarianceMatrix(points, mean);
    auto eigenvalues = Eigenvalues(covMatrix);
    auto eigenvectors = Eigenvectors(covMatrix);

    // Use eigenvectors to find the extremal points
    glm::vec3 minExtremal(FLT_MAX);
    glm::vec3 maxExtremal(-FLT_MAX);
    for (const auto& point : points) {
        glm::vec3 projection;
        for (int i = 0; i < 3; ++i) {
            projection[i] = glm::dot(point - mean, eigenvectors[i]);
        }
        minExtremal = glm::min(minExtremal, projection);
        maxExtremal = glm::max(maxExtremal, projection);
    }

    glm::vec3 center = (minExtremal + maxExtremal) * 0.5f;
    float radius = glm::length(maxExtremal - center);

    return glm::vec4(center, radius);
}