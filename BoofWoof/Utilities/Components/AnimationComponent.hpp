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


	bool PlayAnimation(Entity entity, float timeStart, float timeEnd)
	{
		auto& anim = g_Coordinator.GetComponent<AnimationComponent>(entity);
		auto& graphicsComp = g_Coordinator.GetComponent<GraphicsComponent>(entity);
		float animDeltaTime = anim.m_DeltaTime;

		graphicsComp.SetAnimationTime(graphicsComp.deltaTime);

		// Retrieve model name
		std::string modelName = graphicsComp.getModelName();

		// Guard: check if an animator exists for this model and is valid
		auto animatorIt = g_ResourceManager.AnimatorMap.find(modelName);
		if (animatorIt != g_ResourceManager.AnimatorMap.end() && animatorIt->second)
		{
			// Update the animation with the given delta time
			animatorIt->second->UpdateAnimation(graphicsComp.deltaTime);

			std::cout << animDeltaTime << '\t' << timeStart << '\t' << timeEnd << '\t' << '\n';
			std::cout << "Graphics Component Model name : " << modelName << '\n';

			if (animDeltaTime < timeStart || animDeltaTime > timeEnd)
			{
				std::cout << "it entered here in playAnim \n";
				animatorIt->second->SetAnimationTime(timeStart);
				return true;
			}



		}
		else
		{
			std::cerr << "Animator for model \"" << modelName << "\" not found or is null." << std::endl;
			return false;
		}

		return false;
	}


	std::vector<std::tuple<int,float,float>> animationVector;

	std::tuple<int, float, float> playThisAnimation;


	bool pauseAnimation;

private:

};

#endif // ANIMATION_COMPONENT_H
