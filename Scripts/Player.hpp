#include "../BoofWoof/Core/Logic/Behaviour.h"

struct Player final : public Behaviour
{
	using Behaviour::Behaviour;

	virtual void Update(Entity entity) override
	{	
		glm::vec3 velocity(0.0f);

		if (m_Engine.getInputSystem().isButtonPressed(GLFW_KEY_W))
		{
			std::cout << "movingW" << std::endl;
			velocity.z -= 2.f;
		}

		if (m_Engine.getInputSystem().isButtonPressed(GLFW_KEY_A))
		{
			std::cout << "movingA" << std::endl;
			velocity.x -= 2.f;
		}

		if (m_Engine.getInputSystem().isButtonPressed(GLFW_KEY_S))
		{
			std::cout << "movingS" << std::endl;
			velocity.z += 2.f;
		}

		if (m_Engine.getInputSystem().isButtonPressed(GLFW_KEY_D))
		{
			std::cout << "movingD" << std::endl;
			velocity.x += 2.f;
		}

		// Check if the entity has a collision component and physics body before setting velocity
		if (m_Engine.HaveCollisionComponent(entity) && m_Engine.HavePhysicsBody(entity))
		{
			m_Engine.SetVelocity(entity, velocity);
		}
		
		
	}

	virtual const char* getBehaviourName() override
	{
		return "Player";
	}
};
