#include "pch.h"
//#include "CollisionSystem.h"
//#include <iostream>
//
//void CollisionSystem::Init()
//{
//    // Initialization logic if necessary
//    std::cout << "Collision System Initialized\n";
//}
//
//
//void CollisionSystem::Update()
//{
//    // Get all entities with the CollisionComponent
//    auto allEntities = g_Coordinator.GetAliveEntitiesSet();
//
//    for (auto it = allEntities.begin(); it != allEntities.end(); ++it)
//    {
//        Entity entityA = *it;
//        if (g_Coordinator.HaveComponent<CollisionComponent>(entityA))
//        {
//            auto& collisionCompA = g_Coordinator.GetComponent<CollisionComponent>(entityA);
//            std::cout << "LEMAO" << std::endl;
//
//            // Compare this entity with every other entity
//            for (auto jt = std::next(it); jt != allEntities.end(); ++jt)
//            {
//                Entity entityB = *jt;
//                if (g_Coordinator.HaveComponent<CollisionComponent>(entityB))
//                {
//                    auto& collisionCompB = g_Coordinator.GetComponent<CollisionComponent>(entityB);
//
//                    // Check for collision between entityA and entityB
//                    if (collisionCompA.IsCollidingWith(collisionCompB))
//                    {
//                        // Collision detected
//                        std::cout << "Collision detected between entities: " << entityA << " and " << entityB << "\n";
//
//                        // Handle collision response here if needed (e.g., stopping movement, applying forces, etc.)
//                    }
//                }
//            }
//        }
//    }
//}
