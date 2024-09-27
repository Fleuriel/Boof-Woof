// AnimationManager.h
#ifndef ANIMATIONMANAGER_H
#define ANIMATIONMANAGER_H

#include "IAnimation.h"
#include <vector>
#include <memory>

class AnimationManager {
public:
    void AddAnimation(std::shared_ptr<IAnimation> animation);
    void PlayAll();
    void PauseAll();
    void StopAll();
private:
    std::vector<std::shared_ptr<IAnimation>> animations;
};

#endif
