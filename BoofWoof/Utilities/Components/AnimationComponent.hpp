#ifndef ANIMATION_COMPONENT_H
#define ANIMATION_COMPONENT_H

#include <string>
#include <memory>
#include <unordered_map>


class AnimationComponent {
public:



	std::string animatorName;
	std::string animationName;


	float m_Duration;
	float m_DeltaTime;

//	AnimationComponent() = default;


	AnimationComponent() : animatorName(" "), animationName(" "), m_Duration(0.0f), m_DeltaTime(0.0f) {}




private:

};

#endif // ANIMATION_COMPONENT_H
