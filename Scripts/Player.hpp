#include "../BoofWoof/Core/Logic/Behaviour.h"

struct Player final : public Behaviour
{
	using Behaviour::Behaviour;

	virtual void Update(Entity entity) override
	{	
		glm::vec3 velocity(0.0f);

		if (m_Engine.getInputSystem().isButtonPressed(GLFW_KEY_W))
		{
			velocity.z -= 1.f;
		}

		if (m_Engine.getInputSystem().isButtonPressed(GLFW_KEY_A))
		{
			velocity.x -= 1.f;
		}

		if (m_Engine.getInputSystem().isButtonPressed(GLFW_KEY_S))
		{
			velocity.z += 1.f;
		}

		if (m_Engine.getInputSystem().isButtonPressed(GLFW_KEY_D))
		{
			velocity.x += 1.f;
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
