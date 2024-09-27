// IAnimation.h
#ifndef IANIMATION_H
#define IANIMATION_H

class IAnimation {
public:
    virtual void Play() = 0;
    virtual void Pause() = 0;
    virtual void Stop() = 0;
    virtual void SetLooping(bool loop) = 0;
    virtual float GetDuration() const = 0;
    virtual ~IAnimation() = default;
};

#endif
