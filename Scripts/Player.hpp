#include "../BoofWoof/Core/Logic/Behaviour.h"

struct Player final : public Behaviour
{
	using Behaviour::Behaviour;

	virtual void Update(Entity entity) override
	{
		m_Engine.Movement(entity);
		//std::cout << "Player Update" << std::endl;

	}

	virtual const char* getBehaviourName() override
	{
		return "Player";
	}
};
