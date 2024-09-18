// SkeletalAnimation.h
#ifndef SKELETALANIMATION_H
#define SKELETALANIMATION_H

#include "IAnimation.h"

class SkeletalAnimation : public IAnimation {
public:
    void Play() override;
    void Pause() override;
    void Stop() override;
    void SetLooping(bool loop) override;
    float GetDuration() const override;
};

#endif
