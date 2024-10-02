// SpriteAnimation.h
#ifndef SPRITEANIMATION_H
#define SPRITEANIMATION_H

#include "IAnimation.h"

class SpriteAnimation : public IAnimation {
public:
    void Play() override;
    void Pause() override;
    void Stop() override;
    void SetLooping(bool loop) override;
    float GetDuration() const override;
};

#endif
