#include "../BoofWoof/Core/Logic/Behaviour.h"

struct Player final : public Behaviour
{
	using Behaviour::Behaviour;

	virtual void Update(Entity entity) override
	{	
		
		if (m_Engine.getInputSystem().isButtonPressed(GLFW_KEY_W))
		{
			m_Engine.SetPosition(entity, m_Engine.GetPosition(entity) + glm::vec3(0.0f, 0.0f, -0.1f));
		}

		if (m_Engine.getInputSystem().isButtonPressed(GLFW_KEY_A))
		{
			m_Engine.SetPosition(entity, m_Engine.GetPosition(entity) + glm::vec3(-0.1f, 0.0f, 0.0f));
		}

		if (m_Engine.getInputSystem().isButtonPressed(GLFW_KEY_S))
		{
			m_Engine.SetPosition(entity, m_Engine.GetPosition(entity) + glm::vec3(0.0f, 0.0f, 0.1f));
		}

		if (m_Engine.getInputSystem().isButtonPressed(GLFW_KEY_D))
		{
			m_Engine.SetPosition(entity, m_Engine.GetPosition(entity) + glm::vec3(0.1f, 0.0f, 0.0f));
		}
		
	}

	virtual const char* getBehaviourName() override
	{
		return "Player";
	}
};
