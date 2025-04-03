#ifndef ANIMATION_COMPONENT_H
#define ANIMATION_COMPONENT_H

#include <string>
#include <memory>
#include <unordered_map>
#include "Animation/Animation.h"
#include "Animation/Animator.h"
#include <vector>
#include <tuple>

class AnimationComponent {
public:



	std::string animatorName;
	std::string animationName;


	float m_Duration;
	float m_DeltaTime;

//	AnimationComponent() = default;


	AnimationComponent() : animatorName(" "), animationName(" "), m_Duration(0.0f), m_DeltaTime(0.0f) {}



	AnimationComponent(std::string name, std::vector<std::tuple<int,float,float>> inputTupVec) : animatorName(name), animationName(name), m_Duration(0.0f), m_DeltaTime(0.0f) , animationVector(inputTupVec) {}



	std::vector<std::tuple<int,float,float>> animationVector;

	std::tuple<int, float, float> playThisAnimation;


	bool pauseAnimation;

private:

};

#endif // ANIMATION_COMPONENT_H
