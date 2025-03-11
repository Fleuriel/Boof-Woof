#ifndef ANIMATION_SYSTEM_H
#define ANIMATION_SYSTEM_H

#include "../assimp2016/assimp/scene.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>





// for System
#include "../ECS/System.hpp"
// Struct for keyframe data


class AnimationSystem : public System
{
public:

    //void Update(float dt)
    //{
    //    for (auto entity : allEntities)
    //    {
    //        auto& animComp = g_Coordinator.GetComponent<AnimationComponent>(entity);

    //        if (animComp.isPlaying && animComp.animator)
    //        {
    //            animComp.animator->UpdateAnimation(dt);
    //        }
    //    }
    //}
};



#endif